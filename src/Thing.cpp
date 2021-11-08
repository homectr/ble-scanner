#include "Thing.h"
#include <Arduino.h>
#include "utils.h"
#include "handlers.h"
#include "RF24Bridge.h"

#define NODEBUG_PRINT
#include "debug_print.h"

#define NRF_CSNPIN  15  //D8 + use HW CS pin + 3-5kOhm resistor to connect it to GND
#define NRF_CEPIN   2   //D4

Thing::Thing(){
    // create properties for device
    homieDevice.advertise("cmd").setDatatype("string").settable(cmdHandler);

    // create items
    item = new RF24Bridge("rf24brg", NRF_CEPIN, NRF_CSNPIN);
    
    DEBUG_PRINT("[Thing:Thing] Thing created\n");
}

void Thing::setup(){
    DEBUG_PRINT("[Thing:Setup] SETUP\n");

    if (!Homie.isConfigured()){
        CONSOLE("Homie not configured. Skipping Thing setup. Loop will be ignored.\n");
        return;
    }
    
    DEBUG_PRINT("[Thing:Setup] Completed\n");

    configured = true;
}

bool Thing::updateHandler(const HomieNode& node, const String& property, const String& value){
    // call all update handlers for all items until one returns true
    return item->updateHandler(property,value);
}

void Thing::loop(){
    #ifndef NODEBUG_PRINT
    if (millis()-aliveTimer > 15000){
        aliveTimer = millis();
        DEBUG_PRINT("[Thing] alive ms=%lu\n",millis());
        if (!isConfigured()) 
            DEBUG_PRINT("[Thing] not configured. Skipping loop.\n");
    }
    #endif

    if (!isConfigured()) return;
    item->loop();
    
}