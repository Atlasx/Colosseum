#pragma once

#include "Handle.h"

#include <vector>
#include <cstdint>
#include <type_traits>
#include <cassert>
#include <limits>
#include <concepts>
#include <array>
#include <iostream>


template<typename _ObjectType, std::size_t _MaxItems = 1024ULL, typename _HandleType = GenericHandle>
requires ValidHandleType<_HandleType>
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

		PoolEntry() 
		{
			handle = HandleType::INVALID;
		}
		PoolEntry(HandleType h, ObjectType obj) : handle(h), object(obj) {}

		~PoolEntry() {}
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
			delete[] m_objects;
		}
	}

	template<typename... Args>
	HandleType Create(Args&&... args)
	{
		return CreateWithType(0, 0, std::forward<Args>(args)...);
	}

	template<typename... Args>
	HandleType CreateWithType(UnderlyingHandleType aType, UnderlyingHandleType aFlags, Args&&... args)
	{
		if (m_count >= _MaxItems)
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
		HandleType handle = HandleType::Generate(index, generation + 1, true, aFlags, aType);
		m_objects[index] = PoolEntry(handle, ObjectType{ std::forward<Args>(args)... });
		m_count++;

		return handle;
	}

	void Destroy(const HandleType& handle)
	{
		// validate and remove object from list, update freelist
		if (!IsHandleValid(handle)) return;

		UnderlyingHandleType currentIndex = handle.GetIndex();
		UnderlyingHandleType currentGen = handle.GetGeneration();

		auto& entry = m_objects[currentIndex];
		entry.handle = HandleType::Generate(m_freelistHead, currentGen, false);
		m_freelistHead = currentIndex;
		m_count--;
	}

	ObjectType& operator[](const HandleType& handle)
	{
		return Get(handle);
	}

	ObjectType& Get(const HandleType& handle)
	{
		// Validate handle id and gen
		assert(IsHandleValid(handle));

		return m_objects[handle.GetIndex()].object;
	}

	std::size_t Capacity()
	{
		return _MaxItems;
	}


private:

	PoolEntry* m_objects;
	std::size_t m_count = 0;
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

	// TODO Iterator doesn't have proper bounds checks
	struct Iterator
	{
	public:
		using Category = std::forward_iterator_tag;
		using Distance = std::ptrdiff_t;

		using Value = PoolEntry;
		using Pointer = PoolEntry*;
		using Reference = PoolEntry&;

		Iterator(Pointer ptr) : m_ptr(ptr) {}

		Reference operator*() const
		{
			return *m_ptr;
		}

		Pointer operator->()
		{
			return m_ptr;
		}

		Iterator& operator++()
		{
			do
			{
				++(m_ptr);
			}
			while ((*m_ptr).handle.IsActive() == false);
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}

		bool operator==(const Iterator& other) const
		{
			return m_ptr == other.m_ptr;
		}

		bool operator!=(const Iterator& other) const
		{
			return !(*this == other);
		}

	private:

		Pointer m_ptr;
	};

public:

	Iterator begin()
	{
		return Iterator(&m_objects[0]);
	}

	Iterator end()
	{
		return Iterator(&m_objects[_MaxItems]);
	}
};
