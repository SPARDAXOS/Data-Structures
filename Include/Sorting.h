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
	std::pair<Iterator, Iterator> MergePartition(Iterator start, Iterator end) noexcept {

		int64 RangeSize = (end - start) + 1;

		if (RangeSize == 1)
			return std::pair<Iterator, Iterator>(start, start); //Just go back with the one element really..
		else {

			int64 SplitPoint = RangeSize / 2;

			Iterator FirstHalfStart = start;
			Iterator FirstHalfEnd = start + (SplitPoint - 1);

			Iterator SecondHalfStart = start + SplitPoint;
			Iterator SecondHalfEnd = end;

			std::pair<Iterator, Iterator> Range1 = MergePartition(FirstHalfStart, FirstHalfEnd);
			std::pair<Iterator, Iterator> Range2 = MergePartition(SecondHalfStart, SecondHalfEnd);


			for (Iterator Cursor = Range1.first; Cursor < Range2.second + 1; Cursor++) { //+1
				auto LowestValueIterator = Cursor;
				for (Iterator SortCursor = Cursor; SortCursor < Range2.second + 1; SortCursor++) { //+1
					if (*LowestValueIterator > *SortCursor)
						LowestValueIterator = SortCursor;
				}

				if (LowestValueIterator == Cursor)
					continue;

				auto Temp = *Cursor;
				*Cursor = *LowestValueIterator;
				*LowestValueIterator = Temp;
			}

			return std::pair<Iterator, Iterator>(start, end);
		}
	}

	template<IsPointer Iterator>
	constexpr inline void MergeSort(Iterator start, Iterator end) noexcept {
		
		int64 RangeSize = end - start; 
		int64 SplitPoint = RangeSize / 2;

		Iterator FirstHalfStart = start;
		Iterator FirstHalfEnd = start + (SplitPoint - 1);

		Iterator SecondHalfStart = start + SplitPoint;
		Iterator SecondHalfEnd = end - 1;//Removing 1 that is the after end

		std::pair<Iterator, Iterator> Range1 = MergePartition(FirstHalfStart, FirstHalfEnd);
		std::pair<Iterator, Iterator> Range2 = MergePartition(SecondHalfStart, SecondHalfEnd);

		for (Iterator Cursor = Range1.first; Cursor < Range2.second + 1; Cursor++) { //+1
			auto LowestValueIterator = Cursor;
			for (Iterator SortCursor = Cursor + 1; SortCursor < Range2.second + 1; SortCursor++) { //+1
				if (*LowestValueIterator > *SortCursor)
					LowestValueIterator = SortCursor;
			}

			if (LowestValueIterator == Cursor)
				continue;

			auto Temp = *Cursor;
			*Cursor = *LowestValueIterator;
			*LowestValueIterator = Temp;
		}
	}



	template<IsPointer Iterator>
	constexpr inline void HeapSort(Iterator start, Iterator end) noexcept {


	}
}
#endif // !SORTING_H

