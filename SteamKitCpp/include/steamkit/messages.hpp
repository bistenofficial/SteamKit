#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

#include <steamkit/enums.hpp>

namespace steamkit
{
namespace detail
{
inline void append_le32(std::vector<std::byte>& output, std::uint32_t value)
{
    output.push_back(static_cast<std::byte>(value & 0xFFU));
    output.push_back(static_cast<std::byte>((value >> 8U) & 0xFFU));
    output.push_back(static_cast<std::byte>((value >> 16U) & 0xFFU));
    output.push_back(static_cast<std::byte>((value >> 24U) & 0xFFU));
}
}

template <typename TBody>
class ClientMsg
{
public:
    ClientMsg(EMsg msg, TBody body = TBody{})
        : msg_(msg), body_(std::move(body))
    {
    }

    [[nodiscard]] EMsg message_type() const noexcept
    {
        return msg_;
    }

    [[nodiscard]] const TBody& body() const noexcept
    {
        return body_;
    }

    [[nodiscard]] TBody& body() noexcept
    {
        return body_;
    }

    [[nodiscard]] std::vector<std::byte> serialize() const
    {
        std::vector<std::byte> output;
        detail::append_le32(output, static_cast<std::uint32_t>(msg_));

        auto body_data = body_.serialize();
        output.insert(output.end(), body_data.begin(), body_data.end());
        return output;
    }

private:
    EMsg msg_;
    TBody body_;
};

template <typename TBody>
class ClientGCMsg
{
public:
    ClientGCMsg(std::uint32_t msg, TBody body = TBody{})
        : msg_(msg), body_(std::move(body))
    {
    }

    [[nodiscard]] std::uint32_t message_type() const noexcept
    {
        return msg_;
    }

    [[nodiscard]] const TBody& body() const noexcept
    {
        return body_;
    }

    [[nodiscard]] TBody& body() noexcept
    {
        return body_;
    }

    [[nodiscard]] std::vector<std::byte> serialize() const
    {
        std::vector<std::byte> output;
        detail::append_le32(output, msg_);

        auto body_data = body_.serialize();
        output.insert(output.end(), body_data.begin(), body_data.end());
        return output;
    }

private:
    std::uint32_t msg_;
    TBody body_;
};
}
