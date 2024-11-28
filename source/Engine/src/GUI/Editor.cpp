#include "GUI/Editor.h"

namespace CE
{

	void DebugMenu::AddMenuItem(std::string name, DebugItemCallback callback)
	{
		if (HasMenuItem(name)) return;

		m_items.push_back(std::make_pair(name, callback));
	}

}