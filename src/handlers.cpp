#include "handlers.h"
#include "Thing.h"
#include "utils.h"

//#define NODEBUG_PRINT
#include "debug_print.h"

extern Thing* thing;

bool globalCmdHandler(const HomieRange& range, const String& value){
    DEBUG_PRINT(PSTR("[cmdHandler] val=%s\n"),value.c_str());
    bool updated = false;

    if (value == "reset") {
        esp_reset();
        updated = true;
    }

    if (!updated) updated = thing->cmdHandler(value);

    if (updated) {
        thing->homieDevice.setProperty("cmd").send(value);  // Update the state of the led
        Homie.getLogger() << "Cmd is " << value << endl;
    }

    return updated;
}


bool globalUpdateHandler(const HomieNode &node, const HomieRange &range, const String &property, const String &value){
    DEBUG_PRINT(PSTR("[updHandler] node=%s prop=%s val=%s\n"),node.getId(),property.c_str(),value.c_str());
    bool updated = false;
    String newValue = value;

    updated = thing->updateHandler(node,property,value);

    if (updated) {
        node.setProperty(property).send(newValue);
        Homie.getLogger() << "Node '"<< node.getId() << "' property '" << property << "' set to " << newValue << endl;
    }

    return updated;
}
