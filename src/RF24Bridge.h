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
    RFDevice* lastDevice = nullptr; // last updated device - for caching and duplicate packet identification
    bool isPairing = false; // is pairing mode active?
    unsigned long pairingTimer = 0; 
    HomieNode homie = HomieNode("rf24brg", "RF24 Bridge", "rf24brg");

    protected:
        // save list of paired devices to configuration file
        bool saveDevices();
        RFDevice* createDevice(RFSensorType type, uint16_t id);

    public:
        RF24Bridge(const char* id, uint16_t cePin, uint16_t csnPin);
        virtual void loop() override;
        // initiate pairing with the new device
        void startPairing();
        // load list of paired devices from configuration file
        bool loadDevices();
        virtual bool updateHandler(const String& property, const String& value) override;

};