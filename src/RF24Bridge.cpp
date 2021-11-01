#include "RF24Bridge.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

#define NRF_CHANNEL             120
#define NRF_BROADCAST_ADDRESS  "rf24se"
#define NRF_LISTENING_PIPE      0

void RF24Bridge::loop(){
    radio->startListening();
    #ifndef NODEBUG_PRINT
    if (millis()-aliveTimer > 15000){
        aliveTimer = millis();
        DEBUG_PRINT("[RFB] alive ms=%lu\n",millis());
    }
    #endif

    if (!radio->available(NRF_LISTENING_PIPE)) return;

    RFPacket buffer;
    bool update = false;

    DEBUG_PRINT("[RFB] Data available %d ",sizeof(buffer));
    
    radio->read(&buffer,sizeof(buffer));

    if (!lastDevice || buffer.deviceId != lastDevice->id || buffer.deviceType != lastDevice->type) {
        lastDevice = devices.get(buffer.deviceType, buffer.deviceId);
        if (!lastDevice) {
            RFDevice* d = createDevice(buffer.deviceType, buffer.deviceId);
            if (d) devices.insert(d);
            else CONSOLE("[RFB] Warning: Unknown device type = %d\n",buffer.deviceType);
            lastDevice = d;
        }
        update = true;
    } else {
        update = lastDevice->seqno == buffer.seqno;
    }

    if (update) {
        DEBUG_PRINT(" type=%d adr=%X seq=%u", buffer.deviceType, buffer.deviceId, buffer.seqno);
        lastDevice->update(buffer.payload);
    } else {
        DEBUG_PRINT(" type=%d adr=%X seq=%u duplicate\n", buffer.deviceType, buffer.deviceId, buffer.seqno);

    }
        
}

RF24Bridge::RF24Bridge(const char* id, uint16_t cePin, uint16_t csnPin):Item(id){
    DEBUG_PRINT("[RFB] Initializing bridge ");
    radio = new RF24(cePin, csnPin);
    if (radio->begin()) {
        DEBUG_PRINT("success\n");
    } else {
        DEBUG_PRINT("failed\n");
    }
    radio->setDataRate(RF24_250KBPS); // set datarate to 250kbps to enhance range
    radio->setAutoAck(false);
    radio->setChannel(NRF_CHANNEL); 
    radio->setCRCLength(RF24_CRC_16); // set CRC length
    unsigned char address[7] = NRF_BROADCAST_ADDRESS;
    radio->openReadingPipe(NRF_LISTENING_PIPE,address);
    radio->startListening();

    #ifndef NODEBUG_PRINT
    radio->printPrettyDetails();
    #endif
}

RFDevice* RF24Bridge::createDevice(RFDeviceTypes type, uint16_t id){
    RFDevice* device = nullptr;
    switch (type) {
    case SENSOR_TEMPERATURE:
        device = new RFSensorTemp(id);
        break;
    case SENSOR_CONTACT:
        device = new RFSensorContact(id);
        break;
    
    default:
        break;
    }

    return device;
}