#pragma once

#include "RFDevice.h"

struct RFDeviceListEntry {
    RFDevice *device;
    RFDeviceListEntry *next;
};

class RFDeviceList {
    RFDeviceListEntry *list = nullptr;

    public:
    void insert(RFDevice* device);
    RFDevice* get(RFDeviceTypes type, uint16_t id);
};