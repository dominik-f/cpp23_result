#ifndef DF_RESULT_H
#define DF_RESULT_H

#include <functional>
#include <type_traits>
#include <optional>
#include <string>
#include <concepts>
#include <type_traits>
#include <variant>
#include <exception>
#include <stdexcept>

namespace df
{

/// Discriminated union that holds an expected value or an error value.
template<typename T, typename E>
class result;


class bad_result_access : public std::exception
{
public:
	bad_result_access() = default;
	virtual ~bad_result_access() = default;

	const char* what() const noexcept override
	{ return "bad result access"; }
};

//todo implement storage as union with std::construct_at/std:destroy_at
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



template<typename T>
concept IsVoid = std::is_void_v<T>;
template<typename T>
concept NotVoid = !std::is_void_v<T>;
template<typename T>
concept IsBool = std::is_same_v<T, bool>;
template<typename T>
concept NotBool = !std::is_same_v<T, bool>;


namespace detail {
	template <typename T>
	constexpr bool is_result = false;
	template <typename T, typename E>
	constexpr bool is_result<result<T, E>> = true;
} // namespace detail

template<typename T, typename E>
class result
{
	static constexpr int OK_STATE = 0;
	static constexpr int ERR_STATE = 1;

	struct ok_tag : std::true_type {};
	struct err_tag : std::false_type {};

	detail::result_storage<T, E> storage_;

	/*enum class state
	{
		Ok, Error
	};
	const state state_;*/

	template <IsVoid _T = T>
	constexpr explicit result(ok_tag) : storage_{ .value {std::in_place_index<0>, std::monostate{} } } {}
	template <NotVoid _T = T>
	constexpr explicit result(ok_tag, const _T& value) : storage_{ .value {std::in_place_index<0>, value} } {}

  	template <typename _E = E, typename = std::enable_if<std::is_void_v<_E>>>
	constexpr explicit result(err_tag) : storage_{ .value { std::in_place_index<1>, std::monostate{} } } {}
  	template <typename _E = E, typename = std::enable_if<!std::is_void_v<_E>>>
	constexpr explicit result(err_tag, const _E& error) : storage_{ .value { std::in_place_index<1>, error } } {}

	// https://www.heise.de/blog/C-Core-Guidelines-Der-noexcept-Spezifier-und-Operator-4121657.html
	// Mithilfe der Type-Traits-Bibliothek lässt sich zur Compilezeit prüfen,
	// ob ein Datentyp T einen Konstruktor besitzt, der keine Ausnahme werfen kann:
	// std::is_nothrow_copy_constructible::value.
	// Daher kann auch statt des noexcept-Operators das Prädikat aus der Type-Traits-Bibliothek verwendet werden:

public:
	using value_type = T;
	using error_type = E;

	template <typename _T = T> requires(std::is_void_v<_T>)
	constexpr result() : result(ok_tag{}) {}
	template <typename _T = T> requires(!std::is_void_v<_T>)
	constexpr result(const _T& value) : result(ok_tag{}, value) {}

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
		return result<T, E>(err_tag{});
	}
	template <NotVoid _E = E>
	static result<T, E> with_error(const _E& error)
	{
		return result<T, E>(err_tag{}, error);
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

	constexpr T value_or(NotVoid auto&& default_value) const& {
		if (is_ok())
		{
			return value();
		}
		else
		{
			return default_value;
		}
	}
	constexpr T value_or(NotVoid auto&& default_value) && {
		if (is_ok())
		{
			return std::move(value());
		}
		else
		{
			return default_value;
		}
	}


	template <typename Self>
	constexpr auto&& value(this Self&& self) requires(!std::is_void_v<T>) {
		if (self.is_ok()) {
			return std::get<0>(std::forward<Self>(self).storage_.value);
		}
		throw std::runtime_error("invalid ok access");
	}

	template <typename Self>
	constexpr auto&& error(this Self&& self) requires(!std::is_void_v<E>) {
		if (self.is_err()) {
			return std::get<1>(std::forward<Self>(self).storage_.value);
		}
		throw std::runtime_error("invalid error access");
	}

	// todo check for is_contructible

	/// @brief If this.is_ok() returns the invocation result of the callable func. Otherwise returns the current error of this.
	/// The callable func has to return a result.
	/// But the callable can change the type:  result<T,E> -> result<U,E>
	/// Overload for T != void
	/// @param func callable - must return a result
	/// @return An object of result<U, E>
	template<typename Func>
		requires (!std::is_void_v<T>) && std::is_invocable_v<Func, T>
	constexpr auto and_then(this auto&& self, Func&& func)
	{
		using TValue = decltype(std::forward<decltype(self)>(self).value());
		using TResultOut = std::remove_cv_t<std::invoke_result_t<Func, TValue>>;
		static_assert(detail::is_result<TResultOut>, "The return value of func(value()) must be a specialization of result");
		static_assert(std::is_same_v<typename TResultOut::error_type, E>, "The return value of func(value()) must have the same error_type as this object");
		
		if (std::forward<decltype(self)>(self).is_ok())
		{
			return std::invoke(std::forward<Func>(func), std::forward<decltype(self)>(self).value());
		}
		else
		{
			if constexpr (std::is_void_v<E>)
			{
				return TResultOut(err_tag{});
			}
			else
			{
				return TResultOut(err_tag{}, std::forward<decltype(self)>(self).error());
			}
		}
	}
	/// @brief If this.is_ok() returns the invocation result of the callable func. Otherwise returns the current error of this.
	/// The callable func has to return a result.
	/// But the callable can change the type:  result<T,E> -> result<U,E>
	/// Overload for T == void
	/// @param func callable - must return a result
	/// @return An object of TResultOut
	template<typename Func>
		requires std::is_void_v<T> && std::is_invocable_v<Func>
	constexpr auto and_then(this auto&& self, Func&& func)
	{
		using TResultOut = std::remove_cv_t<std::invoke_result_t<Func>>;
    	static_assert(detail::is_result<TResultOut>, "The return value of func() must be a specialization of result");
		static_assert(std::is_same_v<typename TResultOut::error_type, E>, "The return value of func() must have the same error_type as this object");

		if (std::forward<decltype(self)>(self).is_ok())
		{
			return std::invoke(std::forward<Func>(func));
		}
		else
		{
			if constexpr (std::is_void_v<E>)
			{
				return TResultOut(err_tag{});
			}
			else
			{
				return TResultOut(err_tag{}, std::forward<decltype(self)>(self).error());
			}
		}
	}
	
	/// @brief If this.is_err() returns the invocation result of the callable func. Otherwise returns the current value of this.
	/// The callable func has to return a result.
	/// But the callable can change the type: result<T,E> -> result<T,R>
	/// @param func callable - must return a result
	/// @return An object of result<T, R>
	template<typename Func> requires (!std::is_void_v<E>) && std::invocable<Func, E>
	constexpr auto or_else(Func&& func) const
	{
		using TResultOut = std::remove_cv_t<std::invoke_result_t<Func, decltype((error())) >>;
    	//todo: static_assert(__is_std_expected<_Up>::error, "The result of f(error()) must be a specialization of result");
		static_assert(std::is_same_v<typename TResultOut::value_type, T>, "The result of func(error()) must have the same value_type as this result");

		if (is_err())
		{
			return std::invoke(std::forward<Func>(func), error());
		}
		else
		{
			if constexpr (std::is_void_v<T>)
			{
				return TResultOut(ok_tag{});
			}
			else
			{
				return TResultOut(ok_tag{}, value());
			}
		}
	}
	template<typename Func> requires std::is_void_v<E> && std::invocable<Func>
	constexpr auto or_else(Func&& func) const
	{
		using TResultOut = std::remove_cv_t<std::invoke_result_t<Func>>;
    	//todo: static_assert(__is_std_expected<_Up>::error, "The result of f(error()) must be a specialization of result");
		static_assert(std::is_same_v<typename TResultOut::value_type, T>, "The result of func(error()) must have the same value_type as this result");

		if (is_err())
		{
			return std::invoke(std::forward<Func>(func));
		}
		else
		{
			if constexpr (std::is_void_v<T>)
			{
				return TResultOut(ok_tag{});
			}
			else
			{
				return TResultOut(ok_tag{}, value());
			}
		}
	}
	
	/// @brief If this.is_ok() returns the invocation result of the callable func. Otherwise returns the current error of this.
	/// The callable can return any type.
	/// Returns a new result object.
	/// @param func callable - can return any type
	/// @return Returns an result<TRet, E> where TRet is the return type of func.
	template <typename Func> requires (!std::is_void_v<T>) && std::invocable<Func, T>
	constexpr auto transform_value(Func&& func)
	{
		using TRet = std::remove_cv_t<std::invoke_result_t<Func, decltype((value())) >>;
		//using TRet = std::remove_cv_t<std::invoke_result_t<Func, T&>>;
		using TResultOut = result<TRet, E>;
		
		if (is_ok())
		{
			if constexpr (std::is_void_v<T>)
			{
				return result<TRet, E>(std::invoke(std::forward<Func>(func)));
			}
			else
			{
				return result<TRet, E>(std::invoke(std::forward<Func>(func), value()));
			}
		}
		else
		{
			if constexpr (std::is_void_v<E>)
			{
				return result<TRet, E>(err_tag{});
			}
			else
			{
				return result<TRet, E>(err_tag{}, error());
			}
		}
	}

	template <typename Func> requires std::invocable<Func, E>
	constexpr auto transform_error(Func&& func)
	{
		using EOut = std::remove_cv_t<std::invoke_result_t<Func, decltype((error())) >>;
		//using TRet = std::remove_cv_t<std::invoke_result_t<Func, T&>>;
		using TResultOut = result<T, EOut>;
		
		if (is_err())
		{
			if constexpr (std::is_void_v<E>)
			{
				return TResultOut::with_error(std::invoke(std::forward<Func>(func)));
			}
			else
			{
				return TResultOut::with_error(std::invoke(std::forward<Func>(func), error()));
			}
		}
		else
		{
			if constexpr (std::is_void_v<T>)
			{
				return TResultOut(ok_tag{});
			}
			else
			{
				return TResultOut(ok_tag{}, value());
			}
		}
	}

/*

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
