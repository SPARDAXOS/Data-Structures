#include <exception>
#include <iostream>

#include "Container.h"
#include "Profiler.h"


class Test {
public:
	Test() {
		std::cout << "Default ctor" << std::endl;
	}
	Test(int arg) 
		: number1(arg), number2(arg), number3(arg), number4(arg)
	{
		std::cout << "Custom ctor " << arg << std::endl;

	}
	~Test() {
		std::cout << "Dtor" << std::endl;
	}

	Test(const Test& other) {
		*this = other;
	}
	Test& operator=(const Test& other) noexcept {
		this->number1 = other.number1;
		this->number2 = other.number2;
		this->number3 = other.number3;
		this->number4 = other.number4;

		std::cout << "Copy" << std::endl;
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

		std::cout << "Move" << std::endl;
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
		Container<Test> Array;
		Profiler MainProfiler;

		//DynamicArray<Test> Array2;

		//Array.Emplaceback(1);
		//Array.Emplaceback(2);
		//Array.Emplaceback(3);
		//Array.Emplaceback(4);

		MainProfiler.StartProfile("TestProfile");
		Array.Pushback(4);
		Array.Emplaceback(3);
		Array.Pushback(2);
		Array.Emplaceback(1);
		Container<Test> Array2;
		Array2 = std::move(Array);
		auto results = MainProfiler.EndProfile("TestProfile");
		std::cout << "It took " << results->AsMicroseconds() << " Mircoseconds" << std::endl;
		std::cout << "It took " << results->AsMilliseconds() << " Milliseconds" << std::endl;
		std::cout << "It took " << results->AsSeconds() << " Seconds" << std::endl;
		//Array2.Swap(Array);
	}
	catch (const std::exception& exception) {
		std::cerr << exception.what() << std::endl;
		return 1;
	}

	return 0;
}