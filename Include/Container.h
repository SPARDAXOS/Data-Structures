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

	//EDITING THIS! THE ALLOCATOR PART
	template<class InputIterator>
	constexpr Container(InputIterator first, InputIterator last, const Allocator& allocator = Allocator()) {
		m_Allocator = allocator;
		assign(first, last);
	}







	constexpr Container(std::initializer_list<Type> list, const Allocator& allocator = Allocator()) {
		assign(list);
	}
	 


	~Container() {
		//Could be made into func destroy_and_deallocate()
		clear();
		deallocate_memory_block(m_Capacity);
		//m_Allocator.deallocate<Type>(m_Iterator, m_Capacity);
		//std::cout << "Array Dtor" << std::endl;
	}


	//Copy Semantics
	constexpr Container(const Container& other) //NOT TESTED
		: m_Size(other.m_Size), m_Allocator(AllocatorTraits::select_on_container_copy_construction(other.m_Allocator))
	{
		//Could be made into func
		reserve(m_Size);
		std::copy(other.begin(), other.end(), m_Iterator); //In the example, they are constructed in place
	}
	constexpr Container(const Container& other, const Allocator& allocator)  //NOT TESTED
		: m_Size(other.m_Size), m_Allocator(allocator)
	{
		//Could be made into func
		reserve(m_Size);
		std::copy(other.begin(), other.end(), m_Iterator); //In the example, they are constructed in place
	}
	Container& operator=(const Container& other) noexcept { //NOT TESTED
		if (this == &other)
			return *this;

		//If new alloc is not equal to new then use old to dealloc
		//Otherwise, dont dealloc old mem and see if you can reuse it otherwise make it fit
		//Copy elements

		clear();

		if constexpr (AllocatorTraits::propagate_on_container_copy_assignment::value) {


			auto OldAllocator = this->m_Allocator;
			this->m_Allocator = other->m_Allocator;
			if (this->m_Allocator != OldAllocator) {
				OldAllocator.deallocate<Type>(this->m_Iterator, this->m_Capacity);

				//reserve(other.capacity());
				//assign(other.begin(), other.end());
				//return *this;
			}
		}

		//This implies that if my old resuable memory is big enough then i dont copy the capacity but keep my own?
		//Is copying capacity even logical? It might seem weird tho...

		if (other.size() > this->m_Capacity) {
			reserve(other.capacity());
			assign(other.begin(), other.end());
		}
		else
			assign(other.begin(), other.end());

		return *this;
	}


	//Move Semantics
	constexpr Container(Container&& other) noexcept //NOT TESTED
		: m_Allocator(std::move(other.m_Allocator)), m_Iterator(other.m_Iterator), m_Size(other.m_Size), m_Capacity(other.m_Capacity)
	{
		other.wipe();
	}
	constexpr Container(Container&& other, const Allocator& allocator)  //NOT TESTED
		: m_Size(other.m_Size), m_Capacity(other.m_Capacity), m_Allocator(std::move(allocator))
	{
		if (allocator != other.get_allocator()) {
			//This is the same code as the one in the move assignment operator overload. Make it resusable uninitialized_alloc_move
			if (other.size() > this->m_Capacity)
				reallocate(other.capacity()); //Will get me its capacity ONLY IF I CANT TAKE IT WITH MY OWN //Should probably also copy its capacity always for a clear move semantics

			//This also constructs in place in the example.
			for (SizeType i = 0; i < other.size(); i++) //Could be func move_elements or could resuse assign once it replaces.
				this->at(i) = std::move(other[i]);
		}
		else
			m_Iterator = other.m_Iterator;

		other.wipe();
	}
	Container& operator=(Container&& other) noexcept { //NOT TESTED
		if (this == &other)
			return *this;


		if constexpr (AllocatorTraits::propagate_on_container_move_assignment::value) { //SUCCESS
			clear(); //Could be merged with below destroy_all_and_deallocate()
			deallocate_memory_block(m_Capacity);

			this->m_Allocator = std::move(other.get_allocator());
			this->m_Iterator  = other.m_Iterator;
			this->m_Size      = other.m_Size;
			this->m_Capacity  = other.m_Capacity;

			other.wipe();
		}
		else if (!AllocatorTraits::propagate_on_container_move_assignment::value && this->m_Allocator == other.get_allocator()) { //NOT TESTED
			clear(); //Could be merged with below destroy_all_and_deallocate()
			deallocate_memory_block(m_Capacity);

			this->m_Iterator = other.m_Iterator;
			this->m_Size = other.m_Size;
			this->m_Capacity = other.m_Capacity;

			other.wipe();
		}
		else if (!AllocatorTraits::propagate_on_container_move_assignment::value && this->m_Allocator != other.get_allocator()) { //NOT TESTED
			//Cant take ownership of memory block, add elements instead and allocate memory if needed.
			clear();
			if (other.size() > this->m_Capacity)
				reallocate(other.capacity()); //Will get me its capacity ONLY IF I CANT TAKE IT WITH MY OWN //Should probably also copy its capacity always for a clear move semantics

			for (SizeType i = 0; i < other.size(); i++) //Could be func move_elements or could resuse assign once it replaces.
				this->at(i) = std::move(other[i]);

			this->m_Size = other.m_Size;
			this->m_Capacity = other.m_Capacity;

			other.deallocate_memory_block(other.capacity()); //Clear its memory
			other.wipe();
		}

		//In any case, original elements are all destroyed or replaced by element-wise move-assignment

		return *this;
	}
	//constexpr vector& operator=(std::initializer_list<T> ilist); //Last overload for assignment operator

	inline Reference operator[](SizeType index) const noexcept { return m_Iterator[index]; }

public:
	constexpr inline void push_back(ConstantReference element) {

		//Calls emplace_back 

		if (m_Capacity == 0)
			reserve(1);
		else if (m_Size == m_Capacity)
			reserve(m_Capacity * 2);

		if (!std::copy_constructible<Type>)
			throw std::invalid_argument("Type needs to be copy contructable!");

		AllocatorTraits::construct(m_Allocator, m_Iterator + m_Size, element);
		m_Size++;
	}
	constexpr inline void push_back(Type&& value) {
		emplace_back(std::move(value));
	}

	template<class... args>
	constexpr inline Iterator emplace(Iterator position, args&&... arguments) {


	}
	template<class... args>
	constexpr inline Reference emplace_back(args&&... arguments) {

		//Calls emplace but at the back
		if (m_Capacity == 0)
			reserve(1);
		else if (m_Size == m_Capacity)
			reserve(m_Capacity * 2);

		AllocatorTraits::construct(m_Allocator, m_Iterator + m_Size, arguments...);
		m_Size++;
		return back();
	}

	constexpr inline void pop_back() {
		if (m_Size == 0)
			return;

		if (std::destructible<Type>)
			std::allocator_traits<CustomAllocator<Type>>::destroy(m_Allocator, m_Iterator + (m_Size - 1));

		m_Size--;
	}

	constexpr inline Reference at(SizeType index) {
		if (index >= m_Size)
			throw std::out_of_range("Index out of bounds");

		return m_Iterator[index];
	}
	constexpr inline ConstantReference at(SizeType index) const {
		if (index >= m_Size)
			throw std::out_of_range("Index out of bounds");

		return m_Iterator[index];
	}

	//SETS THE SIZE TO 0 TOO! IT DESTROYS
	constexpr inline void clear() noexcept {
		if (m_Size == 0)
			return;

		destroy(begin(), end());
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

			if (std::destructible<Type>) //Reuse destroy
				std::allocator_traits<CustomAllocator<Type>>::destroy(m_Allocator, iterator);

			std::shift_left(begin() + Index, end(), 1);
			m_Size--;
			return begin() + Index;
		}
	}
	constexpr inline Iterator erase_if(ConstantIterator iterator, Predicate predicate) {
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
		//std::distance //not hjere maybe 
		if (first > last)
			throw std::invalid_argument("Invalid range!");

		SizeType StartIndex = find_position(first);
		if (StartIndex == INVALID_INDEX)
			throw std::invalid_argument("Start iterator out of bounds!");

		SizeType EndIndex = find_position(last);
		if (EndIndex == INVALID_INDEX)
			throw std::invalid_argument("End interator out of bounds!");

		if (StartIndex > EndIndex)
			throw std::invalid_argument("Start iterator overlaps end interator!");


		Iterator OldEnd = end();
		destroy(m_Iterator + StartIndex, m_Iterator + EndIndex);

		std::shift_left(begin() + StartIndex, OldEnd, (EndIndex + 1) - StartIndex); //Doesnt work after refactor due to destroy modifying size

		return begin() + StartIndex;
	}
	constexpr inline Iterator erase_if(ConstantIterator first, ConstantIterator last, Predicate predicate) {
		if (m_Size == 0)
			return nullptr;

		if (first == last)
			return nullptr; //?? check ref cpp

		if (first > last)
			throw std::invalid_argument("Invalid range!");

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

		return nullptr; //??
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

		//from here down
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



	//THEY DO NOT INCREASE THE SIZE!!!! Emplace back actually does The replacement part is my only worry since it shouldnt emplace back but rather insert
	//It replaces elements , from beginning only? size could stay still or increase
	constexpr void assign(SizeType count, const Reference value) {
		if (m_Size > 0)
			clear(); //? wot It should do this! Replace instead!

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
	constexpr inline Iterator allocate_memory_block(const SizeType capacity) {
		if (capacity > maxSize())
			throw std::length_error("Max allowed container size exceeded!");

		//No guarantee
		Iterator NewBuffer = m_Allocator.allocate(sizeof(Type) * capacity);
		if (!NewBuffer)
			throw std::bad_alloc();

		m_Capacity = capacity;

		return NewBuffer;
	}
	constexpr inline void deallocate_memory_block(const SizeType size) {
		if (!m_Iterator)
			return;


		m_Allocator.deallocate<Type>(m_Iterator, size);
		//Capacity == 0; ?? might cause complications with the other alloc funcs
		m_Iterator = nullptr;
	}
	constexpr inline void reallocate(const SizeType capacity) {


		Iterator NewBlock = allocate_memory_block(capacity * 2);
		if (!NewBlock)
			return; //??

		if (m_Size > 0) {
			std::memmove(NewBlock, m_Iterator, m_Size * sizeof(Type));
			deallocate_memory_block((m_Capacity / 2)); //?? why 2/ allocate_memory_block would have doubled the size by this point
			m_Iterator = NewBlock;
		}

		m_Iterator = NewBlock;
	}

	constexpr inline void destroy(Iterator target) noexcept {
		if (!target)
			return;

		if (std::destructible<Type>) // Will pass check even if fundemental
			AllocatorTraits::destroy(m_Allocator, target);

		m_Size--;
	}
	constexpr inline void destroy(Iterator first, Iterator last) noexcept {
		if (!first || !last)
			return;

		if (first > last)
			return;

		if (first == last)
			destroy(first);

		if (std::destructible<Type>) { // Will pass check even if fundemental
			for (Iterator i = first; i < last; i++)
				AllocatorTraits::destroy(m_Allocator, i);
		}
		std::cout << last - first;
		m_Size -= last - first;
	}

	constexpr inline void wipe() noexcept {
		clear(); //Test this!
		m_Iterator = nullptr;
		m_Size = 0;
		m_Capacity = 0;
	}

private:
	constexpr inline int find_position(Iterator iterator) const noexcept {
		if (iterator == begin())
			return 0;
		if (iterator == end())
			return static_cast<int>(m_Size);  //Scary

		for (int i = 0; i < m_Size; i++) {
			if (m_Iterator + i == iterator)
				return i;
		}
		return INVALID_INDEX;
	}
	constexpr inline int find_position(ConstantIterator iterator) const noexcept {
		if (iterator == begin())
			return 0;
		if (iterator == end())
			return static_cast<int>(m_Size); //Scary

		for (int i = 0; i < m_Size; i++) {
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