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

    public:
        void insert(RFDevice* device);
        RFDevice* get(RFSensorType type, uint32_t id);
        RFDevListIterator *iterator();
};

