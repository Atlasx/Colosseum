#pragma once

#include "stdlibincl.h"

class IDebugGUI
{
public:
	virtual void OnDrawGUI() = 0;
	virtual std::string_view GetDebugMenuName() { return "DEBUG"; }
};