#pragma once

#include <Arduino.h>

class Item
{
protected:
    const char *id;

public:
    bool rebootNeeded = false;

public:
    virtual void loop(){};
    Item(const char *id) { this->id = id; };
    const char *getId() { return id; };
    virtual bool updateHandler(const String &property, const String &value) { return false; };
    virtual bool cmdHandler(const String &value) { return false; };
};