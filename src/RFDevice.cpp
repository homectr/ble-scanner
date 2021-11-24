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
        strncpy(buf,DEVICE_STR_SENSOR_TEMP,bufSize);
        break;
    case RFSensorType::CONTACT: 
        strncpy(buf,DEVICE_STR_SENSOR_CONTACT,bufSize);
        break;
    case RFSensorType::HUMIDITY: 
        strncpy(buf,DEVICE_STR_SENSOR_HUMIDITY,bufSize);
        break;
    
    default:
        break;
    }
    bufPos = strlen(buf);
    snprintf(buf+bufPos,bufSize-bufPos-1,"%08X",id);
    this->idStr = strdup(buf);
    this->homie = homie;
}

RFDevice::~RFDevice(){
    free(idStr);
}

RFSensorTemp::RFSensorTemp(uint32_t id, HomieNode *homie):RFDevice(RFSensorType::TEMPERATURE, id, homie){
    DEBUG_PRINT(PSTR("[RFS] Creating sensor type=Temperature id=%s\n"),idStr);
    homie->advertise(this->idStr).setDatatype("float").setRetained(true);
}

void RFSensorTemp:: update(RFSensorPayload& payload){
    temp = strtod((char*)payload, nullptr);

    // update Homie property
    if (Homie.isConnected()) homie->setProperty(idStr).send(String(temp));
    CONSOLE(PSTR("%d Sensor-Temp id=0x%X temp=%.2f\n"),millis(),id,temp);
}

RFSensorContact::RFSensorContact(uint32_t id, HomieNode *homie):RFDevice(RFSensorType::CONTACT, id, homie){
    DEBUG_PRINT(PSTR("[RFS] Creating sensor type=Contact id=%s\n"),idStr);
    homie->advertise(this->idStr).setDatatype("boolean").setRetained(true);
}

void RFSensorContact::update(RFSensorPayload& payload){
    char buf[10];
    strncpy(buf,(char*)payload,10);
    open = strcmp(buf,"0") == 0;

    // update Homie property
    if (Homie.isConnected()) homie->setProperty(idStr).send(open?"false":"true");
    CONSOLE(PSTR("%d Sensor-Contact id=0x%X status=%d\n"),millis(),id,!open);
}

RFSensorHumidity::RFSensorHumidity(uint32_t id, HomieNode *homie):RFDevice(RFSensorType::HUMIDITY, id, homie){
    DEBUG_PRINT(PSTR("[RFS] Creating sensor type=Humidity id=%s\n"),idStr);
    homie->advertise(this->idStr).setDatatype("float").setRetained(true);
}

void RFSensorHumidity:: update(RFSensorPayload& payload){
    char* e;
    hum = strtod((char*)payload, &e);
        
    // update Homie property
    if (Homie.isConnected()) homie->setProperty(idStr).send(String(hum));
    CONSOLE(PSTR("%d Sensor-Hum id=0x%X rh=%.2f\n"),millis(),id,hum);
}
