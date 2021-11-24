#pragma once

#include <Homie.h>
#include <RF24.h>
#include "Item.h"
#include "RFDeviceList.h"
#include "RFPacket.h"

class RF24Bridge: public Item {

    unsigned long aliveTimer = 0;
    RF24 *radio = nullptr;
    RFDeviceList devices; // list of connected devices
    bool devicesUpdated; // if the list of devices updated since last pairing

    uint8_t lastDeviceType = 0;
    uint32_t lastDeviceAdr = 0;

    bool isPairing = false; // is pairing mode active?
    unsigned long pairingTimer = 0; 
    HomieNode homie = HomieNode("rf24", "RF24 Bridge", "rf24");

    String _announced = ""; // list of announced devices
    unsigned long _announceTimer = 0;

    protected:
        // save list of paired devices to configuration file
        bool saveDevices();
        RFDevice* createDevice(RFSensorType type, uint32_t id);
        void processPktData(RFSensorPacket &buffer);
        void processPktAnnounce(RFSensorPacket &buffer);

    public:
        RF24Bridge(const char* id, uint16_t cePin, uint16_t csnPin);
        virtual void loop() override;
        // initiate pairing with the new device
        void startPairing();
        // load list of paired devices from configuration file
        bool loadDevices();
        // asks device to identify itself, e.g. visually
        void identify(RFDevice*);
        virtual bool updateHandler(const String& property, const String& value) override;
        virtual bool cmdHandler(const String& value) override;

};