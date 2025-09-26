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

auto print_nothing() { std::cout << "nothing\n"; return df::result<void, std::string>(); }
auto print_int(const int i) { std::cout << i << "\n"; return df::result<int, std::string>(i); }
auto print_int_cs(const int i) { std::cout << i << "\n"; return df::result<int, const std::string>(i); }
auto print_int_v(const int i) { std::cout << i << "\n"; return df::result<int, void>(i); }
template<typename T, typename E>
auto log_error(const E& err) { std::cout << err << "\n"; return df::result<T, E>::with_error(err); }
template<typename T>
auto log_error_void() { std::cout << "error\n"; return df::result<T, void>::with_error(); }

int add_ten(int v) { return v + 10; }


TEST_F(result_test, and_then_or_else) {
  df::result<int, std::string> r1_i = 10;
  df::result<const int, std::string> r2_ci = 10;
  df::result<void, std::string> r3_v;
  const df::result<int, std::string> cr4_i = 20;
  const df::result<const int, std::string> cr5_ci = 20;
  const df::result<void, std::string> cr6_v;
  
  auto er1_s = df::result<int, std::string>::with_error("e30");
  auto er2_cs = df::result<int, const std::string>::with_error("e40");
  auto er3_v = df::result<int, void>::with_error();
  const auto er4_s = df::result<int, std::string>::with_error("e30");
  const auto er5_cs = df::result<int, const std::string>::with_error("e40");
  const auto er6_v = df::result<int, void>::with_error();

  // todo better tests
  r1_i.and_then(print_int);
  r2_ci.and_then(print_int);
  r3_v.and_then(print_nothing);
  cr4_i.and_then(print_int);
  cr5_ci.and_then(print_int);
  cr6_v.and_then(print_nothing);

  er1_s.and_then(print_int);
  er2_cs.and_then(print_int_cs);
  er3_v.and_then(print_int_v);
  er4_s.and_then(print_int);
  er5_cs.and_then(print_int_cs);
  er6_v.and_then(print_int_v);

  r1_i.or_else(log_error<int, std::string>);
  r2_ci.or_else(log_error<const int, std::string>);
  r3_v.or_else(log_error<void, std::string>);
  cr4_i.or_else(log_error<int, std::string>);
  cr5_ci.or_else(log_error<const int, std::string>);
  cr6_v.or_else(log_error<void, std::string>);

  er1_s.or_else(log_error<int, std::string>);
  er2_cs.or_else(log_error<int, const std::string>);
  er3_v.or_else(log_error_void<int>);
  er4_s.or_else(log_error<int, std::string>);
  er5_cs.or_else(log_error<int, const std::string>);
  er6_v.or_else(log_error_void<int>);
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
