#pragma once

#include <Homie.h>

bool globalCmdHandler(const HomieRange &range, const String &value);
bool globalUpdateHandler(const HomieNode &node, const HomieRange &range, const String &property, const String &value);
void globalOnHomieEvent(const HomieEvent &event);
