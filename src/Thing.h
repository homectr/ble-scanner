#pragma once

#include <Homie.h>
#include "Item.h"
#include "RF24Bridge.h"
#include "List.h"

class Thing {
    public:
        HomieNode homie = HomieNode("thing", "Thing", "thing");

    protected:
        unsigned long seqStatusUpdatedOn = 0;
        unsigned long aliveTimer = 0;

        bool configured = false;
        unsigned long rebootTimer = 0;

        List<Item> items;       

    public:
        Thing();
        void setup(); // call after Homie.setup()
        void loop();
        bool isConfigured(){return configured;};
        bool updateHandler(const HomieNode& node, const String& property, const String& value);
        bool cmdHandler(const String& value);

};

