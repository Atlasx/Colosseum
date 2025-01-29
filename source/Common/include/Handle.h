#pragma once

#include <vector>
#include <cstdint>
#include <type_traits>
#include <cassert>
#include <limits>
#include <concepts>
#include <array>
#include <iostream>

template <typename T>
concept UnsignedIntegral = std::is_unsigned_v<T>;

// Concept of Handles and generations taken from https://floooh.github.io/2018/06/17/handles-vs-pointers.html
//
// Example Handle of 32 bits can represent 1M objects at once, up to 2B created before *boom*
// 
// | ActiveBit |     GenerationBits      |           IndexBits             |
// + --------- + ----------------------- + ------------------------------- +
// | A         | G G G G G G G G G G G G | I I I I I I I I I I ... I I I I |
// |<- bit --> |<-------- 12 bits------> | < ---------- 20 bits----------->|

// I have not been thinking about endian-ness here... I honestly don't know how to apply that stuff yet
// 
template<UnsignedIntegral _Type, std::size_t _IndexBits, std::size_t _GenerationBits>
struct Handle
{
	using UnderlyingType = _Type;

	static constexpr std::size_t ActiveBits = 1;
	static constexpr std::size_t IndexBits = _IndexBits;
	static constexpr std::size_t GenerationBits = _GenerationBits;

	static constexpr std::size_t TotalBits = sizeof(_Type) * 8;
	static_assert(IndexBits + GenerationBits + ActiveBits == TotalBits, "Check expected size of handle!");
	static_assert(IndexBits >= 4 && GenerationBits >= 4, "Too few bits!");

	// Post-shift bit mask for each section of the handle (need to shift before applying mask)
	static constexpr _Type ActiveMask = (1ULL << (IndexBits + GenerationBits));
	static constexpr _Type IndexMask = (1ULL << IndexBits) - 1;
	static constexpr _Type GenerationMask = (1ULL << GenerationBits) - 1;

	_Type m_value;

	Handle() : m_value(INVALID.m_value) {}
	Handle(_Type idx, _Type gen, bool active = false) : m_value((idx& IndexMask) | ((gen & GenerationMask) << _IndexBits) | (active ? ActiveMask : 0)) {}

	static Handle Generate(_Type index, _Type generation, bool active)
	{
		assert(index <= IndexMask && "Index out of range!");
		assert(generation <= GenerationMask && "Generation out of range!");

		return Handle(index, generation, active);
	}

	_Type GetIndex() const
	{
		return m_value & IndexMask;
	}

	_Type GetGeneration() const
	{ // Shift and mask to get leading bits
		return (m_value >> IndexBits) & GenerationMask;
	}

	bool IsActive() const
	{
		return m_value & ActiveMask;
	}

	bool IsValid() const
	{
		return GetIndex() != IndexMask;
	}

	bool operator==(const Handle& other) const
	{
		return m_value == other.m_value;
	}

	bool operator!=(const Handle& other) const
	{
		return !(*this == other);
	}

	static const Handle INVALID;
};

// This was confusing, defining a templated version of invalid handle so any created handle type has
// a corresponding Invalid. 
// These should only be accessed with type aliases for the Handle ie ObjectHandle::INVALID
// instead of a massive Handle<blah, blah, blah>
template <UnsignedIntegral _T, std::size_t _Idx, std::size_t _Gen>
const Handle<_T, _Idx, _Gen> Handle<_T, _Idx, _Gen>::INVALID{ Handle<_T, _Idx, _Gen>::IndexMask, 0, false };


// Finally! Handle type declarations! Systems can create their own too
using ObjectHandle = Handle<std::uint32_t, 20, 11>;
using ComponentHandle = Handle<std::uint32_t, 20, 11>;
using GenericHandle = Handle<std::uint32_t, 20, 11>;
using EntityHandle = Handle<std::uint32_t, 20, 11>;


template <typename _HT>
concept ValidHandleType = requires {
	requires std::unsigned_integral<decltype(_HT::IndexBits)>;
	requires std::unsigned_integral<decltype(_HT::GenerationBits)>;
};