#pragma once

#include <string>
#include <vector>

class DependencyGraph
{
public:

	void AddNode(const std::string& name, const std::vector<std::string>& dependencies);

	void TopologicalSort();

};