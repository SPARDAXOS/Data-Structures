#include <exception>
#include <iostream>

#include "DynamicArray.h"


class Test {
public:
	Test() {
		std::cout << "ctor" << std::endl;
	}
	~Test() {
		std::cout << "dtor" << std::endl;
	}

	long long number1;
	long long number2;
	long long number3;
	long long number4;
};
int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	try {

		//DynamicArray<int> Array(12, 8);





		DynamicArray<int> Array;
		DynamicArray<bool> Array2;
		DynamicArray<Test> Array3;

		Array.

		Array.Reserve(12);
		Array.Pushback(1);
		Array.Pushback(2);
		Array3.Pushback({});
		Array3.Pushback({});
		Array3.PopBack();
		Array3.PopBack();
		//Array.Pushback(2);
		//Array.Pushback(3);
		//Array.Pushback(4);
		//Array.Pushback(5);
		//Array.Pushback(6);
		//Array.Pushback(7);

		Array.PopBack();
		Array.PopBack();
		Array.PopBack();
		Array.PopBack();
		Array.PopBack();
		Array.PopBack();
		Array.PopBack();
		Array.PopBack();

		//DynamicArray<int> Array2 = std::move(Array);
		//Array2.Pushback(8);
		//Array2.Pushback(9);
		//Array2.Pushback(10);
		//Array2.Pushback(11);
		//Array2.Pushback(12);
		//Array2.Pushback(13);
		//Array2.Pushback(14);

		//Array = Array2;
		std::cout << Array.Front() << std::endl;
		std::cout << Array.Back() << std::endl;
		//std::cout << Array.At(1) << std::endl;

	}
	catch (const std::exception& exception) {
		std::cerr << exception.what() << std::endl;
		return 1;
	}

	return 0;
}