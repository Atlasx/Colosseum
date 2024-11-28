#pragma once

#include "imgui.h"

#include <unordered_map>
#include <functional>


namespace CE
{
	class DebugMenu
	{
		using DebugItemCallback = std::function<void()>;

		std::string m_menuName;
		std::vector<std::pair<std::string, DebugItemCallback>> m_items;

	public:
		DebugMenu() : m_menuName("Debug"), m_items{} {}

		void AddMenuItem(std::string name, DebugItemCallback callback);

		void ShowMenuItems()
		{
			ImGui::MenuItem("Test");
		}

	private:
		bool HasMenuItem(std::string name)
		{
			for (auto [itemName, callback] : m_items)
			{
				if (name == itemName) {
					return true;
				}
			}
			return false;
		}
	};
}