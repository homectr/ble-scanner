#pragma once

#include "RFDevice.h"

struct RFDeviceListEntry {
    RFDevice *device;
    RFDeviceListEntry *next;
};

class RFDevListIterator {
    RFDeviceListEntry *first = nullptr;
    RFDeviceListEntry *current = nullptr;
    public:
        RFDevListIterator(RFDeviceListEntry *entry);
        RFDevice* next();
        bool end(){return current == nullptr;};
};

class RFDeviceList {
    RFDeviceListEntry *list = nullptr;
    uint16_t _length = 0;

    public:
        void insert(RFDevice* device);
        RFDevice* get(uint32_t id);
        RFDevice* get(const char* idstr);
        uint16_t length(){return _length;};
        RFDevListIterator *iterator();
};

