#include <exception>
#include <iostream>

#include "Container.h"
#include "Profiler.h"
#include "Sorting.h"
#include "Graph.h"


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
		this->number1 = 0;
		this->number2 = 0;
		this->number3 = 0;
		this->number4 = 0;
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
		Profiler MainProfiler;

		Container<int> Array1;
		Array1.push_back(8);
		Array1.push_back(1);
		Array1.push_back(4);
		Array1.push_back(6);
		Array1.push_back(2);
		Array1.push_back(5);
		Array1.push_back(3);
		Array1.push_back(9);
		Array1.push_back(10);
		Array1.push_back(7);
		Sorting::QuickSort(Array1.begin(), Array1.end());


		//Move Semantics Test
		//Container<int> Array2(std::move(Array), Array.get_allocator());
		//Array2.push_back(8);
		//Array2.push_back(1);
		//Array2.push_back(4);
		//Array2.push_back(6);
		//Array2.push_back(2);
		//Array2 = std::move(Array); //STD::STATE_TRUE WORKS! TEST OTHER 2
		//Array.erase(Array.begin() + 1, Array.end()); //Needs testing 
		//Array.clear();






		//STD VS CONTAINER
		std::vector<Test> Vector;
		Container<Test> Container2;
		MainProfiler.StartProfile("TestProfile");
		for (uint16 i = 0; i < 50000; i++)
			Vector.emplace_back(i);
		
		auto results = MainProfiler.EndProfile("TestProfile");
		std::cout << "Vector took " << results->AsMicroseconds() << " (Mircoseconds)" << std::endl;
		std::cout << "Vector took " << results->AsMilliseconds() << " (Milliseconds)" << std::endl;
		std::cout << "Vector took " << results->AsSeconds() << " (Seconds) \n" << std::endl;
		
		
		MainProfiler.StartProfile("TestProfile2");
		for (uint16 i = 0; i < 50000; i++)
			Container2.emplace_back(i);
		
		auto results2 = MainProfiler.EndProfile("TestProfile2");
		std::cout << "Container took " << results2->AsMicroseconds() << " (Mircoseconds)" << std::endl;
		std::cout << "Container took " << results2->AsMilliseconds() << " (Milliseconds)" << std::endl;
		std::cout << "Container took " << results2->AsSeconds() << " (Seconds)" << std::endl;
	}
	catch (const std::exception& exception) {
		std::cerr << exception.what() << std::endl;
		return 1;
	}

	return 0;
}