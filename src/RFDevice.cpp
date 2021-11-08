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
    
    switch (type)
    {
    case RFSensorType::TEMPERATURE: 
        memcpy(buf,"temp_",bufSize);
        break;
    case RFSensorType::CONTACT: 
        memcpy(buf,"contact_",bufSize);
        break;
    
    default:
        break;
    }
    bufPos = strlen(buf);
    snprintf(buf,bufSize-bufPos-1,"%08X",id);
    this->idStr = strdup(buf);
    this->homie = homie;
}

RFSensorTemp::RFSensorTemp(uint32_t id, HomieNode *homie):RFDevice(TEMPERATURE, id, homie){
    DEBUG_PRINT("[RFS] Creating sensor type=Temperature id=%s\n",idStr);
    homie->advertise("temp").setDatatype("Number:Temperature");
}

void RFSensorTemp:: update(RFSensorPayload payload){
    memcpy((void*)&temp,(void*)&payload,4);
    
    // update Homie property
    if (Homie.isConnected()) homie->setProperty(idStr).send(String(temp));
    Homie.getLogger() << millis() << " Sensor-Temp " << idStr << "=" << temp << endl;
}

RFSensorContact::RFSensorContact(uint32_t id, HomieNode *homie):RFDevice(CONTACT, id, homie){
    DEBUG_PRINT("[RFS] Creating sensor type=Contact id=%s\n",idStr);
    homie->advertise(idStr).setDatatype("Contact");
}

void RFSensorContact::update(RFSensorPayload payload){
    open = payload[0];

    // update Homie property
    if (Homie.isConnected()) homie->setProperty(idStr).send(open?"OPEN":"CLOSED");
    Homie.getLogger() << millis() << " Sensor-Contact " << idStr << "=" << open << endl;
}
