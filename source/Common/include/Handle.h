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
// Example Handle of what 64 bits can represent
// 
// | ActiveBit |   Flag Bits   |     TypeBits    |     GenerationBits      |           IndexBits             |
// + --------- + ------------- + --------------- + ----------------------- + ------------------------------- +
// | A         | F F F F F F F | T T T T T T T T | G G G G G G G G G G G G | I I I I I I I I I I ... I I I I |
// |<- bit --> | <-- 7 bits -> | <--- 8 bits --> | <------ 16 bits ------> | <---------- 32 bits ----------> |

template<UnsignedIntegral _Type, std::size_t _IndexBits, std::size_t _GenerationBits, std::size_t _TypeBits, std::size_t _FlagBits>
struct Handle
{
	using UnderlyingType = _Type;

	static constexpr std::size_t ActiveBits = 1;
	static constexpr std::size_t FlagBits = _FlagBits;
	static constexpr std::size_t TypeBits = _TypeBits;
	static constexpr std::size_t IndexBits = _IndexBits;
	static constexpr std::size_t GenerationBits = _GenerationBits;

	static constexpr std::size_t TotalBits = sizeof(_Type) * 8;
	static_assert(IndexBits + GenerationBits + ActiveBits + FlagBits + TypeBits == TotalBits, "Check expected size of handle!");
	static_assert(IndexBits >= 4 && GenerationBits >= 4, "Too few bits!");

	// Post-shift bit mask for each section of the handle (need to shift before applying mask)
	static constexpr _Type ActiveMask = (1ULL << (FlagBits + TypeBits + GenerationBits + IndexBits));
	static constexpr _Type FlagMask = (1ULL << FlagBits) - 1;
	static constexpr _Type TypeMask = (1ULL << TypeBits) - 1;
	static constexpr _Type IndexMask = (1ULL << IndexBits) - 1;
	static constexpr _Type GenerationMask = (1ULL << GenerationBits) - 1;

	static constexpr std::size_t FlagShift = TypeBits + GenerationBits + IndexBits;
	static constexpr std::size_t TypeShift = GenerationBits + IndexBits;
	static constexpr std::size_t GenerationShift = IndexBits;

	_Type m_value;

	Handle() : m_value(INVALID.m_value) {}
	Handle(_Type idx, _Type gen = 0, _Type typ = 0, _Type flags = 0, bool active = false)
	{
		m_value = 0;
		m_value |= (active ? ActiveMask : 0);
		m_value |= ((flags & FlagMask) << FlagShift);
		m_value |= ((typ & TypeMask) << TypeShift);
		m_value |= ((gen & GenerationMask) << GenerationShift);
		m_value |= (idx & IndexMask);
	}

	static Handle Generate(_Type index, _Type generation, bool active, _Type flags = 0, _Type mType = 0)
	{
		// Do any checks here?
		return Handle(index, generation, mType, flags, active);
	}

	_Type GetIndex() const
	{
		return m_value & IndexMask;
	}

	_Type GetGeneration() const
	{ // Shift and mask to get leading bits
		return (m_value >> GenerationShift) & GenerationMask;
	}

	_Type GetType() const
	{
		return (m_value >> TypeShift) & TypeMask;
	}

	_Type GetFlags() const
	{
		return (m_value >> FlagShift) & FlagMask;
	}

	bool GetFlag(std::size_t offset) const
	{
		assert(offset < FlagBits); // Invalid flag bit offset
		_Type mask = 1ULL << (offset + FlagShift);
		return m_value & mask;
	}

	void SetFlag(bool value, std::size_t offset)
	{
		assert(offset < FlagBits); // Invalid flag bit offset
		_Type mask = 1ULL << (offset + FlagShift);
		m_value = value ? (m_value | mask) : (m_value & ~mask);
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
template <UnsignedIntegral _T, std::size_t _Idx, std::size_t _Gen, std::size_t _Type, std::size_t _Flags>
const Handle<_T, _Idx, _Gen, _Type, _Flags> Handle<_T, _Idx, _Gen, _Type, _Flags>::INVALID{ Handle<_T, _Idx, _Gen, _Type, _Flags>::IndexMask, 0, false };


// Finally! Handle type declarations! Systems can create their own too
using ObjectHandle = Handle<std::uint64_t, 32, 16, 8, 7>;
using ComponentHandle = Handle<std::uint64_t, 32, 16, 8, 7>;
using GenericHandle = Handle<std::uint64_t, 32, 16, 8, 7>;
using EntityHandle = Handle<std::uint64_t, 32, 16, 8, 7>;


template <typename _HT>
concept ValidHandleType = requires {
	requires std::unsigned_integral<decltype(_HT::IndexBits)>;
	requires std::unsigned_integral<decltype(_HT::GenerationBits)>;
};
