#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

namespace steamkit
{
template <typename T>
class Result
{
public:
    static Result success(T value)
    {
        return Result(std::move(value), {});
    }

    static Result failure(std::string error)
    {
        return Result(std::nullopt, std::move(error));
    }

    [[nodiscard]] bool ok() const noexcept
    {
        return value_.has_value();
    }

    [[nodiscard]] const T& value() const
    {
        if (!value_)
        {
            throw std::logic_error("Result has no value: " + error_);
        }
        return *value_;
    }

    [[nodiscard]] T& value()
    {
        if (!value_)
        {
            throw std::logic_error("Result has no value: " + error_);
        }
        return *value_;
    }

    [[nodiscard]] const std::string& error() const noexcept
    {
        return error_;
    }

private:
    Result(std::optional<T> value, std::string error)
        : value_(std::move(value)), error_(std::move(error))
    {
    }

    std::optional<T> value_;
    std::string error_;
};
}
