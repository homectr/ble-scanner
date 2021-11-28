#include "RFDeviceList.h"

#define NODEBUG_PRINT

#include "debug_print.h"

void RFDeviceList::insert(RFDevice* device){
    RFDeviceListEntry* d = new RFDeviceListEntry();
    d->device = device;
    d->next = list;
    
    list = d;  
    _length++;   
}

void RFDeviceList::clear(){
    RFDeviceListEntry* i = list;
    RFDeviceListEntry* j;
    while (i) {
        DEBUG_PRINT(PSTR("[DL-clr] id=0x%X\n"),i->device->id);
        delete i->device;
        j = i;
        i = i->next;
        delete j;
    }
    list = nullptr;
}

void RFDeviceList::clear(uint32_t id){
    RFDeviceListEntry* i = list;
    RFDeviceListEntry* j = nullptr;
    while (i && i->device->id != id) {
        j = i;
        i = i->next;
    }

    if (i) {
        DEBUG_PRINT(PSTR("[DL-clrID] id=0x%X\n"), i->device->id);
        
        if (j) j->next = i->next;
        else list = i->next;
        
        delete i->device;
        delete i;
    }
}

RFDevice* RFDeviceList::get(uint32_t id){
    RFDeviceListEntry* i = list;
    while (i && i->device->id != id) {
        DEBUG_PRINT(PSTR("[DL-get] id=0x%X did=0x%X\n"),id, i->device->id);
        i = i->next;
    }
    if (i) return i->device;

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

