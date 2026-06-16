#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

#include <steamkit/enums.hpp>

namespace steamkit
{
class JobID
{
public:
    constexpr JobID() = default;
    constexpr explicit JobID(std::uint64_t value) noexcept
        : value_(value)
    {
    }

    [[nodiscard]] static constexpr JobID invalid() noexcept
    {
        return JobID();
    }

    [[nodiscard]] constexpr std::uint64_t value() const noexcept
    {
        return value_;
    }

    [[nodiscard]] constexpr bool is_valid() const noexcept
    {
        return value_ != 0;
    }

    friend constexpr bool operator==(JobID lhs, JobID rhs) noexcept = default;

private:
    std::uint64_t value_ = 0;
};

class SteamID
{
public:
    static constexpr std::uint32_t AllInstances = 0;
    static constexpr std::uint32_t DesktopInstance = 1;
    static constexpr std::uint32_t ConsoleInstance = 2;
    static constexpr std::uint32_t WebInstance = 4;
    static constexpr std::uint32_t AccountIDMask = 0xFFFFFFFF;
    static constexpr std::uint32_t AccountInstanceMask = 0x000FFFFF;
    static constexpr std::uint32_t ChatInstanceClan = (AccountInstanceMask + 1) >> 1;
    static constexpr std::uint32_t ChatInstanceLobby = (AccountInstanceMask + 1) >> 2;
    static constexpr std::uint32_t ChatInstanceMMSLobby = (AccountInstanceMask + 1) >> 3;

    SteamID() = default;
    explicit SteamID(std::uint64_t id) noexcept;
    explicit SteamID(std::string_view steam_id);
    SteamID(std::uint32_t account_id, EUniverse universe, EAccountType account_type) noexcept;
    SteamID(std::uint32_t account_id, std::uint32_t instance, EUniverse universe, EAccountType account_type) noexcept;

    void set(std::uint32_t account_id, EUniverse universe, EAccountType account_type) noexcept;
    void instanced_set(std::uint32_t account_id, std::uint32_t instance, EUniverse universe, EAccountType account_type) noexcept;
    bool set_from_string(std::string_view steam_id, EUniverse universe);
    bool set_from_steam3_string(std::string_view steam_id);
    void set_from_uint64(std::uint64_t id) noexcept;

    [[nodiscard]] std::uint64_t to_uint64() const noexcept;
    [[nodiscard]] std::uint64_t static_account_key() const noexcept;
    [[nodiscard]] std::string render(bool steam3 = true) const;

    [[nodiscard]] std::uint32_t account_id() const noexcept;
    void set_account_id(std::uint32_t value) noexcept;

    [[nodiscard]] std::uint32_t account_instance() const noexcept;
    void set_account_instance(std::uint32_t value) noexcept;

    [[nodiscard]] EAccountType account_type() const noexcept;
    void set_account_type(EAccountType value) noexcept;

    [[nodiscard]] EUniverse account_universe() const noexcept;
    void set_account_universe(EUniverse value) noexcept;

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] bool is_clan_account() const noexcept;
    [[nodiscard]] bool is_chat_account() const noexcept;
    [[nodiscard]] bool is_individual_account() const noexcept;
    [[nodiscard]] bool is_lobby() const noexcept;

    friend bool operator==(SteamID lhs, SteamID rhs) noexcept = default;

private:
    [[nodiscard]] std::uint64_t get_bits(std::uint32_t offset, std::uint64_t mask) const noexcept;
    void set_bits(std::uint32_t offset, std::uint64_t mask, std::uint64_t value) noexcept;
    [[nodiscard]] std::string render_steam2() const;
    [[nodiscard]] std::string render_steam3() const;

    std::uint64_t value_ = 0;
};

class GameID
{
public:
    enum class GameType : std::uint8_t
    {
        App = 0,
        GameMod = 1,
        Shortcut = 2,
        P2P = 3,
    };

    GameID() = default;
    explicit GameID(std::uint64_t id) noexcept;
    explicit GameID(std::uint32_t app_id) noexcept;
    explicit GameID(int app_id) noexcept;
    GameID(std::uint32_t app_id, std::string_view mod_path);
    GameID(std::string_view exe_path, std::string_view app_name);

    void set(std::uint64_t id) noexcept;
    [[nodiscard]] std::uint64_t to_uint64() const noexcept;

    [[nodiscard]] std::uint32_t app_id() const noexcept;
    void set_app_id(std::uint32_t value) noexcept;

    [[nodiscard]] GameType app_type() const noexcept;
    void set_app_type(GameType value) noexcept;

    [[nodiscard]] std::uint32_t mod_id() const noexcept;
    void set_mod_id(std::uint32_t value) noexcept;

    [[nodiscard]] bool is_mod() const noexcept;
    [[nodiscard]] bool is_shortcut() const noexcept;
    [[nodiscard]] bool is_p2p_file() const noexcept;
    [[nodiscard]] bool is_steam_app() const noexcept;

    friend bool operator==(GameID lhs, GameID rhs) noexcept = default;

private:
    [[nodiscard]] std::uint64_t get_bits(std::uint32_t offset, std::uint64_t mask) const noexcept;
    void set_bits(std::uint32_t offset, std::uint64_t mask, std::uint64_t value) noexcept;

    std::uint64_t value_ = 0;
};
}

template <>
struct std::hash<steamkit::JobID>
{
    std::size_t operator()(steamkit::JobID id) const noexcept
    {
        return std::hash<std::uint64_t>{}(id.value());
    }
};

template <>
struct std::hash<steamkit::SteamID>
{
    std::size_t operator()(steamkit::SteamID id) const noexcept
    {
        return std::hash<std::uint64_t>{}(id.to_uint64());
    }
};

template <>
struct std::hash<steamkit::GameID>
{
    std::size_t operator()(steamkit::GameID id) const noexcept
    {
        return std::hash<std::uint64_t>{}(id.to_uint64());
    }
};
