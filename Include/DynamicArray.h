#include <algorithm>
#include <math.h>
#include <functional>
#include <memory>
#include <Allocator.h>

#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

constexpr auto INVALID_INDEX = -1;

template<typename T>
class DynamicArray final {
	using Type = T;
	using Iterator = T*;
	using Reference = T&;
	using ConstantIterator = const T*;
	using ConstantReference = const T&;
	using SizeType = std::size_t;
	using Predicate = std::function<bool(const T&)>;

public:
	explicit DynamicArray() noexcept = default;
	explicit DynamicArray(SizeType capacity) noexcept { Reserve(capacity); }
	explicit DynamicArray(SizeType capacity, Type element) noexcept { Reserve(capacity); std::fill(Begin(), Begin() + m_Capacity - 1, element); }

	~DynamicArray() {
		Clear();
		m_Allocator.deallocate<Type>(m_Iterator, m_Capacity * sizeof(Type));
		std::cout << "Array Dtor" << std::endl;
	}

	DynamicArray(const DynamicArray& other) {
		*this = other;
	}
	DynamicArray& operator=(const DynamicArray& other) noexcept {
		if (this->m_Iterator == other.m_Iterator)
			return *this;
		else {
			if (this->m_Size > 0)
				Clear();

			Reserve(other.m_Capacity);
			std::memmove(this->m_Iterator, other.m_Iterator, other.m_Size * sizeof(Type));
			this->m_Size = other.m_Size;

			return *this;
		}
	}

	DynamicArray(DynamicArray&& other) noexcept {
		*this = std::move(other);
	}
	DynamicArray& operator=(DynamicArray&& other) noexcept {
		if (this->m_Iterator == other.m_Iterator)
			return *this;
		else {
			if (this->m_Size > 0)
				Clear();

			this->m_Iterator = other.m_Iterator;
			this->m_Size     = other.m_Size;
			this->m_Capacity = other.m_Capacity;

			other.m_Iterator = nullptr;
			other.m_Size	 = 0;
			other.m_Capacity = 0;

			return *this;
		}
	}

	inline Reference operator[](unsigned int index) const noexcept { return m_Iterator[index]; }

public:
	void Pushback(ConstantReference element) {
		using Allocator = std::allocator_traits<CustomAllocator<Type>>;

		if (m_Capacity == 0)
			Reserve(1);
		else if (m_Size == m_Capacity)
			Reserve(m_Capacity * 2);

		if (!std::copy_constructible<Type>)
			throw std::invalid_argument("Type needs to be copy contructable!");

		//Iterator NewElement = new Type(element);
		Allocator::construct(m_Allocator, m_Iterator + m_Size, element);
		//if (!NewElement)
			//throw std::bad_alloc();

		//std::memmove(&m_Iterator[m_Size], NewElement, sizeof(Type));
		//delete NewElement;
		m_Size++;
	}

	template<class... args>
	constexpr Reference Emplaceback(args&&... arguments) {
		using Allocator = std::allocator_traits<CustomAllocator<Type>>;

		if (m_Capacity == 0)
			Reserve(1);
		else if (m_Size == m_Capacity)
			Reserve(m_Capacity * 2);

		Allocator::construct(m_Allocator, m_Iterator + m_Size, arguments...);
		m_Size++;
		return Back();
	}

	inline void Popback() {
		if (m_Size == 0)
			return;

		Iterator Target = m_Iterator + (m_Size - 1);
		if (std::destructible<Type>)
			Target->~Type();

		m_Size--;
	}

	inline Type At(int index) const {
		if (index >= m_Size || index < 0)
			throw std::invalid_argument("Index out of bounds");

		return m_Iterator[index];
	}

	inline void Clear() {
		if (m_Size == 0)
			return;

		if (std::destructible<Type>) {
			for (unsigned int i = 0; i < m_Size; i++)
				m_Iterator[i].~Type();
		}

		//Dtor
		//Pushback and emplace copy/inplace difference

		m_Size = 0;
	}


	constexpr inline Iterator Erase(ConstantIterator iterator) {
		if (m_Size == 0)
			return nullptr;

		if (iterator == End() - 1) {
			Popback();
			return m_Iterator + (m_Size - 1);
		}
		else if (iterator == Begin()) {
			if (std::destructible<Type>)
				iterator->~Type();

			std::shift_left(m_Iterator, End(), 1);
			m_Size--;
			return m_Iterator;
		}
		else {
			int Index = FindIndex(iterator);
			if (Index == -1)
				throw std::invalid_argument("Iterator out of bounds!");

			if (std::destructible<Type>)
				iterator->~T();

			std::shift_left(Begin() + Index, End(), 1);
			m_Size--;
			return Begin() + Index;
		}
	}
	constexpr inline Iterator EraseIf(ConstantIterator iterator, Predicate predicate) {
		if (m_Size == 0)
			return nullptr;
		if (predicate(*iterator))
			Erase(iterator);

		return nullptr;
	}

	constexpr inline Iterator Erase(ConstantIterator first, ConstantIterator last) {
		if (m_Size == 0)
			return nullptr;

		if (first == last)
			return nullptr; //??

		int StartIndex = FindIndex(first);
		if (StartIndex == INVALID_INDEX)
			throw std::invalid_argument("Start iterator out of bounds!");

		int EndIndex = FindIndex(last);
		if (EndIndex == INVALID_INDEX)
			throw std::invalid_argument("End interator out of bounds!");

		if (StartIndex > EndIndex)
			throw std::invalid_argument("Start iterator overlaps end interator!");

		std::shift_left(Begin() + StartIndex, End(), (EndIndex + 1) - StartIndex);
		m_Size -= (EndIndex + 1) - StartIndex;

		return Begin() + StartIndex;
	}
	constexpr inline Iterator EraseIf(ConstantIterator first, ConstantIterator last, Predicate predicate) {
		if (m_Size == 0)
			return nullptr;

		if (first == last)
			return nullptr; //??

		int StartIndex = FindIndex(first);
		if (StartIndex == INVALID_INDEX)
			throw std::invalid_argument("Start iterator out of bounds!");

		int EndIndex = FindIndex(last);
		if (EndIndex == INVALID_INDEX)
			throw std::invalid_argument("End interator out of bounds!");

		if (StartIndex > EndIndex)
			throw std::invalid_argument("Start iterator overlaps end interator!");
		
		int Current = StartIndex;
		for (int i = 0; i < (EndIndex + 1) - StartIndex; i++) {
			if (predicate(*(m_Iterator + Current))) {
				Erase(m_Iterator + Current);
				continue;
			}
			Current++;
		}

		return nullptr;
	}


public:
	inline Iterator Data() const noexcept { return m_Iterator; }
	inline SizeType Size() const noexcept { return m_Size; }
	inline SizeType Capacity() const noexcept { return m_Capacity; }
	inline bool Empty() const noexcept { return m_Size > 0; }
	inline Reference Front() const noexcept { return m_Iterator[0]; }
	inline Reference Back() const noexcept { return m_Iterator[m_Size - 1]; }
	inline Iterator Begin() const noexcept { return m_Iterator; }
	inline Iterator End() const noexcept { return m_Iterator + m_Size; } //I NEED SOMETHING TO AFTER LAST AVAILABLE ELEMENT INSTEAD OF AFTER END!

	constexpr inline SizeType MaxSize() const noexcept { return static_cast<SizeType>(pow(2, sizeof(Iterator) * 8) / sizeof(Type) - 1); }

	constexpr inline void Reserve(SizeType capacity) {
		if (m_Capacity > capacity)
			return;
		if (capacity > MaxSize())
			throw std::length_error("Max allowed container size exceeded!");

		//Iterator NewBuffer = static_cast<Iterator>(malloc(sizeof(Type) * capacity));
		Iterator NewBuffer = m_Allocator.allocate(sizeof(Type) * capacity);
		if (!NewBuffer)
			throw std::bad_alloc();

		m_Capacity = capacity;
		if (m_Size == 0) {
			m_Iterator = NewBuffer;
			return;
		}

		std::memmove(NewBuffer, m_Iterator, m_Size * sizeof(Type));
		m_Allocator.deallocate<Type>(m_Iterator, (m_Capacity / 2) * sizeof(Type));
		m_Iterator = NewBuffer;
	}
	constexpr inline void ShrinkToFit() {
		if (m_Capacity == m_Size)
			return;

		if (m_Size == 0 && m_Capacity > 0) {
			m_Allocator.deallocate<Type>(m_Iterator, m_Capacity * sizeof(Type));
			m_Iterator = nullptr;
			m_Capacity = 0;
			return;
		}
		else {
			Iterator NewBuffer = m_Allocator.allocate<Type>(sizeof(Type) * m_Size);
			if (!NewBuffer)
				throw std::bad_alloc();

			SizeType DeallocationSize = m_Capacity * sizeof(Type);
			m_Capacity = m_Size;
			std::memmove(NewBuffer, m_Iterator, m_Size * sizeof(Type));
			m_Allocator.deallocate<Type>(m_Iterator, DeallocationSize);
			m_Iterator = NewBuffer;
		}
	}
	constexpr inline void Swap(DynamicArray<Type>& other) noexcept {
		SizeType Capacity = this->m_Capacity;
		this->m_Capacity = other.m_Capacity;
		other.m_Capacity = Capacity;

		SizeType SizeType = this->m_Size;
		this->m_Size = other.m_Size;
		other.m_Size = SizeType;

		Iterator Iterator = this->m_Iterator;
		this->m_Iterator = other.m_Iterator;
		other.m_Iterator = Iterator;
	}

	//Insert
	//Swap (other vec?)`Make sure to test vecs of different template types
	//Emplace
	//Emplace back
	//Resize

private:
	constexpr inline int FindIndex(Iterator iterator) const noexcept {
		for (unsigned int i = 0; i < m_Size; i++) {
			if (m_Iterator + i == iterator)
				return i;
		}
		return INVALID_INDEX;
	}
	constexpr inline int FindIndex(ConstantIterator iterator) const noexcept {
		for (unsigned int i = 0; i < m_Size; i++) {
			if (m_Iterator + i == iterator)
				return i;
		}
		return INVALID_INDEX;
	}


private:
	Iterator m_Iterator = nullptr;
	SizeType m_Size = 0;
	SizeType m_Capacity = 0;
	CustomAllocator<Type> m_Allocator;
};
#endif // !DYNAMIC_ARRAY