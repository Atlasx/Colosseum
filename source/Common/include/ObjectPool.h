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

	static constexpr std::size_t _TotalBits = sizeof(_Type) * 8;
	static_assert(_IndexBits + _GenerationBits == _TotalBits, "Check expected size of handle!");
	static_assert(_IndexBits >= 4 && _GenerationBits >= 4, "Too few bits!");

	static constexpr std::size_t ActiveBits = 1;
	static constexpr std::size_t IndexBits = _IndexBits;
	static constexpr std::size_t GenerationBits = _GenerationBits;

	// Post-shift bit mask for each section of the handle (need to shift before applying mask)
	static constexpr _Type ActiveMask = (1ULL << (IndexBits + GenerationBits));
	static constexpr _Type IndexMask = (1ULL << IndexBits) - 1;
	static constexpr _Type GenerationMask = (1ULL << GenerationBits) - 1;
	
	_Type m_value;

	Handle(_Type idx = IndexMask, _Type gen = 0, bool active) : m_value((idx & IndexMask) | ((gen & GenerationMask) << _IndexBits) | (active ? ActiveMask : 0)) {}

	static Handle Generate(_Type index, _Type generation, bool active)
	{
		assert(index <= IndexMask && "Index out of range!");
		assert(generation <= GenerationMask && "Generation out of range!");

		return Handle(index, generation, active);
	}

	_Type GetIndex() const
	{
		return m_value & _IndexMask;
	}

	_Type GetGeneration() const
	{ // Shift and mask to get leading bits
		return (m_value >> _IndexBits) & _GenerationMask;
	}

	bool IsActive() const
	{
		return m_value & ActiveMask;
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

	static const Handle INVALID;
};

// This was confusing, defining a templated version of invalid handle so any created handle type has
// a corresponding Invalid. 
// These should only be accessed with type aliases for the Handle ie ObjectHandle::INVALID
// instead of a massive Handle<blah, blah, blah>
template <UnsignedIntegral _T, std::size_t _Idx, std::size_t _Gen>
const Handle<_T, _Idx, _Gen> Handle<_T, _Idx, _Gen>::INVALID{_IndexMask, 0};


// Finally! Handle type declarations! Systems can create their own too
using ObjectHandle = Handle<std::uint32_t, 20, 11>;
using ComponentHandle = Handle<std::uint32_t, 20, 11>;
using GenericHandle = Handle<std::uint32_t, 20, 11>;

template <typename _HT>
concept ValidHandleType = requires {
	{ _HT::IndexBits } -> std::integral;
	{ _HT::GenerationBits } -> std::integral;
};

template<typename _ObjectType, std::size_t _MaxItems = 1024ULL, typename _HandleType = GenericHandle>
//requires ValidHandleType<_HandleType>
class ObjectPool
{
public:
	using HandleType = _HandleType;
	using ObjectType = _ObjectType;

	ObjectPool() : m_objects(_MaxItems), m_freelistHead(0) 
	{
		// Iterate populate freelist handles, m_objects invalid initialized
		for (std::size_t i = 0; i < _MaxItems - 1; i++)
		{
			HandleType::UnderlyingType nextIndex = i + 1;
			m_objects[i].handle = HandleType(nextIndex, 0, false);
		}

		// Last element special, end of free list
		m_objects[_MaxItems - 1].handle = HandleType::INVALID;
	}

	template<typename... args>
	HandleType Create(Args&&... args)
	{
		if (m_objects.size() == _MaxItems)
		{
			// Hit max: throw? allocate more space?
		}

		// Get index/generation from freelistHead
		auto& freePool = m_objects[m_freelistHead];
		HandleType::UnderlyingType index = m_freelistHead;
		HandleType::UnderlyingType generation = freePool.handle.GetGeneration();

		// Update freelistHead with previous in chain
		m_freelistHead = freePool.handle.GetIndex();

		// Create handle, stow object
		HandleType handle = HandleType::Generate(index, generation + 1, true);
		m_objects[m_freelistHead] = std::pair<HandleType{}, ObjectType{std::forward<Args>(args)...}>;

		return handle;
	}

	ObjectType& Get(const HandleType& handle)
	{
		// Validate handle id and gen
		if (!IsHandleValid(handle))
		{
			// Old handle: not sure what to return here, maybe throw?
		}
		return m_objects[handle.GetIndex()];
	}

	void Destroy(const HandleType& handle)
	{
		// validate and remove object from list update freelist
		if (!IsHandleValid(handle)) return;

		// TODO set pool entry handle to invalid, update freelist

	}

private:
	struct PoolEntry {
		HandleType handle;
		ObjectType object;
	};

	std::array<PoolEntry, _MaxItems> m_objects;
	HandleType::UnderlyingType m_freelistHead;

	bool IsHandleValid(const HandleType& handle) const
	{
		std::size_t index = handle.GetIndex();

		if (index < 0 || index >= _MaxItems || !handle.IsActive()) {
			return false;
		}
		return true;
	}
};