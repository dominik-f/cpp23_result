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

//todo noexcept where possible
//todo inline where possible
//todo constexpr where possible
//todo concepts where possible
//todo add tests for T and E types, e.g. void, bool, non-copyable, non-moveable, non-default-constructible
//todo struct Ok
//todo struct Err


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

	constexpr explicit result(ok_type, const T& value) : state_{ state::Ok }, ok_{ value } {}
	constexpr explicit result(err_type, const E& error) : state_{ state::Error }, err_{ error } {}

	// https://www.heise.de/blog/C-Core-Guidelines-Der-noexcept-Spezifier-und-Operator-4121657.html
	// Mithilfe der Type-Traits-Bibliothek lässt sich zur Compilezeit prüfen,
	// ob ein Datentyp T einen Konstruktor besitzt, der keine Ausnahme werfen kann:
	// std::is_nothrow_copy_constructible::value.
	// Daher kann auch statt des noexcept-Operators das Prädikat aus der Type-Traits-Bibliothek verwendet werden:

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

	[[nodiscard]] constexpr inline bool is_ok() const { return state_ == state::Ok; }
	[[nodiscard]] constexpr inline bool is_err() const { return state_ == state::Error; }

	// explicit to avoid implicit conversion to bool
	// e.g. result<string, strint> r; int i = r; 
	// 
	// disabled if(result) confusion when T is bool
	// e.g. result<bool> res = ...; if(res) ...
	// use res.is_ok() or res.is_err() instead
	// todo delete if T is bool to avoid confusion
  	template <typename U = T, typename = std::enable_if<!std::is_same_v<U, bool>>>
	constexpr explicit operator bool() const noexcept {
		return is_ok();
	}


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
