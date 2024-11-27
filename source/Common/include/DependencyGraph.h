#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <type_traits>

template<typename... Dependencies>
struct DependencyList {
	using type = std::tuple<Dependencies...>;
};

template<typename System, typename DependencyListType>
struct SystemDependencies {
	using SystemType = System;
	using Dependencies = DependencyListType;
};

template<typename System, typename DependencyList>
struct InitSystem {
	static void Init() {
		InitDependencies(typename DependencyList::type{});

		System{}.Init();
	}

private:
	template<typename... Dependencies>
	static void InitDependencies(std::tuple<Dependencies...>) {
		(InitSystem<Dependencies, typename Dependencies::Dependencies>::Init(), ...);
	}
};

template<typename... Systems>
void InitializeAllSystems() {
	// Call Init on each System+Dependency pair (InitSystem)
	(InitSystem<typename Systems::SystemType, typename Systems::Dependencies>::Init(), ...);
}
