#pragma once

#include <vector>
#include <cstdint>
#include <type_traits>
#include <cassert>
#include <limits>
#include <concepts>

template <typename T>
concept UnsignedIntegral = std::is_unsigned_v<T>;

// Concept of Handles and generations taken from https://floooh.github.io/2018/06/17/handles-vs-pointers.html
//
// Example Handle of 32 bits can represent 1M objects at once, up to 4B created before *boom*
// 
// |     GenerationBits      |           IndexBits             |
// +------------------------ + ------------------------------- +
// | G G G G G G G G G G G G | I I I I I I I I I I ... I I I I |
// |<-------- 12 bits------> | < ---------- 20 bits----------->|
// 
// I have not been thinking about endian-ness here... I honestly don't know how to apply that stuff yet
// 
template<UnsignedIntegral _Type, std::size_t _IndexBits, std::size_t _GenerationBits>
struct Handle
{
	static constexpr std::size_t _TotalBits = sizeof(_Type) * 8;
	static_assert(_IndexBits + _GenerationBits == _TotalBits, "Check expected size of handle!");
	static_assert(_IndexBits >= 4 && _GenerationBits >= 4, "Too few bits!");

	static constexpr std::size_t IndexBits = _IndexBits;
	static constexpr std::size_t GenerationBits = _GenerationBits;

	// Post-shift bit mask for each section of the handle (need to shift before applying mask)
	static constexpr _Type _IndexMask = (1ULL << _IndexBits) - 1;
	static constexpr _Type _GenerationMask = (1ULL << _GenerationBits) - 1;

	_Type m_value;

	Handle(_Type idx = _IndexMask, uint16_t gen = 0) : m_value((idx & _IndexMask) | ((gen & _GenerationMask) << _IndexBits)) {}

	_Type GetIndex() const
	{
		return m_value & _IndexMask;
	}

	_Type GetGeneration() const
	{ // Shift and mask to get leading bits
		return (m_value >> _IndexBits) & _GenerationMask;
	}

	bool IsValid() const
	{
		return GetIndex() != _IndexMask;
	}

	bool operator==(const Handle& other) const
	{
		return m_value == other.m_value;
	}

	bool operator!=(const Handle& other) const
	{
		return !(*this == other);
	}

	//static const Handle INVALID;
};

// This was confusing, defining a templated version of invalid handle so any created handle type has
// a corresponding Invalid. 
// These should only be accessed with type aliases for the Handle ie ObjectHandle::INVALID
// instead of a massive Handle<blah, blah, blah>
/*
template <UnsignedIntegral _T, std::size_t _Idx, std::size_t _Gen>
const Handle<_T, _Idx, _Gen> Handle<_T, _Idx, _Gen>::INVALID{_IndexMask, 0};
*/

// Finally! Handle type declarations! Systems can create their own too
using ObjectHandle = Handle<std::uint32_t, 20, 12>;
using ComponentHandle = Handle<std::uint32_t, 20, 12>;
using GenericHandle = Handle<std::uint32_t, 20, 12>;

template <typename _HT>
concept ValidHandleType = requires {
	{ _HT::IndexBits } -> std::integral;
	{ _HT::GenerationBits } -> std::integral;
};

template<typename _ObjectType, typename _HandleType = GenericHandle, std::size_t _MaxItems = 1024ULL>
//requires ValidHandleType<_HandleType>
class ObjectPool
{
public:
	using HandleType = _HandleType;
	using ObjectType = _ObjectType;

	ObjectPool() : m_objects(_MaxItems), m_freelistIndex(0) {}

	// Copies into array
	HandleType Create(const ObjectType& item)
	{
		std::size_t index = m_freelistIndex;
		if (index < 0 || index >= _MaxItems) {
			return HandleType::INVALID;
		}

		m_objects[index] = item;
		return HandleType{ static_cast<std::uint16_t>(index), 0 };
	}

	ObjectType& Get(const HandleType& handle)
	{
		// Validate handle id and gen
		return m_objects[handle.GetIndex()];
	}

	void Destroy(const HandleType& handle)
	{
		// validate and remove object from list update freelist
	}

private:
	std::vector<ObjectType> m_objects;
	std::size_t m_freelistIndex;
};