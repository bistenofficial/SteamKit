#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <utility>

namespace steamkit
{
enum class ProtocolTypes : std::uint32_t
{
    None = 0,
    Tcp = 1 << 0,
    Udp = 1 << 1,
    WebSocket = 1 << 2,
    All = Tcp | Udp | WebSocket,
};

constexpr ProtocolTypes operator|(ProtocolTypes lhs, ProtocolTypes rhs) noexcept
{
    return static_cast<ProtocolTypes>(static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs));
}

constexpr bool has_flag(ProtocolTypes value, ProtocolTypes flag) noexcept
{
    return (static_cast<std::uint32_t>(value) & static_cast<std::uint32_t>(flag)) != 0;
}

struct Configuration
{
    ProtocolTypes protocols = ProtocolTypes::Tcp | ProtocolTypes::WebSocket;
    std::chrono::milliseconds connection_timeout = std::chrono::seconds(10);
    std::string web_api_base_address = "https://api.steampowered.com/";
};
}
