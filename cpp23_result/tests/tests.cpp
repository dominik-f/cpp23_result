#include <gtest/gtest.h>

#include <format>
#include <print>
#include "cpp23_result.h"
#include "result.h"

using df::result;

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

TEST_F(result_test, ok)
{
	result<int> r = 10;
	ASSERT_TRUE(r.is_ok());
	EXPECT_EQ(r.value(), 10);
}
TEST_F(result_test, ok_ref_qualifiers)
{
	result<int> r = 10;
	const result<int> cr = 10;

	EXPECT_EQ(r.value(), 10);
	EXPECT_EQ(std::move(r.value()), 10);
	EXPECT_EQ(cr.value(), 10);
	EXPECT_EQ(std::move(cr.value()), 10);
}
TEST_F(result_test, err)
{
	auto r = result<int, int>::with_error(10);
	ASSERT_TRUE(r.is_err());
	EXPECT_EQ(r.error(), 10);
}
TEST_F(result_test, err_ref_qualifiers)
{
	auto r = result<int, int>::with_error(10);
	const auto cr = result<int, int>::with_error(10);

	EXPECT_EQ(r.error(), 10);
	EXPECT_EQ(std::move(r.error()), 10);
	EXPECT_EQ(cr.error(), 10);
	EXPECT_EQ(std::move(cr.error()), 10);
}
