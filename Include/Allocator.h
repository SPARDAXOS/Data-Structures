


#ifndef CUSTOM_ALLOCATOR
#define CUSTOM_ALLOCATOR

template<class _Alloc>
class CustomAllocator final {
public:
	using value_type = _Alloc;
	using pointer = _Alloc*;
	using size_type = std::size_t;
	using const_pointer = const _Alloc*;
	using const_reference = const _Alloc&;

	using propagate_on_container_move_assignment = std::true_type;
	using propagate_on_container_copy_assignment = std::true_type;
	using propagate_on_container_swap			 = std::true_type;
	//using is_always_equal						 = std::true_type; //C++17 for non-empty allocators that are always equal

public:
	CustomAllocator() noexcept = delete;/*  {
		std::cout << "My allocator ctor ctor was called" << std::endl;
	}*/

	CustomAllocator(int id) noexcept 
		: m_ID(id)
	{
	}
	~CustomAllocator() {
		if (m_AllocatedMemory > 0)
			std::cout << "Memory was not deallocated!\n" << m_AllocatedMemory << " Bytes was leaked!" << std::endl;
	}

	template <class U>
	inline CustomAllocator(const CustomAllocator<U>&) noexcept {
		std::cout << "My allocator custom ctor was called" << std::endl;
	}

	template<class... args>
	inline void construct(pointer address, args&&... arguments) {

		std::construct_at(address, std::forward<args>(arguments)...);

		m_Allocations++;
		m_AllocatedMemory += sizeof(value_type);
		std::cout << "Allocation (construct) was made with size " << sizeof(value_type) << std::endl;
	}


	[[nodiscard]] constexpr inline pointer allocate(const size_type size) {
		if (size == 0)
			return nullptr;

		m_Allocations++;
		m_AllocatedMemory += size;
		pointer AllocatedMemory = static_cast<pointer>(malloc(size));

		std::cout << "Allocation (allocate) was made with size " << size << std::endl;
		return AllocatedMemory;
	}
	template<typename T>
	inline void deallocate(T* address, size_type size) {
		if (!address)
			return;

		m_Allocations--;
		m_AllocatedMemory -= size * sizeof(T);
		m_Deallocations++;
		free(address);

		std::cout << "Deallocation was made with size " << size << std::endl;
	}

private:
	int m_ID;
	size_type m_Allocations = 0;
	size_type m_Deallocations = 0;
	size_type m_AllocatedMemory = 0;
};

namespace {
	template <class T, class U>
	constexpr bool operator==(const CustomAllocator<T>&, const CustomAllocator<U>&) noexcept {
		return true;
	}

	template <class T, class U>
	constexpr bool operator!=(const CustomAllocator<T>&, const CustomAllocator<U>&) noexcept {
		return false;
	}
}
#endif // !CUSTOM_ALLOCATOR


