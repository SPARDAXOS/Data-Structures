#include "Container.h"

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

		if ((end - start) < 2)
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


	inline void CountingSort(int* start, int* end) noexcept {
		if (!ValidateRange(start, end))
			return;


		int* CurrentElement = start;
		size_t HighestElementValue = *start;
		while (CurrentElement != end) {
			if (*CurrentElement > HighestElementValue)
				HighestElementValue = *CurrentElement;
			CurrentElement++;
		}


		int32 DefaultCount = 0;
		Container<int> Count; 
		Container<int> ElementsCopy(start, end - 1); //problem ALSO SHOULD NOT ADD END ELEMENT! OR CHECK CPPREF the ctor is the problem to check!
		Count.assign(++HighestElementValue, DefaultCount);

		for (int* i = start; i < end; i++)
			Count[*i]++;

		for (uint32 i = 1; i < Count.size(); i++)
			*(Count.begin() + i) += *(Count.begin() + i - 1);

		for (int* i = ElementsCopy.end() - 1; i > ElementsCopy.begin() - 1; i--) {
			Count[*i]--;
			*(start + (Count[*i])) = *i;
		}
	}

	template<IsPointer Iterator>
	constexpr inline void QuickSort(Iterator start, Iterator end) noexcept {


	}


	template<IsPointer Iterator>
	void Partition(Iterator start, Iterator end) noexcept {

		int64 RangeSize = (end - start) + 1;

		//Its possible to end up with a final partition between 3 elements if the count is odd.
		if (RangeSize == 1) {
			return; 
		}
		if (RangeSize == 2) {
			if (*start > *end) {
				auto Temp = *start;
				*start = *end;
				*end = Temp;
				return;
			}
		}
		else {

			int64 SplitPoint = RangeSize / 2;

			Iterator FirstHalfStart = start;
			Iterator FirstHalfEnd = start + (SplitPoint - 1);

			Iterator SecondHalfStart = start + SplitPoint;
			Iterator SecondHalfEnd = end;

			Partition(FirstHalfStart, FirstHalfEnd);
			Partition(SecondHalfStart, SecondHalfEnd);
		}
	}

	template<IsPointer Iterator>
	constexpr inline void MergeSort(Iterator start, Iterator end) noexcept {
		
		int64 RangeSize = end - start; //Removing 1 that is end
		int64 SplitPoint = RangeSize / 2;

		Iterator FirstHalfStart = start;
		Iterator FirstHalfEnd = start + (SplitPoint - 1);

		Iterator SecondHalfStart = start + SplitPoint;
		Iterator SecondHalfEnd = end - 1;

		std::cout << FirstHalfStart;
		std::cout << FirstHalfEnd;
		std::cout << SecondHalfStart;
		std::cout << SecondHalfEnd;

		Partition(FirstHalfStart, FirstHalfEnd);
		Partition(SecondHalfStart, SecondHalfEnd);
	}



	template<IsPointer Iterator>
	constexpr inline void HeapSort(Iterator start, Iterator end) noexcept {


	}
}
#endif // !SORTING_H

