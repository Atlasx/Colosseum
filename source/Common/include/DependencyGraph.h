#pragma once

#include <string>
#include <unordered_map>
#include <vector>

template<typename T>
class DependencyGraph
{
public:

	void AddNode(T node, const std::vector<T>& dependencies) {
		m_nodes.emplace(node, dependencies);
	}

	void TopologicalSort();

private:
	std::unordered_map<T, std::vector<T>> m_nodes;
	
};