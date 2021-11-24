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
    if (!j) list = d;
    else j->next = d;

    _length++;
    
}

RFDevice* RFDeviceList::get(uint32_t id){
    RFDeviceListEntry* i = list;
    while (i && i->device->id < id) i = i->next;
    if (i && i->device->id == id) return i->device;

    return nullptr;
}

RFDevice* RFDeviceList::get(const char* idstr){
    RFDeviceListEntry* i = list;
    while (i && strcmp(i->device->idStr,idstr) != 0) i = i->next;

    if (i) return i->device;

    return nullptr;
}

RFDevListIterator::RFDevListIterator(RFDeviceListEntry *entry){
    first = entry;
    current = entry;
}

RFDevice* RFDevListIterator::next(){
    RFDeviceListEntry *c = current;
    current = current->next;
    return c->device;
}

RFDevListIterator* RFDeviceList::iterator(){
    return new RFDevListIterator(list);
}

