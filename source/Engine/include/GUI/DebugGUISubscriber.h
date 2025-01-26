#pragma once

class IDebugGUISubscriber
{
public:
	virtual void OnDrawGUI() = 0;
	virtual std::string_view GetDebugMenuName() { return "DEBUG"; }
	virtual bool IsDrawEnabled() = 0;
	virtual void SetDrawEnabled(bool) = 0;
};