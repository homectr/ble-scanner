#include "RFDevice.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

RFDevice::RFDevice(RFSensorType type, uint32_t id, HomieNode *homie){
    this->type=type; 
    this->id=id;
    char buf[20];
    uint16_t bufSize = sizeof(buf);
    uint16_t bufPos = 0;

    memset(buf, 0, bufSize);
    
    switch (type) {
    case RFSensorType::TEMPERATURE: 
        strncpy(buf,"temp_",bufSize);
        break;
    case RFSensorType::CONTACT: 
        strncpy(buf,"contact_",bufSize);
        break;
    case RFSensorType::HUMIDITY: 
        strncpy(buf,"humidity_",bufSize);
        break;
    
    default:
        break;
    }
    bufPos = strlen(buf);
    snprintf(buf+bufPos,bufSize-bufPos-1,"%08X",id);
    this->idStr = strdup(buf);
    this->homie = homie;
    
    // each device shall have a property for triggering identification processs of such device
    String s = String(idStr)+"_identify";
    homie->advertise(strdup(s.c_str())).setDatatype("boolean").settable().setRetained(false);
}

RFSensorTemp::RFSensorTemp(uint32_t id, HomieNode *homie):RFDevice(RFSensorType::TEMPERATURE, id, homie){
    DEBUG_PRINT("[RFS] Creating sensor type=Temperature id=%s\n",idStr);
    homie->advertise(this->idStr).setDatatype("float");
}

void RFSensorTemp:: update(RFSensorPayload& payload){
    char* e;
    temp = strtod((char*)payload, &e);

    // update Homie property
    if (Homie.isConnected()) homie->setProperty(idStr).send(String(temp));
    Homie.getLogger() << millis() << " Sensor-Temp " << idStr << " temp=" << temp << endl;
}

RFSensorContact::RFSensorContact(uint32_t id, HomieNode *homie):RFDevice(RFSensorType::CONTACT, id, homie){
    DEBUG_PRINT("[RFS] Creating sensor type=Contact id=%s\n",idStr);
    homie->advertise(this->idStr).setDatatype("boolean");
}

void RFSensorContact::update(RFSensorPayload& payload){
    open = payload[0];

    // update Homie property
    if (Homie.isConnected()) homie->setProperty(idStr).send(open?"true":"false");
    Homie.getLogger() << millis() << " Sensor-Contact " << idStr << " contact=" << open << endl;
}

RFSensorHumidity::RFSensorHumidity(uint32_t id, HomieNode *homie):RFDevice(RFSensorType::HUMIDITY, id, homie){
    DEBUG_PRINT("[RFS] Creating sensor type=Humidity id=%s\n",idStr);
    homie->advertise(this->idStr).setDatatype("float");
}

void RFSensorHumidity:: update(RFSensorPayload& payload){
    char* e;
    hum = strtod((char*)payload, &e);
        
    // update Homie property
    if (Homie.isConnected()) homie->setProperty(idStr).send(String(hum));
    Homie.getLogger() << millis() << " Sensor-Hum " << idStr << " hum=" << hum << endl;
}
