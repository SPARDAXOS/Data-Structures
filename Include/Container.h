#include <Allocator.h>
#include <algorithm>
#include <math.h>
#include <functional>

#ifndef CONTAINER_H
#define CONTAINER_H

constexpr auto INVALID_INDEX = -1;

template<
	class T, 
	class Alloc = CustomAllocator<T>
>
class Container final {
	using Type = T;
	using Allocator = Alloc;
	using SizeType = std::size_t;
	using Iterator = Type*;
	using ConstantIterator = const T*;
	using ReverseIterator = std::reverse_iterator<Iterator>;
	using ReverseConstantIterator = std::reverse_iterator<ConstantIterator>;
	using Reference = T&;
	using ConstantReference = const T&;
	using Predicate = std::function<bool(const T&)>;
	using AllocatorTraits = std::allocator_traits<CustomAllocator<Type>>;

public:
	//explicit Container() noexcept = default;
	explicit Container(SizeType capacity) noexcept { reserve(capacity); }
	explicit Container(SizeType capacity, Type element) noexcept { reserve(capacity); std::fill(begin(), begin() + m_Capacity - 1, element); }

	//ctor
	constexpr Container() noexcept (noexcept(Allocator())) {};

	constexpr explicit Container(const Allocator& allocator) noexcept
		:	m_Allocator(allocator)
	{ 
	}

	constexpr Container(SizeType count, ConstantReference value, const Allocator allocator = Allocator()) {
		reserve(count);
		for (unsigned int i = 0; i < count; i++)
			push_back(value);
	}
	constexpr explicit Container(SizeType count, const Allocator& allocator = Allocator()) {
		reserve(count);
		for (unsigned int i = 0; i < count; i++)
			emplace_back({});
	}

	template<class InputIterator>
	constexpr Container(InputIterator first, InputIterator last, const Allocator& allocator = Allocator()) {
		assign(first, last);
	}
	//Assign does not set size!!!

	//6 is copy ctor
	constexpr Container(const Container& other) 
		:	m_Allocator(AllocatorTraits::selec_on_container_copy_construction(other.m_Allocator))
	{
		m_Size = other.m_Size;
		reserve(m_Size);
		std::copy(other.begin(), other.end(), m_Iterator);
	}

	constexpr Container(const Container& other, const Allocator& allocator) {
		m_Allocator = Allocator(allocator);
		m_Size = other.size();
		reserve(m_Size);
		std::copy(other.begin(), other.end(), m_Iterator);
		//m_Allocator = Allocator(allocator); //Wouldnt make sense if i keep allocator cause its const ref!
	}

	//8 is move ctor
	constexpr Container(Container&& other) noexcept {
		m_Allocator = std::move(other.m_Allocator);

		this->m_Iterator = other.m_Iterator;
		this->m_Size = other.m_Size;
		this->m_Capacity = other.m_Capacity;

		other.m_Iterator = nullptr;
		other.m_Size = 0;
		other.m_Capacity = 0;
	}

	constexpr Container(Container&& other, const Allocator& allocator) {
		m_Allocator = std::move(allocator);
		m_Size = std::move(other.m_Size);
		m_Capacity = std::move(other.m_Capacity);
		if (allocator != m_Allocator) {
			m_Iterator = std::move(other.m_Iterator);


		}
	}

	constexpr Container(std::initializer_list<Type> list, const Allocator& allocator = Allocator()) {
		assign(list);
	}
	 


	~Container() {
		clear();
		m_Allocator.deallocate<Type>(m_Iterator, m_Capacity);
		//std::cout << "Array Dtor" << std::endl;
	}

	constexpr Container(const Container& other) {
		//*this = other;
		//New allocator by traits func then uses it to allocate resources

		this->m_Allocator = AllocatorTraits::select_on_container_copy_construction(other.m_Allocator);
		assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
	}
	Container& operator=(const Container& other) noexcept {
		if (this->m_Iterator == other.m_Iterator)
			return *this;
		else {
			//If new alloc is not equal to new then use old to dealloc
			//Otherwise, dont dealloc old mem and see if you can reuse it otherwise make it fit
			//Copy elements

			if constexpr (AllocatorTraits::propagate_on_container_copy_assignment::value) {

				clear(); //Not sure

				auto OldAllocator = this->m_Allocator;
				this->m_Allocator = other->m_Allocator;
				if (this->m_Allocator != OldAllocator) {
					OldAllocator.deallocate<Type>(this->m_Iterator, this->m_Capacity);
					reserve(other.capacity());
					assign(other.begin(), other.end());
				}
				else {
					//Move this out? this wot section with minor adjustments to the one above it?
					if (this->m_Capacity >= other.size())
						assign(other.begin(), other.end());
					else {
						reserve(other->capacity());
						assign(other.begin(), other.end());
					}
				}
			}
			//otherwise wot? 

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
			clear();//Deallocate memory using own allocator

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
				assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
			}




			return *this;
		}
	}

	inline Reference operator[](unsigned int index) const noexcept { return m_Iterator[index]; }

public:
	void push_back(ConstantReference element) {

		if (m_Capacity == 0)
			reserve(1);
		else if (m_Size == m_Capacity)
			reserve(m_Capacity * 2);

		if (!std::copy_constructible<Type>)
			throw std::invalid_argument("Type needs to be copy contructable!");

		AllocatorTraits::construct(m_Allocator, m_Iterator + m_Size, element);
		m_Size++;
	}

	template<class... args>
	constexpr Reference emplace_back(args&&... arguments) {

		if (m_Capacity == 0)
			reserve(1);
		else if (m_Size == m_Capacity)
			reserve(m_Capacity * 2);

		AllocatorTraits::construct(m_Allocator, m_Iterator + m_Size, arguments...);
		m_Size++;
		return back();
	}

	inline void pop_back() {
		if (m_Size == 0)
			return;

		if (std::destructible<Type>)
			std::allocator_traits<CustomAllocator<Type>>::destroy(m_Allocator, m_Iterator + (m_Size - 1));

		m_Size--;
	}

	inline Type at(int index) const {
		if (index >= m_Size || index < 0)
			throw std::invalid_argument("Index out of bounds");

		return m_Iterator[index];
	}

	inline void clear() {
		if (m_Size == 0)
			return;


		if (std::destructible<Type>) {
			for (unsigned int i = 0; i < m_Size; i++)
				std::allocator_traits<CustomAllocator<Type>>::destroy(m_Allocator, m_Iterator + i);
		}

		//Dtor
		//Pushback and emplace copy/inplace difference

		m_Size = 0;
	}


	constexpr inline Iterator erase(ConstantIterator iterator) {
		if (m_Size == 0)
			return nullptr;

		if (iterator == end() - 1) {
			pop_back();
			return m_Iterator + (m_Size - 1);
		}
		else if (iterator == begin()) {
			if (std::destructible<Type>)
				std::allocator_traits<CustomAllocator<Type>>::destroy(m_Allocator, iterator);

			std::shift_left(m_Iterator, end(), 1);
			m_Size--;
			return m_Iterator;
		}
		else {
			int Index = find_index(iterator);
			if (Index == -1)
				throw std::invalid_argument("Iterator out of bounds!");

			if (std::destructible<Type>)
				std::allocator_traits<CustomAllocator<Type>>::destroy(m_Allocator, iterator);

			std::shift_left(begin() + Index, end(), 1);
			m_Size--;
			return begin() + Index;
		}
	}
	constexpr inline Iterator EraseIf(ConstantIterator iterator, Predicate predicate) {
		if (m_Size == 0)
			return nullptr;
		if (predicate(*iterator))
			erase(iterator);

		return nullptr;
	}

	constexpr inline Iterator erase(ConstantIterator first, ConstantIterator last) {
		if (m_Size == 0)
			return nullptr;

		if (first == last)
			return nullptr; //??

		int StartIndex = find_index(first);
		if (StartIndex == INVALID_INDEX)
			throw std::invalid_argument("Start iterator out of bounds!");

		int EndIndex = find_index(last);
		if (EndIndex == INVALID_INDEX)
			throw std::invalid_argument("End interator out of bounds!");

		if (StartIndex > EndIndex)
			throw std::invalid_argument("Start iterator overlaps end interator!");

		//				std::allocator_traits<CustomAllocator<Type>>::destroy(m_Allocator, iterator);   ????????????
		//This needs to iterate and call dtor on elements otherwise its borken
		//Foreach from start indx to end inx, call destroy 

		//Test this!!!!!!!!!!!!!!
		for (int i = StartIndex; i < EndIndex + 1; i++)
			std::allocator_traits<CustomAllocator<Type>>::destroy(m_Allocator, m_Iterator + StartIndex);

		std::shift_left(begin() + StartIndex, end(), (EndIndex + 1) - StartIndex);
		m_Size -= (EndIndex + 1) - StartIndex;

		return begin() + StartIndex;
	}
	constexpr inline Iterator EraseIf(ConstantIterator first, ConstantIterator last, Predicate predicate) {
		if (m_Size == 0)
			return nullptr;

		if (first == last)
			return nullptr; //??

		int StartIndex = find_index(first);
		if (StartIndex == INVALID_INDEX)
			throw std::invalid_argument("Start iterator out of bounds!");

		int EndIndex = find_index(last);
		if (EndIndex == INVALID_INDEX)
			throw std::invalid_argument("End interator out of bounds!");

		if (StartIndex > EndIndex)
			throw std::invalid_argument("Start iterator overlaps end interator!");
		
		int Current = StartIndex;
		for (int i = 0; i < (EndIndex + 1) - StartIndex; i++) {
			if (predicate(*(m_Iterator + Current))) {
				erase(m_Iterator + Current);
				continue;
			}
			Current++;
		}

		return nullptr;
	}


public:
	constexpr inline Iterator data() const noexcept { return m_Iterator; }
	constexpr inline SizeType size() const noexcept { return m_Size; }
	constexpr inline SizeType capacity() const noexcept { return m_Capacity; }
	constexpr inline bool empty() const noexcept { return m_Size > 0; }
	constexpr inline Reference front() const noexcept { return m_Iterator[0]; }
	constexpr inline Reference back() const noexcept { return m_Iterator[m_Size - 1]; }

	constexpr inline Iterator begin() noexcept { return m_Iterator; }
	constexpr inline ConstantIterator begin() const noexcept { return m_Iterator; }
	constexpr inline ConstantIterator cbegin() const noexcept { return m_Iterator; }

	constexpr inline ReverseIterator rbegin() noexcept { return ReverseIterator(m_Iterator + (m_Size - 1)); }
	constexpr inline ReverseConstantIterator rbegin() const noexcept { return ReverseConstantIterator(m_Iterator + (m_Size - 1)); }
	constexpr inline ReverseConstantIterator crbegin() const noexcept { return rbegin(); }

	constexpr inline ReverseIterator rend() noexcept { return ReverseIterator(m_Iterator - 1); }
	constexpr inline ReverseConstantIterator rend() const noexcept { return ReverseConstantIterator(m_Iterator - 1); }
	constexpr inline ReverseConstantIterator crend() const noexcept { return rend(); }

	constexpr inline Iterator end() noexcept { return m_Iterator + m_Size; }
	constexpr inline ConstantIterator end() const noexcept { return m_Iterator + m_Size; }
	constexpr inline ConstantIterator cend() const noexcept { return m_Iterator + m_Size; }


	constexpr inline CustomAllocator<Type> get_allocator() const noexcept { return m_Allocator; }

	constexpr inline SizeType maxSize() const noexcept { return static_cast<SizeType>(pow(2, sizeof(Iterator) * 8) / sizeof(Type) - 1); }

	constexpr inline void reserve(SizeType capacity) {
		if (m_Capacity > capacity || m_Capacity == capacity) //The second check needs testing!
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
	constexpr inline void shrink_to_fit() {
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
	constexpr inline void swap(Container<Type>& other) noexcept {
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



	//THEY DO NOT INCREASE THE SIZE!!!!
	constexpr void assign(SizeType count, const Reference value) {
		if (m_Size > 0)
			clear();

		for (SizeType i = 0; i < count; i++)
			emplace_back(value);
	}
	template<class InputIt>
	constexpr void assign(InputIt first, InputIt last) {
		if (m_Size > 0)
			clear();

		auto Current = first;
		while (Current != last + 1) {
			emplace_back(*Current);
			Current++;
		}
	}
	constexpr void assign(std::initializer_list<Type> list) {
		if (m_Size > 0)
			clear();

		for (SizeType i = 0; i < list.size(); i++)
			emplace_back(list.begin() + i);
	}


	//Insert
	//Emplace
	//Resize

private:
	constexpr inline int find_index(Iterator iterator) const noexcept {
		for (unsigned int i = 0; i < m_Size; i++) {
			if (m_Iterator + i == iterator)
				return i;
		}
		return INVALID_INDEX;
	}
	constexpr inline int find_index(ConstantIterator iterator) const noexcept {
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




#endif // !CONTAINER_H