#ifndef DF_RESULT_H
#define DF_RESULT_H

#include <functional>
#include <type_traits>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include <exception>
#include <stdexcept>

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

namespace detail
{
	template<typename T, typename E>
	struct result_storage
	{
		std::variant<T, E> value;
	};
	template<typename T>
	struct result_storage<T, void>
	{
		std::variant<T, std::monostate> value;
	};
	template<typename E>
	struct result_storage<void, E>
	{
		std::variant<std::monostate, E> value;
	};
	template<>
	struct result_storage<void, void>
	{
		std::variant<std::monostate, std::monostate> value;
	};
	
} // namespace detail


//todo noexcept where possible
//todo inline where possible
//todo constexpr where possible
//todo concepts where possible
//todo add tests for T and E types, e.g. void, bool, non-copyable, non-moveable, non-default-constructible
//todo struct Ok
//todo struct Err

template<class T>
concept IsVoid = std::is_void_v<T>;
template<class T>
concept NotVoid = !std::is_void_v<T>;
template<class T>
concept IsBool = std::is_same_v<T, bool>;
template<class T>
concept NotBool = !std::is_same_v<T, bool>;


template<typename T, typename E>
class result
{
	static constexpr int OK_STATE = 0;
	static constexpr int ERR_STATE = 1;

	struct ok_type : std::true_type {};
	struct err_type : std::false_type {};

	detail::result_storage<T, E> storage_;

	/*enum class state
	{
		Ok, Error
	};
	const state state_;*/

	template <IsVoid _T = T>
	constexpr explicit result(ok_type) : storage_{ .value {std::in_place_index<0>, std::monostate{} } } {}
	template <NotVoid _T = T>
	constexpr explicit result(ok_type, const _T& value) : storage_{ .value {std::in_place_index<0>, value} } {}

  	template <typename _E = E, typename = std::enable_if<std::is_void_v<_E>>>
	constexpr explicit result(err_type) : storage_{ .value { std::in_place_index<1>, std::monostate{} } } {}
  	template <typename _E = E, typename = std::enable_if<!std::is_void_v<_E>>>
	constexpr explicit result(err_type, const _E& error) : storage_{ .value { std::in_place_index<1>, error } } {}

	// https://www.heise.de/blog/C-Core-Guidelines-Der-noexcept-Spezifier-und-Operator-4121657.html
	// Mithilfe der Type-Traits-Bibliothek lässt sich zur Compilezeit prüfen,
	// ob ein Datentyp T einen Konstruktor besitzt, der keine Ausnahme werfen kann:
	// std::is_nothrow_copy_constructible::value.
	// Daher kann auch statt des noexcept-Operators das Prädikat aus der Type-Traits-Bibliothek verwendet werden:

public:
	//using ok_type = T;
	//using err_type = E;

	template <typename _T = T> requires(std::is_void_v<_T>)
	constexpr result() : result(ok_type{}) {}
	template <typename _T = T> requires(!std::is_void_v<_T>)
	constexpr result(const _T& value) : result(ok_type{}, value) {}

	virtual ~result() = default;

	/*
	ok_t, err_t
	explicit result constructors for ok_t, err_t (usable for returns)

	 operator bool
	 has_value
	 value
	 value_or
	 error
	 error_or

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


	  https://en.cppreference.com/w/cpp/utility/expected.html
	emplace - constructs the expected value in-place
	swap - exchanges the contents

	// operator==

	*/

	template <IsVoid _E = E>
	static result<T, E> with_error()
	{
		return result<T, E>(err_type{});
	}
	template <NotVoid _E = E>
	static result<T, E> with_error(const _E& error)
	{
		return result<T, E>(err_type{}, error);
	}

	[[nodiscard]] constexpr inline bool is_ok() const { return storage_.value.index() == OK_STATE; }
	[[nodiscard]] constexpr inline bool is_err() const { return storage_.value.index() == ERR_STATE; }

	// explicit to avoid implicit conversion to bool
	// e.g. result<string, strint> r; int i = r; 
	// 
	// disabled if(result) confusion when T is bool
	// e.g. result<bool> res = ...; if(res) ...
	// use res.is_ok() or res.is_err() instead
	// todo delete if T is bool to avoid confusion
  	template <typename U = T> requires(NotBool<U>)
	constexpr explicit operator bool() const noexcept {
		return is_ok();
	}

/*
	auto expect(const char* msg) && ->T  requires (!std::is_void_v<T>) {
		if (is_ok())
		{
			return std::move(value());
		}
		else
		{
			throw std::runtime_error(msg);
		}
	}*/

	template<NotVoid _T = T>
	constexpr T value_or(_T&& default_value) const& {
		if (is_ok())
		{
			return value();
		}
		else
		{
			return default_value;
		}
	}
	template<NotVoid _T = T>
	constexpr T value_or(_T&& default_value) && {
		if (is_ok())
		{
			return std::move(value());
		}
		else
		{
			return default_value;
		}
	}


	template <class Self>
	constexpr auto&& value(this Self&& self) {
		if (self.is_ok()) {
			return std::get<0>(std::forward<Self>(self).storage_.value);
		}
		throw std::runtime_error("invalid ok access");
	}

	template <class Self>
	constexpr auto&& error(this Self&& self) requires(!std::is_void_v<E>) {
		if (self.is_err()) {
			return std::get<1>(std::forward<Self>(self).storage_.value);
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

#endif // DF_RESULT_H
