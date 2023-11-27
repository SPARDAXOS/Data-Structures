#include <Allocator.h>
#include <algorithm>
#include <math.h>
#include <functional>

#ifndef CONTAINER
#define CONTAINER

constexpr auto INVALID_INDEX = -1;
static auto ALLOCATOR_ID = 0;

template<typename T>
class Container final {
	using Type = T;
	using Iterator = T*;
	using Reference = T&;
	using ConstantIterator = const T*;
	using ConstantReference = const T&;
	using SizeType = std::size_t;
	using Predicate = std::function<bool(const T&)>;

public:
	explicit Container() noexcept 
		:	m_Allocator(ALLOCATOR_ID)
	{
		ALLOCATOR_ID++;
	};
	explicit Container(SizeType capacity) noexcept { reserve(capacity); }
	explicit Container(SizeType capacity, Type element) noexcept { reserve(capacity); std::fill(begin(), begin() + m_Capacity - 1, element); }

	~Container() {
		Clear();
		m_Allocator.deallocate<Type>(m_Iterator, m_Capacity);
		std::cout << "Array Dtor" << std::endl;
	}

	Container(const Container& other) {
		//*this = other;
		//New allocator by traits func then uses it to allocate resources

		this->m_Allocator = std::allocator_traits<CustomAllocator<Type>>::select_on_container_copy_construction(other.m_Allocator);
		Assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
	}
	Container& operator=(const Container& other) noexcept {
		if (this->m_Iterator == other.m_Iterator)
			return *this;
		else {

			//Copy assing the allocator if the container move assignment is falde
			if (std::allocator_traits<CustomAllocator<Type>>::propagate_on_container_copy_assignment::value) {
				if (*this != other)
					Clear();

				this->m_Allocator = other->m_Allocator;
			}

			Assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));


			//this->m_Allocator = std::allocator_traits<CustomAllocator<Type>>::select_on_container_copy_construction(other.m_Allocator);
			//
			//reserve(other.m_Capacity);
			//std::memmove(this->m_Iterator, other.m_Iterator, other.m_Size * sizeof(Type));
			//this->m_Size = other.m_Size;

			return *this;
		}
	}

	Container(Container&& other) noexcept {
		//Move constructs the allocator and steals resources
		m_Allocator = std::move(other.m_Allocator);

		this->m_Iterator = other.m_Iterator;
		this->m_Size = other.m_Size;
		this->m_Capacity = other.m_Capacity;

		other.m_Iterator = nullptr;
		other.m_Size = 0;
		other.m_Capacity = 0;


		//*this = std::move(other);
	}
	Container& operator=(Container&& other) noexcept {
		if (this->m_Iterator == other.m_Iterator)
			return *this;
		else {
			Clear();//Deallocate memory using own allocator

			if constexpr (std::allocator_traits<CustomAllocator<Type>>::propagate_on_container_move_assignment::value) {
				this->m_Allocator = std::move(other.m_Allocator); //Move assign from rhs allocator

				//Memory ownership + stuff
				this->m_Iterator = other.m_Iterator;
				this->m_Size = other.m_Size;
				this->m_Capacity = other.m_Capacity;

				other.m_Iterator = nullptr;
				other.m_Size = 0;
				other.m_Capacity = 0;
			}
			else if (!std::allocator_traits<CustomAllocator<Type>>::propagate_on_container_move_assignment::value && this->m_Allocator == other.m_Allocator) {
				//Move assignment is skipped for allocators
				 
				//Memory ownership + stuff
				this->m_Iterator = other.m_Iterator;
				this->m_Size = other.m_Size;
				this->m_Capacity = other.m_Capacity;

				other.m_Iterator = nullptr;
				other.m_Size = 0;
				other.m_Capacity = 0;
			}
			else if (!std::allocator_traits<CustomAllocator<Type>>::propagate_on_container_move_assignment::value && this->m_Allocator != other.m_Allocator) {
				Assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
			}




			return *this;
		}
	}

	inline Reference operator[](unsigned int index) const noexcept { return m_Iterator[index]; }

public:
	void Pushback(ConstantReference element) {
		using Allocator = std::allocator_traits<CustomAllocator<Type>>;

		if (m_Capacity == 0)
			reserve(1);
		else if (m_Size == m_Capacity)
			reserve(m_Capacity * 2);

		if (!std::copy_constructible<Type>)
			throw std::invalid_argument("Type needs to be copy contructable!");

		Allocator::construct(m_Allocator, m_Iterator + m_Size, element);
		m_Size++;
	}

	template<class... args>
	constexpr Reference Emplaceback(args&&... arguments) {
		using Allocator = std::allocator_traits<CustomAllocator<Type>>;

		if (m_Capacity == 0)
			reserve(1);
		else if (m_Size == m_Capacity)
			reserve(m_Capacity * 2);

		Allocator::construct(m_Allocator, m_Iterator + m_Size, arguments...);
		m_Size++;
		return back();
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

		if (iterator == end() - 1) {
			Popback();
			return m_Iterator + (m_Size - 1);
		}
		else if (iterator == begin()) {
			if (std::destructible<Type>)
				iterator->~Type();

			std::shift_left(m_Iterator, end(), 1);
			m_Size--;
			return m_Iterator;
		}
		else {
			int Index = FindIndex(iterator);
			if (Index == -1)
				throw std::invalid_argument("Iterator out of bounds!");

			if (std::destructible<Type>)
				iterator->~T();

			std::shift_left(begin() + Index, end(), 1);
			m_Size--;
			return begin() + Index;
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

		std::shift_left(begin() + StartIndex, end(), (EndIndex + 1) - StartIndex);
		m_Size -= (EndIndex + 1) - StartIndex;

		return begin() + StartIndex;
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
	inline Iterator data() const noexcept { return m_Iterator; }
	inline SizeType size() const noexcept { return m_Size; }
	inline SizeType capacity() const noexcept { return m_Capacity; }
	inline bool empty() const noexcept { return m_Size > 0; }
	inline Reference front() const noexcept { return m_Iterator[0]; }
	inline Reference back() const noexcept { return m_Iterator[m_Size - 1]; }
	inline Iterator begin() const noexcept { return m_Iterator; }
	inline Iterator end() const noexcept { return m_Iterator + m_Size; }

	inline constexpr CustomAllocator<Type> get_allocator() const noexcept { return m_Allocator; }

	constexpr inline SizeType maxSize() const noexcept { return static_cast<SizeType>(pow(2, sizeof(Iterator) * 8) / sizeof(Type) - 1); }

	constexpr inline void reserve(SizeType capacity) {
		if (m_Capacity > capacity)
			return;
		if (capacity > maxSize())
			throw std::length_error("Max allowed container size exceeded!");

		Iterator NewBuffer = m_Allocator.allocate(sizeof(Type) * capacity);
		if (!NewBuffer)
			throw std::bad_alloc();

		m_Capacity = capacity;
		if (m_Size == 0) {
			m_Iterator = NewBuffer;
			return;
		}

		std::memmove(NewBuffer, m_Iterator, m_Size * sizeof(Type));
		m_Allocator.deallocate<Type>(m_Iterator, (m_Capacity / 2));
		m_Iterator = NewBuffer;
	}
	constexpr inline void ShrinkToFit() {
		if (m_Capacity == m_Size)
			return;

		if (m_Size == 0 && m_Capacity > 0) {
			m_Allocator.deallocate<Type>(m_Iterator, m_Capacity);
			m_Iterator = nullptr;
			m_Capacity = 0;
			return;
		}
		else {
			Iterator NewBuffer = m_Allocator.allocate<Type>(sizeof(Type) * m_Size);
			if (!NewBuffer)
				throw std::bad_alloc();

			SizeType DeallocationSize = m_Capacity;
			m_Capacity = m_Size;
			std::memmove(NewBuffer, m_Iterator, m_Size * sizeof(Type));
			m_Allocator.deallocate<Type>(m_Iterator, DeallocationSize);
			m_Iterator = NewBuffer;
		}
	}
	constexpr inline void Swap(Container<Type>& other) noexcept {
		SizeType capacity = this->m_Capacity;
		this->m_Capacity = other.m_Capacity;
		other.m_Capacity = capacity;
		
		SizeType SizeType = this->m_Size;
		this->m_Size = other.m_Size;
		other.m_Size = SizeType;
		
		Iterator Iterator = this->m_Iterator;
		this->m_Iterator = other.m_Iterator;
		other.m_Iterator = Iterator;

		if (std::allocator_traits<CustomAllocator<Type>>::propagate_on_container_swap::value)
			std::swap(m_Allocator, other.m_Allocator);
	}

	constexpr void Assign(SizeType count, const Reference value) {
		if (m_Size > 0)
			Clear();

		for (SizeType i = 0; i < count; i++)
			Emplaceback(value);
	}
	template<class InputIt>
	constexpr void Assign(InputIt first, InputIt last) {
		if (m_Size > 0)
			Clear();

		auto Current = first;
		while (Current != last + 1) {
			Emplaceback(*Current);
			Current++;
		}
	}
	constexpr void Assign(std::initializer_list<Type> list) {
		if (m_Size > 0)
			Clear();

		for (SizeType i = 0; i < list.size(); i++)
			Emplaceback(list.begin() + i);
	}


	//Insert
	//Emplace
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



namespace {

	//Operators


}




#endif // !CONTAINER