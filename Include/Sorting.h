

#ifndef SORTING_H
#define SORTING_H


namespace Sorting {

	enum class Order{ ASCENDING, DESCENDING };

	template<typename Type>
	concept IsPointer = std::is_pointer_v<Type>;

	template<IsPointer Iterator>
	constexpr inline bool ValidateRange(Iterator start, Iterator end) noexcept {
		if (start == end)
			return false;

		if (start > end)
			return false;

		return true;
	}

	template<IsPointer Iterator>
	constexpr inline void ExchangeSort(Iterator start, Iterator end) noexcept {
		if (!ValidateRange(start, end))
			return;

		for (Iterator Cursor = start; Cursor < end; Cursor++) {
			for (Iterator AfterCursor = Cursor + 1; AfterCursor < end; AfterCursor++) {
				if (*Cursor < *AfterCursor) {
					auto Temp = *Cursor;
					*Cursor = *AfterCursor;
					*AfterCursor = Temp;
				}
			}
		}
	}

	template<IsPointer Iterator>
	constexpr inline void SelectionSort(Iterator start, Iterator end) noexcept {
		if (!ValidateRange(start, end))
			return;

		for (Iterator Cursor = start; Cursor < end; Cursor++) {
			Iterator LowestValueIterator = Cursor;
			for (Iterator AfterCursor = Cursor + 1; AfterCursor < end; AfterCursor++) {
				if (*AfterCursor < *LowestValueIterator)
					LowestValueIterator = AfterCursor;
			}
			if (*Cursor != *LowestValueIterator) {
				auto Temp = *Cursor;
				*Cursor = *LowestValueIterator;
				*LowestValueIterator = Temp;
			}
		}
	}

	template<IsPointer Iterator>
	constexpr inline void BubbleSort(Iterator start, Iterator end) noexcept {
		if (!ValidateRange(start, end))
			return;

		for (Iterator Cursor = start; Cursor < end; Cursor++) {
			for (Iterator AfterCursor = start; AfterCursor < end - 1; AfterCursor++) {

				if (*AfterCursor > *(AfterCursor + 1)) {
					auto Temp = *AfterCursor;
					*AfterCursor = *(AfterCursor + 1);
					*(AfterCursor + 1) = Temp;
				}
			}
		}
	}

	template<IsPointer Iterator>
	constexpr inline void InsertionSort(Iterator start, Iterator end) noexcept {
		if (!ValidateRange(start, end))
			return;

		for (Iterator Cursor = start; Cursor < end; Cursor++) {
			Iterator AfterCursor = Cursor;
			while (AfterCursor > start && *AfterCursor > *(AfterCursor - 1)) {
				auto Temp = *AfterCursor;
				*AfterCursor = *(AfterCursor - 1);
				*(AfterCursor - 1) = Temp;

				AfterCursor--;
			}
		}
	}

	template<IsPointer Iterator>
	constexpr inline void CountingSort(Iterator start, Iterator end) noexcept {
		if (!ValidateRange(start, end))
			return;

		//static_assert(decltype(Iterator) == float&& "CountingSort does not work on decimals!");

	}

	template<class Iterator>
	constexpr inline void QuickSort(Iterator* start, Iterator* end) noexcept {


	}

	template<class Iterator>
	constexpr inline void MergeSort(Iterator* start, Iterator* end) noexcept {


	}

	template<class Iterator>
	constexpr inline void HeapSort(Iterator* start, Iterator* end) noexcept {


	}
}
#endif // !SORTING_H

