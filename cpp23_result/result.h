#pragma once

#include <optional>
#include <string>
#include <type_traits>

template<typename T, typename E = std::string>
class result
{
	enum class Status
	{
		Ok, Error
	};
	const Status status_;
	std::optional<T> ok_;
	std::optional<E> err_;

	result(const E& error) : status_{ Status::Error }, err_{ error } {}

public:
	result(const T& value) : status_{ Status::Ok }, ok_{ value } {}

	~result() = default;

	static result<T, E> with_error(const E& error)
	{
		return result<T, E>(error);
	}

	bool is_ok() { return status_ == Status::Ok; }
	bool is_err() { return status_ == Status::Error; }

	auto expect(const char* msg) && ->T  requires !std::is_void_v<T> {
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

	// One version of value which works for everything
	template <class Self>
	constexpr auto&& error(this Self&& self) {
		if (self.is_err()) {
			return std::forward<Self>(self).err_.value();
		}
		throw std::runtime_error("invalid error access");
	}

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
	}

private:

};
