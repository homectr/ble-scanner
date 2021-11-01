#pragma once

#include <Homie.h>

bool cmdHandler(const HomieRange &range, const String &value);
bool updateHandler(const HomieNode &node, const HomieRange &range, const String &property, const String &value);
