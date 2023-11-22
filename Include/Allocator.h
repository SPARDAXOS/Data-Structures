


#ifndef CUSTOM_ALLOCATOR
#define CUSTOM_ALLOCATOR

template<class Alloc>
class CustomAllocator final {
public:
	using value_type = Alloc;

public:
	CustomAllocator() noexcept {
		std::cout << "My allocator ctor ctor was called" << std::endl;
	}

	template <class U>
	inline CustomAllocator(const CustomAllocator<U>&) noexcept {
		std::cout << "My allocator custom ctor was called" << std::endl;
	}

	inline Alloc* allocate(std::size_t n) {
		
		m_Allocations++;
		m_AllocatedMemory += sizeof(Alloc);
		m_MemoryBlock = new Alloc[n];

		//Infinite loop
		std::cout << "Allocation was made with size " << n << std::endl;
		return m_MemoryBlock;
	}
	inline void Deallocate(Alloc* p, std::size_t n) {
		std::cout << "Deallocation was made with size " << n << std::endl;
	}

private:
	Alloc* m_MemoryBlock = nullptr;
	size_t m_Allocations = 0;
	size_t m_AllocatedMemory = 0;
};

namespace {
	template <class T, class U>
	constexpr bool operator==(const CustomAllocator<T>&, const CustomAllocator<U>&) noexcept {

	}

	template <class T, class U>
	constexpr bool operator!=(const CustomAllocator<T>&, const CustomAllocator<U>&) noexcept {

	}
}
#endif // !CUSTOM_ALLOCATOR


