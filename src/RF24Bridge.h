#pragma once

#include <RF24.h>
#include "Item.h"
#include "RFDeviceList.h"
#include "RFPacket.h"

class RF24Bridge: public Item {

    unsigned long aliveTimer = 0;
    RF24 *radio = nullptr;
    RFDeviceList devices;
    RFDevice* lastDevice = nullptr;

    public:
        RF24Bridge(const char* id, uint16_t cePin, uint16_t csnPin);
        virtual void loop();
        RFDevice* createDevice(RFDeviceTypes type, uint16_t id);

};