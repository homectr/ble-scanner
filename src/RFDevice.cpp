#include "RFDevice.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

RFDevice::RFDevice(RFDeviceTypes type, uint16_t id){
    this->type=type; 
    this->id=id;
    char buf[5];
    snprintf(buf,5,"%04X",id);
    this->idStr = strdup(buf);
}

RFSensorTemp::RFSensorTemp(uint16_t id):RFDevice(SENSOR_TEMPERATURE,id){
    DEBUG_PRINT("[RFS] Creating sensor type=Temperature id=%s\n",idStr);
    HomieNode* node = new HomieNode(idStr, idStr, "sensor_temp"); 
    node->advertise("t").setDatatype("Number:Temperature");
};

void RFSensorTemp:: update(RFPayload payload){
    memcpy((void*)&temp,(void*)&payload,4);
    
    // update Homie property
    if (Homie.isConnected()) homie->setProperty("t").send(String(temp));
    Homie.getLogger() << millis() << " Sensor-Temp " << idStr << "=" << temp << endl;
}

RFSensorContact::RFSensorContact(uint16_t id):RFDevice(SENSOR_CONTACT,id){
    DEBUG_PRINT("[RFS] Creating sensor type=Contact id=%s\n",idStr);
    HomieNode* node = new HomieNode(idStr, idStr, "sensor_contact"); 
    node->advertise("contact").setDatatype("Contact");
};

void RFSensorContact::update(RFPayload payload){
    open = payload[0];

    // update Homie property
    if (Homie.isConnected()) homie->setProperty("contact").send(open?"OPEN":"CLOSED");
    Homie.getLogger() << millis() << " Sensor-Contact " << idStr << "=" << open << endl;
}
