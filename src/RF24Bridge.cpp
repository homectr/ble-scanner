#include "RF24Bridge.h"
#include <FS.h>

//#define NODEBUG_PRINT
#include "debug_print.h"

#define RF24BR_CHANNEL              120
#define RF24BR_BRIDGE_ADDRESS       0xB1B2B3B401LL
#define RF24BR_ACTUATOR_ADDRESS     0xA1A2A3A4A5LL
#define RF24BR_PAIRING_TIMEOUT      10000
#define RF24BR_ALIVE_TIMEOUT        10000

#define DEVICE_LIST_FILE_NAME       "/devices.txt"

#define CHAR_LINEFEED char(10)

void RF24Bridge::loop(){
    radio->startListening();
    #ifndef NODEBUG_PRINT
    if (millis()-aliveTimer > RF24BR_ALIVE_TIMEOUT){
        aliveTimer = millis();
        DEBUG_PRINT("[RFB] alive ms=%lu\n",millis());
    }
    #endif

    if (isPairing && millis()-pairingTimer > RF24BR_PAIRING_TIMEOUT) {
        isPairing = false;
        CONSOLE("[RFB] Pairing mode ended\n");
        if (devicesUpdated) {
            CONSOLE("[RFB] List of devices updated.\n");
            saveDevices();
        }
    }

    if (!radio->available()) return;

    RFSensorPacket buffer;
    bool update = false;

    DEBUG_PRINT("[RFB] Data available %d ",sizeof(buffer));
    
    radio->read(&buffer,sizeof(buffer));

    if (!lastDevice || buffer.srcAdr != lastDevice->id || buffer.deviceType != lastDevice->type) {
        lastDevice = devices.get(buffer.deviceType, buffer.srcAdr);
        if (!lastDevice && isPairing) {
            RFDevice* d = createDevice(buffer.deviceType, buffer.srcAdr);
            if (d) { 
                devices.insert(d);
                devicesUpdated = true;
            }
            else CONSOLE("[RFB] Warning: Unknown device type = %d\n",buffer.deviceType);
            lastDevice = d;
        }
        update = lastDevice != nullptr;
    } else {
        update = lastDevice->seqno != buffer.seqno;
    }

    if (lastDevice) {
        if (update) {
            DEBUG_PRINT(" type=%d adr=%08X seq=%u lastDev=%X\n", buffer.deviceType, buffer.srcAdr, buffer.seqno, lastDevice);
            lastDevice->update(buffer.payload);
            lastDevice->seqno = buffer.seqno;
        } else {
            DEBUG_PRINT(" type=%d adr=%X seq=%u duplicate\n", buffer.deviceType, buffer.srcAdr, buffer.seqno);
        }
    } else {
        DEBUG_PRINT("Unpaired device.\n");
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
    radio->setPALevel(RF24_PA_MAX);
    radio->setDataRate(RF24_250KBPS); // set datarate to 250kbps to enhance range
    radio->setAutoAck(false);
    radio->setChannel(RF24BR_CHANNEL); 
    radio->setCRCLength(RF24_CRC_16); // set CRC length
    radio->openReadingPipe(1, RF24BR_BRIDGE_ADDRESS);
    radio->startListening();

    #ifndef NODEBUG_PRINT
    radio->printPrettyDetails();
    #endif

    homie.advertise("pairing").setDatatype("boolean").settable();

    loadDevices();
}

RFDevice* RF24Bridge::createDevice(RFSensorType type, uint32_t id){
    RFDevice* device = nullptr;
    switch (type) {
    case RFSensorType::TEMPERATURE:
        device = new RFSensorTemp(id, &homie);
        break;
    case RFSensorType::CONTACT:
        device = new RFSensorContact(id, &homie);
        break;
    
    default:
        break;
    }

    return device;
}

void RF24Bridge::startPairing(){
    CONSOLE("[RFB] Pairing mode started\n");
    radio->stopListening();
    // lower output power -> newly paired sensor has to be close to the bridge -> security
    radio->setPALevel(RF24_PA_MIN);
    // open pipe for pairing
    radio->openWritingPipe(RF24BR_ACTUATOR_ADDRESS);
    uint32_t ms = millis();
    RFActuatorPacket p;
    p.seqno = ms;
    p.dstAdr = 0xFFFFFFFF;
    p.pktType = RFPacketType::SCAN;
    for(int i=0;i<3;i++){
        radio->writeFast(&p, sizeof(p));
        delay(20);
    }
    pairingTimer = millis();
    isPairing = true;
}

bool RF24Bridge::saveDevices(){
    DEBUG_PRINT("[RFB] Saving devices\n");
    File f = SPIFFS.open(DEVICE_LIST_FILE_NAME,"w");
    if (!f) {
        DEBUG_PRINT("[RFB] Error saving devices\n");
        return false;
    }
    RFDevListIterator *i = devices.iterator();
    int j = 0;
    while (!i->end()){
        j++;
        RFDevice *d = i->next();
        DEBUG_PRINT("Saving type=%s id=%s\n",String(d->type).c_str(),d->idStr);
        f.write(String(d->type).c_str());
        f.write(',');
        f.write(String(d->id).c_str());
        f.write(CHAR_LINEFEED);
    }
    f.close();
    delete i;
    DEBUG_PRINT("[RFB] Saved %d devices\n",j);
    return true;
}

bool RF24Bridge::loadDevices(){
    DEBUG_PRINT("[RFB] Loading devices\n");
    File f = SPIFFS.open(DEVICE_LIST_FILE_NAME,"r");
    if (!f) {
        CONSOLE("[RFB] Error opening list of devices\n");
        return false;
    }
    int j = 0;
    while (f.available()){
        String line = f.readStringUntil(CHAR_LINEFEED);
        DEBUG_PRINT("[RFB-ld] line=%s\n",line.c_str());
        int i = line.indexOf(',');
        if (i>0) {
            uint8_t devT = line.substring(0,i-1).toInt();
            uint32_t devA = line.substring(i+1).toInt();
            DEBUG_PRINT("[RFB-ld] device type=%d adr=%X\n",devT, devA);
            RFDevice *d = createDevice((RFSensorType)devT, devA);
            if (d) {
                devices.insert(d);
                j++;
            }
            else CONSOLE("[RFB-ld] Unknown device type %d\n",devT);
        }
    }
    f.close();
    CONSOLE("[RFB] %d devices loaded\n",j);
    return true;
}

bool RF24Bridge::updateHandler(const String& property, const String& value){
    if (property == "pairing"){
        if (value == "true") {
            if (!isPairing) startPairing();
            return true;
        }
    }

    return false;
};