#include "RF24Bridge.h"
#include <FS.h>
#include "utils.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

#define RF24BR_CHANNEL              120
#define RF24BR_BRIDGE_ADDRESS       0xB1B2B3B401LL
#define RF24BR_ACTUATOR_ADDRESS     0xA1A2A3A4A5LL
#define RF24BR_PAIRING_TIMEOUT      10000
#define RF24BR_ALIVE_TIMEOUT        10000

#define DEVICE_LIST_FILE_NAME       "/devices.txt"

#define CHAR_LINEFEED char(10)

void RF24Bridge::processPktData(RFSensorPacket &buffer){
    RFDevice* d = devices.get(buffer.srcAdr); // find device in the list of paired devices
    if (!d && isPairing) { // device not found, but pairing is active
        d = createDevice(buffer.deviceType, buffer.srcAdr);
        if (d) { 
            devices.insert(d);
            DEBUG_PRINT(PSTR("[RFB-Data] Device added to the list. len=%d\n"),devices.length());
            devicesUpdated = true;
        }
        else CONSOLE(PSTR("[RFB-Data] Warning: Unknown device type = %d\n"),buffer.deviceType);
    }

    if (d) {
        DEBUG_PRINT(PSTR("[RFB-Data] Updating dtype=%d adr=0x%08X\n"), d->type, d->id);
        d->update(buffer.payload);
    } else {
        CONSOLE(PSTR("Device not paired. adr=0x%X type=%d\n"),buffer.srcAdr, buffer.deviceType);
    }

}

void RF24Bridge::processPktAnnounce(RFSensorPacket &buffer){

    DEBUG_PRINT(PSTR("Announce type=%d id=%X\n"),buffer.deviceType, buffer.srcAdr);
    if (!_announceTimer) _announceTimer = millis();
    if (_announced.length()>0) _announced += ", ";

    switch (buffer.deviceType) {
        case RFSensorType::CONTACT :
            _announced += DEVICE_STR_SENSOR_CONTACT;
            break;
        case RFSensorType::TEMPERATURE :
            _announced += DEVICE_STR_SENSOR_TEMP;
            break;
        case RFSensorType::HUMIDITY :
            _announced += DEVICE_STR_SENSOR_HUMIDITY;
            break;
        default:
            DEBUG_PRINT(PSTR("Unknown device type\n"));
            return;
    }

    char adr[10];
    snprintf(adr,10,":%08X",buffer.srcAdr);
    _announced += adr;

}

void RF24Bridge::loop(){
    radio->startListening();
    #ifndef NODEBUG_PRINT
    if (millis()-aliveTimer > RF24BR_ALIVE_TIMEOUT){
        aliveTimer = millis();
        DEBUG_PRINT(PSTR("[RFB] alive ms=%lu\n"),millis());
    }
    #endif

    if (isPairing && millis()-pairingTimer > RF24BR_PAIRING_TIMEOUT) {
        isPairing = false;
        CONSOLE(PSTR("[RFB] Pairing mode ended\n"));
        if (devicesUpdated) {
            CONSOLE(PSTR("[RFB] List of devices updated.\n"));
            saveDevices();
            esp_reset(); // reset to reload homie props
        }
        homie.setProperty("pairing").setQos(1).send("false");
    }

    if (_announced.length()>0 && millis()-_announceTimer > 5000){
        homie.setProperty("newdevices").setQos(1).send(_announced);
        _announced = "";
        _announceTimer = 0;
    }

    if (!radio->available()) return;

    RFSensorPacket buffer;
    radio->read(&buffer,sizeof(buffer));

    bool duplicatePkt = (buffer.srcAdr == lastDeviceAdr && buffer.deviceType == lastDeviceType);
    DEBUG_PRINT(PSTR("[RFB] Data available device=0x%X lastDevice=0x%X duplicate=%d\n"),buffer.srcAdr, lastDeviceAdr, duplicatePkt);

    if (!duplicatePkt) {
        lastDeviceAdr = buffer.srcAdr;
        lastDeviceType = buffer.deviceType;

        switch (buffer.pktType)
        {
        case RFPacketType::DATA :
            processPktData(buffer);
            break;
        case RFPacketType::ANNOUNCE :
            processPktAnnounce(buffer);
            break;
        default:
            DEBUG_PRINT(PSTR("[brg-loop] Unknown packet type=%d\n"),buffer.pktType);
            break;
        }
        
    }
        
}

RF24Bridge::RF24Bridge(const char* id, uint16_t cePin, uint16_t csnPin):Item(id){
    DEBUG_PRINT(PSTR("[RFB] Initializing bridge "));
    radio = new RF24(cePin, csnPin);
    if (radio->begin()) {
        DEBUG_PRINT(PSTR("success\n"));
    } else {
        DEBUG_PRINT(PSTR("failed\n"));
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

    homie.advertise("pairing").setDatatype("boolean").settable().setRetained(true);
    homie.advertise("newdevices").setDatatype("string").setRetained(true);

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
    case RFSensorType::HUMIDITY:
        device = new RFSensorHumidity(id, &homie);
        break;
    
    default:
        break;
    }

    return device;
}

void RF24Bridge::startPairing(){
    CONSOLE(PSTR("[RFB] Pairing mode started\n"));
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
    devicesUpdated = false;
}

bool RF24Bridge::saveDevices(){
    DEBUG_PRINT(PSTR("[RFB] Saving devices\n"));
    File f = SPIFFS.open(DEVICE_LIST_FILE_NAME,"w");
    if (!f) {
        DEBUG_PRINT(PSTR("[RFB] Error saving devices\n"));
        return false;
    }
    RFDevListIterator *i = devices.iterator();
    int j = 0;
    while (!i->end()){
        j++;
        RFDevice *d = i->next();
        DEBUG_PRINT(PSTR("Saving type=%s id=%s\n"),String(d->type).c_str(),d->idStr);
        f.write(String(d->type).c_str());
        f.write(',');
        f.write(String(d->id).c_str());
        f.write(CHAR_LINEFEED);
    }
    f.close();
    delete i;
    DEBUG_PRINT(PSTR("[RFB] Saved %d devices\n"),j);
    return true;
}

bool RF24Bridge::loadDevices(){
    DEBUG_PRINT(PSTR("[RFB] Loading devices\n"));
    File f = SPIFFS.open(DEVICE_LIST_FILE_NAME,"r");
    if (!f) {
        CONSOLE(PSTR("[RFB] Error opening list of devices\n"));
        return false;
    }
    int j = 0;
    while (f.available()){
        String line = f.readStringUntil(CHAR_LINEFEED);
        DEBUG_PRINT(PSTR("[RFB-ld] line=%s\n"),line.c_str());
        int i = line.indexOf(',');
        if (i>0) {
            uint8_t devT = line.substring(0,i).toInt();
            uint32_t devA = line.substring(i+1).toInt();
            DEBUG_PRINT(PSTR("[RFB-ld] device type=%d adr=0x%X\n"),devT, devA);
            RFDevice *d = createDevice((RFSensorType)devT, devA);
            if (d) {
                devices.insert(d);
                j++;
            }
            else CONSOLE(PSTR("[RFB-ld] Unknown device type %d\n"),devT);
        }
    }
    f.close();
    CONSOLE("[RFB] %d devices loaded\n",devices.length());
    return true;
}


bool RF24Bridge::cmdHandler(const String& value){
    if (value == "clear-all"){
        CONSOLE(PSTR("CLEARING PAIRED DEVICES\n"));
        devices.clear();
        saveDevices();
        esp_reset(); // reset in order to reload homie props
        return true;
    }

    if (value.startsWith("clear:")){
        char i = value.indexOf(':');
        if (i<0) return false;
        uint32_t id = strtol(value.substring(i+1).c_str(),0,16);
        CONSOLE(PSTR("CLEARING PAIRED DEVICE id=0x%X\n"),id);
        devices.clear(id);
        saveDevices();
        esp_reset(); // reset in order to reload homie props
        return true;
    }

    if (value.startsWith("pair:")){
        char i = value.indexOf(':');
        if (i<0) return false;
        char j = value.indexOf(':',i+1);
        if (j<=0) return false;
        String dt = value.substring(i+1,j);
        uint32_t id = strtol(value.substring(j+1).c_str(),0,16);
        CONSOLE(PSTR("PAIRING DEVICE type=%s id=0x%X\n"),dt.c_str(), id);
        RFDevice *d = nullptr;
        if (dt == DEVICE_STR_SENSOR_HUMIDITY)
            d = createDevice(RFSensorType::HUMIDITY,id);
        if (dt == DEVICE_STR_SENSOR_TEMP)
            d = createDevice(RFSensorType::TEMPERATURE,id);
        if (dt == DEVICE_STR_SENSOR_CONTACT)
            d = createDevice(RFSensorType::CONTACT,id);

        if (d) {
            devices.insert(d);
            saveDevices();
            CONSOLE(PSTR("DEVICE PAIRED type=%s id=0x%X\n"),dt.c_str(), id);
            esp_reset(); // reset in order to reload homie props
        }
        return true;
    }

    return false;

}

bool RF24Bridge::updateHandler(const String& property, const String& value){
    
    if (property == "pairing"){
        if (value == "true") {
            DEBUG_PRINT(PSTR("[RFB-uh] Request for pairing process\n"));
            if (!isPairing) startPairing();
        }
        return true;
    }

    int i = property.indexOf("identify");
    if (i > 0) {
        String s = property.substring(0,i).c_str();
        DEBUG_PRINT(PSTR("[RFB-uh] Request for identification for %s\n"),s.c_str());
        RFDevice* d = devices.get(s.c_str());
        if (d) identify(d);
        else {
            DEBUG_PRINT(PSTR("[RFB-uh] Device not registered %s.\n"),s.c_str());
        }
        return true;
    }

    return false;
};

void RF24Bridge::identify(RFDevice* device){
    if (!device) return;
    CONSOLE(PSTR("[RFB] Sending identification request to %s\n"),device->idStr);
    radio->stopListening();
    // lower output power -> newly paired sensor has to be close to the bridge -> security
    radio->setPALevel(RF24_PA_MAX);
    // open pipe for sending indentification request
    radio->openWritingPipe(RF24BR_ACTUATOR_ADDRESS);
    
    RFActuatorPacket p;
    p.seqno = millis();
    p.dstAdr = device->id;
    p.pktType = RFPacketType::IDENTIFY;
    for(int i=0;i<3;i++){
        radio->writeFast(&p, sizeof(p));
        delay(20);
    }
}