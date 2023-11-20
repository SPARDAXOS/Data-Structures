#include <algorithm>
#include <math.h>

#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

template<typename T>
class DynamicArray final {
	using Type = T;
	using Iterator = T*;
	using Reference = T&;

public:
	explicit DynamicArray() noexcept = default;
	explicit DynamicArray(size_t capacity) noexcept { Reserve(capacity); }
	explicit DynamicArray(size_t capacity, Type element) noexcept { Reserve(capacity); std::fill(Begin(), End(), element); }

	~DynamicArray() {
		if (m_Size > 0)
			delete[] m_Iterator;
		std::cout << "dtor" << std::endl;
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
	void Pushback(const Type element) {
		if (m_Capacity == 0)
			Reserve(1);
		else if (m_Size == m_Capacity)
			Reserve(m_Capacity * 2);

		//If exceeds max type
		//If allocation fails
		//If has no copy ctor


		Iterator NewElement = new Type(element);
		std::memmove(&m_Iterator[m_Size], NewElement, sizeof(Type));
		delete NewElement;
		m_Size++;
	}
	void EmplaceBack(const Reference element) {

	}

	inline void PopBack() {
		if (m_Size == 0)
			return;

		Iterator Target = m_Iterator + (m_Size - 1);
		//if (std::is_fundamental_v<Type>)
		//	*Target = 0;
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
		delete[] m_Iterator;
		m_Size = 0;
	}

public:
	inline Iterator Data() const noexcept { return m_Iterator; }
	inline size_t Size() const noexcept { return m_Size; }
	inline size_t Capacity() const noexcept { return m_Capacity; }
	inline bool Empty() const noexcept { return m_Size > 0; }
	inline Reference Front() const noexcept { return m_Iterator[0]; }
	inline Reference Back() const noexcept { return m_Iterator[m_Size - 1]; }
	inline Iterator Begin() const noexcept { return m_Iterator; }
	inline Iterator End() const noexcept { return &m_Iterator[m_Capacity]; }

	inline constexpr size_t MaxSize() const noexcept { return static_cast<size_t>(pow(2, sizeof(Iterator) * 8) / sizeof(Type) - 1); }

	inline void Reserve(size_t capacity) {
		if (m_Capacity > capacity)
			return;

		Iterator NewBuffer = static_cast<Iterator>(malloc(sizeof(Type) * capacity));
		if (!NewBuffer)
			throw std::bad_alloc();

		m_Capacity = capacity;
		if (m_Size == 0) {
			m_Iterator = NewBuffer;
			return;
		}

		std::memmove(NewBuffer, m_Iterator, m_Size * sizeof(Type));
		free(m_Iterator);
		m_Iterator = NewBuffer;
	}

	//PopBack
	//Insert
	//Swap (other vec?)`Make sure to test vecs of different template types
	//Emplace
	//Emplace back
	//ShrinkToFit
	//Resize

private:

	//Use Bitshift to remove and iterate on the array.


private:
	Iterator m_Iterator = nullptr;
	size_t m_Size = 0;
	size_t m_Capacity = 0;
};
#endif // !DYNAMIC_ARRAY

