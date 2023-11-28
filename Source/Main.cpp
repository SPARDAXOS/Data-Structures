#include <exception>
#include <iostream>

#include "Container.h"
#include "Profiler.h"
#include "Sorting.h"


class Test {
public:
	Test() {
		//std::cout << "Default ctor" << std::endl;
	}
	Test(int arg) 
		: number1(arg), number2(arg), number3(arg), number4(arg)
	{
		//std::cout << "Custom ctor " << arg << std::endl;

	}
	~Test() {
		//std::cout << "Dtor" << std::endl;
	}

	Test(const Test& other) {
		*this = other;
	}
	Test& operator=(const Test& other) noexcept {
		this->number1 = other.number1;
		this->number2 = other.number2;
		this->number3 = other.number3;
		this->number4 = other.number4;

		//std::cout << "Copy" << std::endl;
		return *this;
	}

	Test(Test&& other) noexcept {
		*this = std::move(other);
	}
	Test& operator=(Test&& other) noexcept {

		this->number1 = other.number1;
		this->number2 = other.number2;
		this->number3 = other.number3;
		this->number4 = other.number4;

		other.number1 = 0;
		other.number2 = 0;
		other.number3 = 0;
		other.number4 = 0;

		//std::cout << "Move" << std::endl;
		return *this;
	}

public:
	int number1 = 1;
	int number2 = 2;
	int number3 = 3;
	int number4 = 4;
};
int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	try {
		Container<int> Array;
		Array.push_back(9);
		Array.push_back(10);
		Array.push_back(7);
		Array.push_back(1);
		Array.push_back(4);
		Array.push_back(3);
		Array.push_back(2);
		Array.push_back(6);
		Array.push_back(5);
		Array.push_back(8);
		Sorting::InsertionSort(Array.begin(), Array.end());

		std::vector<Test> vector;

		Profiler MainProfiler;

		//DynamicArray<Test> Array2;

		//Array.Emplaceback(1);
		//Array.Emplaceback(2);
		//Array.Emplaceback(3);
		//Array.Emplaceback(4);




		//STD VS CONTAINER
		//MainProfiler.StartProfile("TestProfile");
		//for (uint16 i = 0; i < 50000; i++)
		//	vector.emplace_back(i);
		//
		//auto results = MainProfiler.EndProfile("TestProfile");
		//std::cout << "Vector took " << results->AsMicroseconds() << " Mircoseconds" << std::endl;
		//std::cout << "Vector took " << results->AsMilliseconds() << " Milliseconds" << std::endl;
		//std::cout << "Vector took " << results->AsSeconds() << " Seconds" << std::endl;
		//
		//
		//
		//MainProfiler.StartProfile("TestProfile2");
		//for (uint16 i = 0; i < 50000; i++)
		//	Array.emplace_back(i);
		//
		//auto results2 = MainProfiler.EndProfile("TestProfile2");
		//std::cout << "My container took " << results2->AsMicroseconds() << " Mircoseconds" << std::endl;
		//std::cout << "My container took " << results2->AsMilliseconds() << " Milliseconds" << std::endl;
		//std::cout << "My container took " << results2->AsSeconds() << " Seconds" << std::endl;


		//Array.Pushback(4);
		//Array.Pushback(3);
		//Array.Pushback(2);
		//Array.Pushback(1);
		//Container<Test> Array2;
		//Array2 = std::move(Array);
		//Array2.Swap(Array);
	}
	catch (const std::exception& exception) {
		std::cerr << exception.what() << std::endl;
		return 1;
	}

	return 0;
}