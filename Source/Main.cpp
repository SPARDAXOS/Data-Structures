#include <exception>
#include <iostream>

#include "DynamicArray.h"



int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	try {

		DynamicArray<int> Array;
		Array.Pushback(1);
		Array.Pushback(2);
		Array.Pushback(3);
		Array.Pushback(4);
		Array.Pushback(5);
		Array.Pushback(6);
		Array.Pushback(7);

	}
	catch (const std::exception& exception) {
		std::cerr << exception.what() << std::endl;
		return 1;
	}

	return 0;
}