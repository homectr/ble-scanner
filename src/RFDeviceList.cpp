#include "RFDeviceList.h"

void RFDeviceList::insert(RFDevice* device){
    RFDeviceListEntry* i = list;
    RFDeviceListEntry* j = nullptr;
    while (i && i->device->type <= device->type && i->device->id < device->id) {
        j = i;
        i = i->next;
    }
    
    RFDeviceListEntry* d = new RFDeviceListEntry();
    d->device = device;
    d->next = i;
    if (j) j->next = d;    
}

RFDevice* RFDeviceList::get(RFDeviceTypes type, uint16_t id){
    RFDeviceListEntry* i = list;
    while (i && i->device->type <= type && i->device->id < id) {
        i = i->next;
    }
    if (i && i->device->type == type && i->device->id == id) return i->device;
    else return nullptr;
}

