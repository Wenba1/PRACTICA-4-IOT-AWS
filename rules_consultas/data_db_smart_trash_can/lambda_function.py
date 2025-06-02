import boto3
from decimal import Decimal
import json
import time

dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('smart_trash_can_data')

def lambda_handler(event, context):
    print("Raw Event:", event)

    try:
        thing_name = event.get('thing_name', 'unknown')
        timestamp = event.get('timestamp', int(time.time()))

        raw_depth = event.get('depth_cm', 0)
        depth = round(raw_depth, 1)
        if depth <= 0 or depth > 15:
            print(f"Invalid depth_cm value: {depth}. Skipping insert.")
            return {'statusCode': 400, 'message': f'Invalid depth_cm value: {depth}'}

        depth = Decimal(str(depth)) 

        motion_detected = event.get('motion_detected')
        lid_open = event.get('lid_open')
        filling_state = event.get('filling_state')
        serial_numner = event.get('serial_number')

        last_item = get_last_item(thing_name)
        print("Last item:", last_item)

        should_save = (
            not last_item or
            abs(depth - last_item.get('depth_cm', Decimal('0'))) >= Decimal('0.5') or
            filling_state != last_item.get('filling_state') or
            lid_open != last_item.get('lid_open') or
            motion_detected != last_item.get('motion_detected')
        )

        if should_save:
            item = {
                'thing_name': thing_name,
                'timestamp': timestamp,
                'depth_cm': depth,
                'lid_open': lid_open,
                'filling_state': filling_state,
                'serial_number': serial_numner
            }

            print("Saving to DynamoDB:", item)
            table.put_item(Item=item)
            return {'statusCode': 200, 'message': 'Inserted'}
        else:
            print("No significant change. Skipping insert.")
            return {'statusCode': 200, 'message': 'Skipped'}

    except Exception as e:
        print(f"Error: {e}")
        return {'statusCode': 500, 'message': str(e)}


def get_last_item(thing_name):
    try:
        response = table.query(
            KeyConditionExpression=boto3.dynamodb.conditions.Key('thing_name').eq(thing_name),
            ScanIndexForward=False,  
            Limit=1
        )
        items = response.get('Items')
        return items[0] if items else None
    except Exception as e:
        print(f"Error getting last item: {e}")
        return None
