#pragma once

#include <Homie.h>
#include "Item.h"
#include "RF24Bridge.h"

class Thing {
    public:
        HomieNode homieDevice = HomieNode("thing", "Thing", "thing");
        HomieNode homieSwitches = HomieNode("rf24brg", "RF24 Bridge", "rf24brg");

    protected:
        unsigned long seqStatusUpdatedOn = 0;
        unsigned long aliveTimer = 0;

        bool configured = false;

        Item* item = nullptr;

        

    public:
        Thing();
        void setup(); // call after Homie.setup()
        void loop();
        bool isConfigured(){return configured;};

};

