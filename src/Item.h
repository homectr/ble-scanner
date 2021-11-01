#pragma once

class Item {
    protected:
        const char* id;
    public:
        virtual void loop(){};
        Item(const char* id){this->id=id;}
        const char* getId(){return id;};
};