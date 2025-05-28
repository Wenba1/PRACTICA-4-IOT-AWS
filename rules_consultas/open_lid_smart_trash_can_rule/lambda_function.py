
import json
import boto3
import time

client = boto3.client('iot-data', region_name='us-east-1')

def lambda_handler(event, context):
    try:
        thing_name = event.get('thing_name', None)

        if thing_name is None:
            raise ValueError("Missing 'thing_name'")

        topic = f"$aws/things/{thing_name}/shadow/update"

        # 1. Abrir tapa
        open_payload = {
            "state": {
                "desired": {
                    "lid_open": True
                }
            }
        }

        client.publish(topic=topic, qos=1, payload=json.dumps(open_payload))

        # 2. Esperar 4 segundos
        time.sleep(4)

        # 3. Cerrar tapa
        close_payload = {
            "state": {
                "desired": {
                    "lid_open": False
                }
            }
        }

        client.publish(topic=topic, qos=1, payload=json.dumps(close_payload))

        return {
            'statusCode': 200,
            'body': 'Tapa abierta y cerrada automáticamente.'
        }

    except Exception as e:
        print(f"Exception: {e}")
        return {
            'statusCode': 500,
            'body': str(e)
        }


