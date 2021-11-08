#pragma once

#include <Homie.h>
#include "Item.h"
#include "RF24Bridge.h"

class Thing {
    public:
        HomieNode homieDevice = HomieNode("thing", "Thing", "thing");

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
        bool updateHandler(const HomieNode& node, const String& property, const String& value);

};

