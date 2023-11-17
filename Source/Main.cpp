#include <exception>
#include <iostream>

#include "DynamicArray.h"



int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	try {

		//DynamicArray<int> Array(12, 8);





		DynamicArray<int> Array;
		Array.Pushback(1);
		Array.Pushback(2);
		Array.Pushback(3);
		Array.Pushback(4);
		Array.Pushback(5);
		Array.Pushback(6);
		Array.Pushback(7);

		//DynamicArray<int> Array2;
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