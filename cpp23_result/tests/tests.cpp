#include <gtest/gtest.h>

#include <format>
#include <print>
#include "result.h"
#include <expected>


class result_test : public ::testing::Test
{
private:
    /* data */
public:
    result_test(/* args */) {
		std::expected<int, std::string> e1 = 10;
		std::expected<int, std::string> e2 = std::unexpected("error");
	}
    ~result_test() {}
};

TEST_F(result_test, test1)
{
	std::cout << "Hello CMake." << std::endl;

	df::result<int, std::string> r1{ 10 };
	std::println("r1: {}", r1.value());

	auto e1 = df::result<int, std::string>::with_error<std::string>("e1");
	std::println("e1: {}", e1.error());
}

TEST_F(result_test, ok)
{
	df::result<int, std::string> r = 10;
	ASSERT_TRUE(r.is_ok());
	EXPECT_EQ(r.value(), 10);
}
TEST_F(result_test, ok_ref_qualifiers)
{
	df::result<int, std::string> r = 10;
	const df::result<int, std::string> cr = 10;

	EXPECT_EQ(r.value(), 10);
	EXPECT_EQ(std::move(r.value()), 10);
	EXPECT_EQ(cr.value(), 10);
	EXPECT_EQ(std::move(cr.value()), 10);
}
TEST_F(result_test, err)
{
	auto r = df::result<int, int>::with_error(10);
	ASSERT_TRUE(r.is_err());
	EXPECT_EQ(r.error(), 10);
}
TEST_F(result_test, err_ref_qualifiers)
{
	auto r = df::result<int, int>::with_error(10);
	const auto cr = df::result<int, int>::with_error(10);

	EXPECT_EQ(r.error(), 10);
	EXPECT_EQ(std::move(r.error()), 10);
	EXPECT_EQ(cr.error(), 10);
	EXPECT_EQ(std::move(cr.error()), 10);
}


TEST_F(result_test, BoolOperatorOk) {
  df::result<int, std::string> r(4);
  EXPECT_FALSE(r.is_err());
  EXPECT_TRUE(r.is_ok());
  EXPECT_TRUE(r);
}

TEST_F(result_test, BoolOperatorError) {
  auto r = df::result<int, std::string>::with_error<std::string>("asdf");
  EXPECT_TRUE(r.is_err());
  EXPECT_FALSE(r.is_ok());
  EXPECT_FALSE(r);
}

TEST_F(result_test, IsOk) {
  df::result<int, std::string> r(4);
  const df::result<int, std::string> cr(5);

  EXPECT_TRUE(r.is_ok());
  EXPECT_TRUE(cr.is_ok());
  EXPECT_TRUE(std::move(r).is_ok());
  EXPECT_TRUE(std::move(cr).is_ok());
}

TEST_F(result_test, IsError) {
  auto r = df::result<int, std::string>::with_error<std::string>("foo");
  const auto cr = df::result<int, std::string>::with_error<std::string>("foo");

  EXPECT_TRUE(r.is_err());
  EXPECT_TRUE(cr.is_err());
  EXPECT_TRUE(std::move(r).is_err());
  EXPECT_TRUE(std::move(cr).is_err());
}

TEST_F(result_test, VoidVariants) {
  df::result<int, int> r1 = 10;
  df::result<int, void> r2 = 10;
  df::result<void, int> r3;
  df::result<void, void> r4;

  auto e1 = df::result<int, int>::with_error(20);
  auto e2 = df::result<int, void>::with_error();
  auto e3 = df::result<void, int>::with_error(20);
  auto e4 = df::result<void, void>::with_error();
}

df::result<int, std::string> print_int(const int i) { std::cout << i << "\n"; return i; }
df::result<int, std::string> log_error(const std::string& err) { std::cout << err << "\n"; return df::result<int, std::string>::with_error(err); }
int add_ten(int v) { return v + 10; }

TEST_F(result_test, and_then_or_else) {
  df::result<int, std::string> r1 = 10;
  const df::result<int, std::string> cr2 = 20;
  auto e1 = df::result<int, std::string>::with_error("e30");
  const auto ce2 = df::result<int, std::string>::with_error("e40");

  // todo better tests
  r1.and_then(print_int);
  cr2.and_then(print_int);
  r1.or_else(log_error);
  cr2.or_else(log_error);

  e1.and_then(print_int);
  ce2.and_then(print_int);
  e1.or_else(log_error);
  ce2.or_else(log_error);
}

TEST_F(result_test, transform) {
  df::result<int, std::string> r1 = 10;
  const df::result<int, std::string> cr2 = 20;
  auto e1 = df::result<int, std::string>::with_error("e30");
  const auto ce2 = df::result<int, std::string>::with_error("e40");

  auto x = r1.transform_value(add_ten);
  EXPECT_EQ(x.value(), 20);
  EXPECT_EQ(r1.value(), 10);
}
