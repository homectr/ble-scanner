#include "handlers.h"
#include "Thing.h"
#include "utils.h"
#include "Logger.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

extern Thing* thing;

bool globalCmdHandler(const HomieRange& range, const String& value){
    Logger& logger = Logger::getInstance();
    logger.logf_P(LOG_DEBUG,PSTR("[g-cmdHandler] val=%s"),value.c_str());
    
    bool updated = false;

    if (!updated) updated = thing->cmdHandler(value);

    if (updated) {
        thing->homie.setProperty("cmd").send(value);  // Update the state of the led
        Homie.getLogger() << "Cmd is " << value << endl;
    }

    return updated;
}


bool globalUpdateHandler(const HomieNode &node, const HomieRange &range, const String &property, const String &value){
    Logger& logger = Logger::getInstance();
    logger.logf_P(LOG_DEBUG,PSTR("[g-updHandler] node=%s prop=%s val=%s"),node.getId(),property.c_str(),value.c_str());
    bool updated = false;
    String newValue = value;

    updated = thing->updateHandler(node,property,value);

    if (updated) {
        node.setProperty(property).send(newValue);
        Homie.getLogger() << "Node '"<< node.getId() << "' property '" << property << "' set to " << newValue << endl;
    }

    return updated;
}

void globalOnHomieEvent(const HomieEvent& event) {
    Logger& logger = Logger::getInstance();
    const char* devId = &(Homie.getConfiguration().deviceId[0]);
    switch(event.type) {
        case HomieEventType::OTA_STARTED:
        // Do whatever you want when OTA is started
            logger.logf_P(LOG_NOTICE,PSTR("OTA started. device=%s"), devId);
        break;
        case HomieEventType::OTA_FAILED:
        // Do whatever you want when OTA is failed
            logger.logf_P(LOG_ERR,PSTR("OTA failed. device==%s"),devId);
        break;
        case HomieEventType::OTA_SUCCESSFUL:
        // Do whatever you want when OTA is successful
            logger.logf_P(LOG_NOTICE,PSTR("OTA successful. device=%s"),devId);
        break;
        case HomieEventType::ABOUT_TO_RESET:
        // Do whatever you want when the device is about to reset
            SPIFFS.end();
        break;
        case HomieEventType::WIFI_CONNECTED:
        // Do whatever you want when Wi-Fi is connected in normal mode
            logger.logf_P(LOG_NOTICE,PSTR("Wifi connected %s"),event.ip.toString().c_str());

        // You can use event.ip, event.gateway, event.mask
        break;
        case HomieEventType::MQTT_DISCONNECTED:
        // Do whatever you want when MQTT is disconnected in normal mode

        // You can use event.mqttReason
        /*
            MQTT Reason (source: https://github.com/marvinroger/async-mqtt-client/blob/master/src/AsyncMqttClient/DisconnectReasons.hpp)
            0 TCP_DISCONNECTED
            1 MQTT_UNACCEPTABLE_PROTOCOL_VERSION
            2 MQTT_IDENTIFIER_REJECTED
            3 MQTT_SERVER_UNAVAILABLE
            4 MQTT_MALFORMED_CREDENTIALS
            5 MQTT_NOT_AUTHORIZED
            6 ESP8266_NOT_ENOUGH_SPACE
            7 TLS_BAD_FINGERPRINT
        */
            logger.logf_P(LOG_ERR,PSTR("MQTT disconnected reason=%d device=%s"),event.mqttReason, devId);
        break;
        case HomieEventType::SENDING_STATISTICS:
        // Do whatever you want when statistics are sent in normal mode
            logger.logf_P(LOG_DEBUG,PSTR("Sending stats. device=%s"),devId);
        break;
    }
}
