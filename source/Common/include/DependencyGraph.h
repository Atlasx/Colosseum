#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class DependencyGraph
{
	struct SortingData {
		std::string m_name;
		int m_inDegree;
		std::vector<std::string> m_dependencies;

		SortingData() : m_name(""), m_inDegree(0) {}
		SortingData(const std::string n, const std::vector<std::string>& dep) : m_name(n), m_dependencies(dep) {
			m_inDegree = dep.size();
		}
	};

public:

	void AddNode(const std::string name, const std::vector<std::string>& dependencies) {
		m_nodes[name] = SortingData(name, dependencies);
	}

	void TopologicalSort();

private:

	std::unordered_map<std::string, SortingData> m_nodes;
	
};