

#ifndef DYNAMIC_ARRAY
#define DYNAMIC_ARRAY

template<typename T>
class DynamicArray final {
	using Type = T;
	using Iterator = T*;

public:
	explicit DynamicArray() noexcept = default;
	explicit DynamicArray(size_t capacity) noexcept { Reallocate(capacity); }

	~DynamicArray() {
		if (m_Size > 0)
			delete[] m_Iterator;
	}

	DynamicArray(const DynamicArray& other) {
		this->std::copy(other);
	}
	DynamicArray& operator=(const DynamicArray& other) noexcept {
		if (this->m_Iterator == other.m_Iterator)
			return *this;
		else {

		}
	}

	DynamicArray(DynamicArray&& other) noexcept {
		this == std::move(other);
	}
	DynamicArray& operator=(DynamicArray&& other) noexcept {
		if (this->m_Iterator == other.m_Iterator)
			return *this;
		else {

		}
	}

public:
	void Pushback(const Type& element) {
		if (m_Capacity == 0)
			Reallocate(1);
		else if (m_Size == m_Capacity)
			Reallocate(m_Capacity * 2);


		const Iterator NewElement = new Type(element);
		std::memmove(&m_Iterator[m_Size], NewElement, sizeof(Type));
		m_Size++;
	}
	void EmplaceBack(const Type& element) {

	}

public:
	inline size_t GetSize() const noexcept { return m_Size; }
	inline size_t GetCapacity() const noexcept { return m_Capacity; }

private:
	inline void Reallocate(size_t capacity) {
		m_Capacity = capacity;
		Iterator NewBuffer = static_cast<Iterator>(malloc(sizeof(Type) * m_Capacity));
		if (m_Size == 0) {
			m_Iterator = NewBuffer;
			return;
		}

		std::memmove(NewBuffer, m_Iterator, m_Size * sizeof(Type));
		free(m_Iterator);
		m_Iterator = NewBuffer;
	}

	//Use Bitshift to remove and iterate on the array.


private:
	Iterator m_Iterator = nullptr;
	size_t m_Size = 0;
	size_t m_Capacity = 0;
};
#endif // !DYNAMIC_ARRAY

