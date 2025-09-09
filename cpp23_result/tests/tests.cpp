#include <gtest/gtest.h>

#include <format>
#include <print>
#include "cpp23_result.h"
#include "result.h"

class result_test : public ::testing::Test
{
private:
    /* data */
public:
    result_test(/* args */) {}
    ~result_test() {}
};

TEST_F(result_test, test1)
{
	std::cout << "Hello CMake." << std::endl;

	result<int> r1{ 10 };
	std::println("r1: {}", r1.value());

	auto e1 = result<int>::with_error("e1");
	std::println("e1: {}", e1.error());
}
