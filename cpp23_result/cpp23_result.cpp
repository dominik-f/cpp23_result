// cpp23_result.cpp : Defines the entry point for the application.
//

#include <format>
#include <print>
#include "cpp23_result.h"
#include "result.h"

int main()
{
	std::cout << "Hello CMake." << std::endl;

	result<int> r1{ 10 };
	std::println("r1: {}", r1.value());

	auto e1 = result<int>::with_error("e1");
	std::println("e1: {}", e1.error());

	return 0;
}
