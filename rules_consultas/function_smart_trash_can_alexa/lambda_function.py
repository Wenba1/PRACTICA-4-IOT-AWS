import logging
import json
import boto3
import ask_sdk_core.utils as ask_utils
from ask_sdk_core.skill_builder import SkillBuilder
from ask_sdk_core.dispatch_components import AbstractRequestHandler
from ask_sdk_core.dispatch_components import AbstractExceptionHandler
from ask_sdk_core.handler_input import HandlerInput
from ask_sdk_core.utils import is_request_type, is_intent_name


from ask_sdk_model import Response

logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

iot_client = boto3.client('iot-data', region_name='us-east-1', endpoint_url='https://ahxlba0g2uxk8-ats.iot.us-east-1.amazonaws.com')

USER_THING_MAP = {
    "amzn1.ask.account.AMARIIZAJ566BX3GJCR4YAZL2FQFKJAPVQZLPYYGC2432YWNJNYBIGUXYUGBDTTQZ5ENEV2AV7ZDFJ2JCGCMAWFXB5YBZHO57QTD5DDZLRG3O67JAVLYZ54ADIF7F6PWDKOWGRYWLX4T4ILSTLAZNQVU33TD4TXOZOTPNCXYJZV6JYAIZ7Z6RPA6L6WFCBUZTQ2HN6X66VVAZ3KV4ZHB7HUUQZS5OBPXCXD5UXGN2W5Q": "smart_trash_can_sn0001"
}

def get_thing_name(handler_input):
    user_id = handler_input.request_envelope.session.user.user_id
    logger.info(f"User ID: {user_id}")
    return USER_THING_MAP.get(user_id)

def get_shadow_state(thing_name):
    response = iot_client.get_thing_shadow(thingName=thing_name)
    payload = json.loads(response['payload'].read())
    return payload.get("state", {}).get("reported", {})

def update_shadow_state(desired_state):
    payload = {
        "state": {
            "desired": desired_state
        }
    }
    iot_client.update_thing_shadow(thingName=THING_NAME, payload=json.dumps(payload))

class LaunchRequestHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return ask_utils.is_request_type("LaunchRequest")(handler_input)

    def handle(self, handler_input):
        speak_output = "Welcome to your smart trash can. You can ask me to open the lid, enable automatic mode, or get the status."
        reprompt = "What would you like to do?"
        return (
            handler_input.response_builder
                .speak(speak_output)
                .ask(reprompt)
                .response
        )


class GetTrashCanStatusIntentHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return is_intent_name("GetTrashCanStatusIntent")(handler_input)

    def handle(self, handler_input):
        thing_name = get_thing_name(handler_input)
        if not thing_name:
            speech = "Sorry, I couldn't identify your assigned trash can. Please make sure your device is registered."
            reprompt = "Can you try again or check your device registration?"
            return handler_input.response_builder.speak(speech).ask(reprompt).response

        status = get_shadow_state(thing_name)

        if not status:
            speech = "I couldn't get the trash can's status at the moment. Please try again later."
            reprompt = "Would you like to try again or do something else?"
            return handler_input.response_builder.speak(speech).ask(reprompt).response

        speech = (
            f"The trash can is currently {'open' if status.get('lid_open') else 'closed'}, "
            f"with a depth of {status.get('depth_cm', 'unknown')} centimeters, "
            f"LED color is {status.get('led_color', 'unknown')}, and the bin is {status.get('filling_state', 'unknown')}."
        )
        reprompt = "Would you like to do something else with the trash can?"
        return handler_input.response_builder.speak(speech).ask(reprompt).response


class OpenLidIntentHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return is_intent_name("OpenLidIntent")(handler_input)

    def handle(self, handler_input):
        thing_name = get_thing_name(handler_input)
        if not thing_name:
            speech = "I'm sorry, I couldn't find your assigned trash can."
            reprompt = "Please check your device and try again."
            return handler_input.response_builder.speak(speech).ask(reprompt).response
        try:
            response = iot_client.get_thing_shadow(thingName=thing_name)
            shadow_payload = json.loads(response['payload'].read())
            lid_open_state = shadow_payload.get("state", {}).get("reported", {}).get("lid_open", False)
        except Exception as e:
            speech = "I'm having trouble checking the current lid state."
            reprompt = "Would you like to try again?"
            return handler_input.response_builder.speak(speech).ask(reprompt).response

        if lid_open_state:
            speech = "The lid is already open."
            reprompt = "Would you like to do something else?"
            return handler_input.response_builder.speak(speech).ask(reprompt).response

        iot_client.update_thing_shadow(
            thingName=thing_name,
            payload=json.dumps({
                "state": {
                    "desired": {
                        "lid_open": True     
                    }
                }
            })
        )

        speech = "Opening the trash can lid."
        reprompt = "Is there anything else you want to do?"
        return handler_input.response_builder.speak(speech).ask(reprompt).response


class CloseLidIntentHandler(AbstractRequestHandler):
    def can_handle(self, handler_input):
        return is_intent_name("CloseLidIntent")(handler_input)

    def handle(self, handler_input):
        thing_name = get_thing_name(handler_input)
        if not thing_name:
            speech = "I'm sorry, I couldn't find your assigned trash can."
            reprompt = "Please check your device and try again."
            return handler_input.response_builder.speak(speech).ask(reprompt).response
        try:
            response = iot_client.get_thing_shadow(thingName=thing_name)
            shadow_payload = json.loads(response['payload'].read())
            lid_open_state = shadow_payload.get("state", {}).get("reported", {}).get("lid_open", True)
        except Exception as e:
            speech = "I'm having trouble checking the current lid state."
            reprompt = "Would you like to try again?"
            return handler_input.response_builder.speak(speech).ask(reprompt).response

        if not lid_open_state:
            speech = "The lid is already closed."
            reprompt = "Would you like to do anything else?"
            return handler_input.response_builder.speak(speech).ask(reprompt).response

        iot_client.update_thing_shadow(
            thingName=thing_name,
            payload=json.dumps({
                "state": {
                    "desired": {
                        "lid_open": False     
                    }
                }
            })
        )

        speech = "Closing the trash can lid."
        reprompt = "Is there anything else you want to do?"
        return handler_input.response_builder.speak(speech).ask(reprompt).response


class CatchAllExceptionHandler(AbstractExceptionHandler):
    def can_handle(self, handler_input, exception):
        return True

    def handle(self, handler_input, exception):
        logger.error(exception, exc_info=True)
        speech = "Sorry, something went wrong. Please try again."
        return handler_input.response_builder.speak(speech).ask(speech).response

sb = SkillBuilder()
sb.add_request_handler(LaunchRequestHandler())
sb.add_request_handler(GetTrashCanStatusIntentHandler())
sb.add_request_handler(OpenLidIntentHandler())
sb.add_request_handler(CloseLidIntentHandler())
sb.add_exception_handler(CatchAllExceptionHandler())

lambda_handler = sb.lambda_handler()