#pragma once

#include <type_traits>
#include <optional>
#include <string>
#include <type_traits>

namespace df
{
	
class bad_result_access : public std::exception
{
public:
	bad_result_access() = default;
	virtual ~bad_result_access() = default;

	const char* what() const noexcept override
	{ return "bad result access"; }
};


template<typename T, typename E = std::string>
class result
{
	struct ok_type : std::true_type {};
	struct err_type : std::false_type {};

	enum class state
	{
		Ok, Error
	};
	const state state_;
	std::optional<T> ok_;
	std::optional<E> err_;

	constexpr result(ok_type, const T& value) : state_{ state::Ok }, ok_{ value } {}
	constexpr result(err_type, const E& error) : state_{ state::Error }, err_{ error } {}

public:
	constexpr result(const T& value) : result(ok_type{}, value) {}

	~result() = default;

	/*
	 operator bool
	 has_value
	 value
	 value_or
	  and_then
	  or_else
	  transform
	  is_ok()
	  is_err()

	  static with_ok
	  static with_err

	  throw when moved

	  inplace init

	  T, E: contructor tests, when types can be
	  - default initialized (or not)
	  - move/copy constructed (or not)

	  delete bool operator if T is bool to stop confusion
	*/

	static result<T, E> with_error(const E& error)
	{
		return result<T, E>(err_type{}, error);
	}

	constexpr bool is_ok() const { return state_ == state::Ok; }
	constexpr bool is_err() const { return state_ == state::Error; }

	auto expect(const char* msg) && ->T  requires (!std::is_void_v<T>) {
		if (is_ok())
		{
			return std::move(value());
		}
		else
		{
			throw std::runtime_error(msg);
		}
	}

	constexpr T value_or(T&& default_value) const& {
		if (is_ok())
		{
			return std::move(value());
		}
		else
		{
			throw default_value;
		}
	}

	//T value() {
	//	if (is_ok()) return *ok_;
	//	else throw std::runtime_error("invalid ok access");
	//}
	//E error() {
	//	if (is_err()) return *err_;
	//	else throw std::runtime_error("invalid error access");
	//}

	template <class Self>
	constexpr auto&& value(this Self&& self) {
		if (self.is_ok()) {
			return std::forward<Self>(self).ok_.value();
		}
		throw std::runtime_error("invalid ok access");
	}

	template <class Self>
	constexpr auto&& error(this Self&& self) {
		if (self.is_err()) {
			return std::forward<Self>(self).err_.value();
		}
		throw std::runtime_error("invalid error access");
	}
/*
	// version of value for non-const lvalues
	constexpr T& value()& {
		if (is_ok()) {
			return *ok_;
		}
		throw std::runtime_error("invalid ok access");
	}

	// version of value for const lvalues
	constexpr T const& value() const& {
		if (is_ok()) {
			return *ok_;
		}
		throw std::runtime_error("invalid ok access");
	}

	// version of value for non-const rvalues... are you bored yet?
	constexpr T&& value()&& {
		if (is_ok()) {
			return std::move(*ok_);
		}
		throw std::runtime_error("invalid ok access");
	}

	// you sure are by this point
	constexpr T const&& value() const&& {
		if (is_ok()) {
			return std::move(*ok_);
		}
		throw std::runtime_error("invalid ok access");
	}*/
/*

- transform



/// TransformError &
  template <class Func>
  constexpr auto TransformError(Func&& func)



  // do not make explicit
  constexpr Result(const Ok<T>& ok) {
  constexpr Result(const Err<E>& err) {

// throw if error
constexpr inline void Expect(const std::string& str) const



and_then

or_else



*/

private:

};

}
