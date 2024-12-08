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
	Handle(_Type idx = IndexMask, _Type gen = 0, bool active = false) : m_value((idx & IndexMask) | ((gen & GenerationMask) << _IndexBits) | (active ? ActiveMask : 0)) {}

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
const Handle<_T, _Idx, _Gen> Handle<_T, _Idx, _Gen>::INVALID{Handle<_T, _Idx, _Gen>::IndexMask, 0, false};


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
	using UnderlyingHandleType = HandleType::UnderlyingType;
	
	struct PoolEntry
	{
		HandleType handle;
		ObjectType object;
	};

	ObjectPool() : m_freelistHead(0) 
	{
		// Allocate our pool, don't bother clearing data
		m_objects = new PoolEntry[_MaxItems];

		// Iterate populate freelist handles, m_objects invalid initialized
		for (std::size_t i = 0; i < _MaxItems - 1; i++)
		{
			UnderlyingHandleType nextIndex = i + 1;
			m_objects[i].handle = HandleType(nextIndex, 0, false);
		}

		// Last element special, end of free list
		m_objects[_MaxItems - 1].handle = HandleType::INVALID;
	}

	~ObjectPool()
	{
		if (m_objects != nullptr)
		{
			free(m_objects);
		}
	}

	template<typename... Args>
	HandleType Create(Args&&... args)
	{
		if (m_objects.size() == _MaxItems)
		{
			// Hit max: throw? allocate more space?
			std::cout << "Failed to create new object, out of space!" << std::endl;
			return HandleType::INVALID;
		}

		// Get index/generation from freelistHead
		auto& freePool = m_objects[m_freelistHead];
		UnderlyingHandleType index = m_freelistHead;
		UnderlyingHandleType generation = freePool.handle.GetGeneration();

		// Update freelistHead with previous in chain
		m_freelistHead = freePool.handle.GetIndex();

		// Create handle, stow object
		HandleType handle = HandleType::Generate(index, generation + 1, true);
		m_objects[index] = PoolEntry(handle, ObjectType{ std::forward<Args>(args)... });

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
		UnderlyingHandleType currentIndex = handle.GetIndex();
		UnderlyingHandleType currentGen = handle.GetGeneration();
		auto& entry = m_objects[currentIndex];
		entry.handle = HandleType::Generate(m_freelistHead, currentGen, false);
		m_freelistHead = currentIndex;
	}

private:

	PoolEntry* m_objects;
	UnderlyingHandleType m_freelistHead;

	bool IsHandleValid(const HandleType& handle) const
	{
		if (handle == HandleType::INVALID)
		{
			return false;
		}

		// Index Bounds Checks
		UnderlyingHandleType index = handle.GetIndex();
		if (index < 0 || index >= _MaxItems)
		{
			return false;
		}

		// Handle is being used a freelist entry
		if (!handle.IsActive())
		{
			return false;
		}

		// Handle generation is outdated
		UnderlyingHandleType gen = handle.GetGeneration();
		auto& entry = m_objects[index];
		UnderlyingHandleType currentGen = entry.handle.GetGeneration();

		if (gen != currentGen)
		{
			return false;
		}

		return true;
	}

	struct ObjectPoolEntryIterator
	{
	public:
		using Category = std::forward_iterator_tag;
		using Distance = std::ptrdiff_t;

		using Value = PoolEntry;
		using Pointer = PoolEntry*;
		using Reference = PoolEntry&;


		using PoolEntryType = typename PoolEntry;

		ObjectPoolEntryIterator(Pointer ptr)
			: m_ptr(ptr) {}

		bool operator!=(const ObjectPoolIterator& other) const
		{
			return m_index != other.m_index;
		}

		ObjectPoolEntryIterator& operator++()
		{
			++m_index;
			return *this;
		}

		typename ObjectPool::ObjectType& operator*() const
		{
			return m_pool.m_objects[m_index].object;
		}

	private:
		void Advance()
		{
			while (m_index < m_pool.m_objects.size())
			{
				++m_index;
			}
		}

		ObjectPool& m_pool;
		Pointer m_ptr;
	};

public:

	ObjectPoolEntryIterator begin()
	{
		return ObjectPoolEntryIterator();
	}

	ObjectPoolEntryIterator end()
	{
		return ObjectPoolEntryIterator()
	}
};

// Some templating fun here. Essentially we have a couple of ways to iterate over
// our object pool, over all valid (default) or all entries including freelist
// this is also a way to learn more about templating and iterators :D
template <typename PoolEntryType>
struct ActiveOnlyPolicy
{
	bool ShouldInclude(const PoolEntryType& entry) const
	{
		return entry.handle.IsActive();
	}
};

template <typename PoolEntryType>
struct AllEntriesPolicy
{
	bool ShouldInclude(const PoolEntryType& entry) const
	{
		return true;
	}
};

// = ActiveOnlyPolicy<typename ObjectPoolType::PoolEntry>
//template <typename ObjectPoolType, typename IterationPolicyType>
