#include <steamkit/types.hpp>

#include <array>
#include <charconv>
#include <optional>
#include <regex>
#include <sstream>
#include <string>

namespace steamkit
{
namespace
{
constexpr char UnknownAccountTypeChar = 'i';

std::uint32_t crc32(std::string_view data)
{
    static constexpr std::uint32_t Polynomial = 0xEDB88320U;
    static constexpr auto table = [] {
        std::array<std::uint32_t, 256> values{};
        for (std::uint32_t i = 0; i < values.size(); ++i)
        {
            std::uint32_t crc = i;
            for (int bit = 0; bit < 8; ++bit)
            {
                crc = (crc & 1U) != 0 ? (Polynomial ^ (crc >> 1U)) : (crc >> 1U);
            }
            values[i] = crc;
        }
        return values;
    }();

    std::uint32_t crc = 0xFFFFFFFFU;
    for (unsigned char byte : data)
    {
        crc = table[(crc ^ byte) & 0xFFU] ^ (crc >> 8U);
    }
    return crc ^ 0xFFFFFFFFU;
}

std::optional<std::uint32_t> parse_u32(const std::ssub_match& match)
{
    const auto text = match.str();
    std::uint64_t value = 0;
    const auto* first = text.data();
    const auto* last = text.data() + text.size();
    auto [ptr, ec] = std::from_chars(first, last, value);
    if (ec != std::errc{} || ptr != last || value > 0xFFFFFFFFULL)
    {
        return std::nullopt;
    }
    return static_cast<std::uint32_t>(value);
}

std::optional<EAccountType> account_type_from_char(char value)
{
    switch (value)
    {
    case 'A':
        return EAccountType::AnonGameServer;
    case 'G':
        return EAccountType::GameServer;
    case 'M':
        return EAccountType::Multiseat;
    case 'P':
        return EAccountType::Pending;
    case 'C':
        return EAccountType::ContentServer;
    case 'g':
        return EAccountType::Clan;
    case 'T':
        return EAccountType::Chat;
    case 'I':
        return EAccountType::Invalid;
    case 'U':
        return EAccountType::Individual;
    case 'a':
        return EAccountType::AnonUser;
    case UnknownAccountTypeChar:
        return EAccountType::Invalid;
    default:
        return std::nullopt;
    }
}

char account_type_to_char(EAccountType value)
{
    switch (value)
    {
    case EAccountType::AnonGameServer:
        return 'A';
    case EAccountType::GameServer:
        return 'G';
    case EAccountType::Multiseat:
        return 'M';
    case EAccountType::Pending:
        return 'P';
    case EAccountType::ContentServer:
        return 'C';
    case EAccountType::Clan:
        return 'g';
    case EAccountType::Chat:
        return 'T';
    case EAccountType::Invalid:
        return 'I';
    case EAccountType::Individual:
        return 'U';
    case EAccountType::AnonUser:
        return 'a';
    default:
        return UnknownAccountTypeChar;
    }
}
}

SteamID::SteamID(std::uint64_t id) noexcept
    : value_(id)
{
}

SteamID::SteamID(std::string_view steam_id)
{
    set_from_string(steam_id, EUniverse::Public);
}

SteamID::SteamID(std::uint32_t account_id, EUniverse universe, EAccountType account_type) noexcept
{
    set(account_id, universe, account_type);
}

SteamID::SteamID(std::uint32_t account_id, std::uint32_t instance, EUniverse universe, EAccountType account_type) noexcept
{
    instanced_set(account_id, instance, universe, account_type);
}

void SteamID::set(std::uint32_t account_id, EUniverse universe, EAccountType account_type) noexcept
{
    set_account_id(account_id);
    set_account_universe(universe);
    set_account_type(account_type);
    set_account_instance(account_type == EAccountType::Clan || account_type == EAccountType::GameServer ? 0 : DesktopInstance);
}

void SteamID::instanced_set(std::uint32_t account_id, std::uint32_t instance, EUniverse universe, EAccountType account_type) noexcept
{
    set_account_id(account_id);
    set_account_universe(universe);
    set_account_type(account_type);
    set_account_instance(instance);
}

bool SteamID::set_from_string(std::string_view steam_id, EUniverse universe)
{
    if (steam_id.empty())
    {
        return false;
    }

    static const std::regex steam2_regex(R"(^STEAM_([0-4]):([0-1]):([0-9]+)$)", std::regex::icase);
    std::smatch match;
    const std::string text(steam_id);
    if (!std::regex_match(text, match, steam2_regex))
    {
        return false;
    }

    const auto auth_server = parse_u32(match[2]);
    const auto account_id = parse_u32(match[3]);
    if (!auth_server || !account_id)
    {
        return false;
    }

    set_account_universe(universe);
    set_account_instance(DesktopInstance);
    set_account_type(EAccountType::Individual);
    set_account_id((*account_id << 1U) | *auth_server);
    return true;
}

bool SteamID::set_from_steam3_string(std::string_view steam_id)
{
    if (steam_id.empty())
    {
        return false;
    }

    static const std::regex steam3_regex(R"(^\[([AGMPCgcLTIUai]):([0-4]):([0-9]+)(?::([0-9]+))?\]$)");
    static const std::regex steam3_fallback_regex(R"(^\[([AGMPCgcLTIUai]):([0-4]):([0-9]+)(?:\(([0-9]+)\))?\]$)");

    std::smatch match;
    const std::string text(steam_id);
    if (!std::regex_match(text, match, steam3_regex) && !std::regex_match(text, match, steam3_fallback_regex))
    {
        return false;
    }

    const auto account = parse_u32(match[3]);
    const auto universe = parse_u32(match[2]);
    if (!account || !universe || *universe > static_cast<std::uint32_t>(EUniverse::Dev))
    {
        return false;
    }

    const char type_char = match[1].str()[0];
    auto type = account_type_from_char(type_char);
    if (!type)
    {
        return false;
    }

    std::uint32_t instance = 1;
    if (match[4].matched)
    {
        auto parsed_instance = parse_u32(match[4]);
        if (!parsed_instance)
        {
            return false;
        }
        instance = *parsed_instance;
    }
    else
    {
        instance = type_char == 'g' || type_char == 'T' || type_char == 'c' || type_char == 'L' ? 0 : 1;
    }

    if (type_char == 'c')
    {
        instance |= ChatInstanceClan;
        type = EAccountType::Chat;
    }
    else if (type_char == 'L')
    {
        instance |= ChatInstanceLobby;
        type = EAccountType::Chat;
    }

    set_account_universe(static_cast<EUniverse>(*universe));
    set_account_instance(instance);
    set_account_type(*type);
    set_account_id(*account);
    return true;
}

void SteamID::set_from_uint64(std::uint64_t id) noexcept
{
    value_ = id;
}

std::uint64_t SteamID::to_uint64() const noexcept
{
    return value_;
}

std::uint64_t SteamID::static_account_key() const noexcept
{
    return (static_cast<std::uint64_t>(account_universe()) << 56U)
        + (static_cast<std::uint64_t>(account_type()) << 52U)
        + account_id();
}

std::string SteamID::render(bool steam3) const
{
    return steam3 ? render_steam3() : render_steam2();
}

std::uint32_t SteamID::account_id() const noexcept
{
    return static_cast<std::uint32_t>(get_bits(0, 0xFFFFFFFFULL));
}

void SteamID::set_account_id(std::uint32_t value) noexcept
{
    set_bits(0, 0xFFFFFFFFULL, value);
}

std::uint32_t SteamID::account_instance() const noexcept
{
    return static_cast<std::uint32_t>(get_bits(32, 0xFFFFFULL));
}

void SteamID::set_account_instance(std::uint32_t value) noexcept
{
    set_bits(32, 0xFFFFFULL, value);
}

EAccountType SteamID::account_type() const noexcept
{
    return static_cast<EAccountType>(get_bits(52, 0xFULL));
}

void SteamID::set_account_type(EAccountType value) noexcept
{
    set_bits(52, 0xFULL, static_cast<std::uint64_t>(value));
}

EUniverse SteamID::account_universe() const noexcept
{
    return static_cast<EUniverse>(get_bits(56, 0xFFULL));
}

void SteamID::set_account_universe(EUniverse value) noexcept
{
    set_bits(56, 0xFFULL, static_cast<std::uint64_t>(value));
}

bool SteamID::is_valid() const noexcept
{
    if (account_type() <= EAccountType::Invalid || account_type() > EAccountType::AnonUser)
    {
        return false;
    }

    if (account_universe() <= EUniverse::Invalid || account_universe() > EUniverse::Dev)
    {
        return false;
    }

    if (account_type() == EAccountType::Individual && (account_id() == 0 || account_instance() > WebInstance))
    {
        return false;
    }

    if (account_type() == EAccountType::Clan && (account_id() == 0 || account_instance() != 0))
    {
        return false;
    }

    if (account_type() == EAccountType::GameServer && account_id() == 0)
    {
        return false;
    }

    return true;
}

bool SteamID::is_clan_account() const noexcept
{
    return account_type() == EAccountType::Clan;
}

bool SteamID::is_chat_account() const noexcept
{
    return account_type() == EAccountType::Chat;
}

bool SteamID::is_individual_account() const noexcept
{
    return account_type() == EAccountType::Individual || account_type() == EAccountType::ConsoleUser;
}

bool SteamID::is_lobby() const noexcept
{
    return account_type() == EAccountType::Chat && (account_instance() & ChatInstanceLobby) != 0;
}

std::uint64_t SteamID::get_bits(std::uint32_t offset, std::uint64_t mask) const noexcept
{
    return (value_ >> offset) & mask;
}

void SteamID::set_bits(std::uint32_t offset, std::uint64_t mask, std::uint64_t value) noexcept
{
    value_ = (value_ & ~(mask << offset)) | ((value & mask) << offset);
}

std::string SteamID::render_steam2() const
{
    switch (account_type())
    {
    case EAccountType::Invalid:
    case EAccountType::Individual:
    {
        const auto universe_digit = account_universe() <= EUniverse::Public ? 0 : static_cast<std::uint32_t>(account_universe());
        return "STEAM_" + std::to_string(universe_digit) + ":" + std::to_string(account_id() & 1U) + ":" + std::to_string(account_id() >> 1U);
    }
    default:
        return std::to_string(to_uint64());
    }
}

std::string SteamID::render_steam3() const
{
    char type_char = account_type_to_char(account_type());
    if (account_type() == EAccountType::Chat)
    {
        if ((account_instance() & ChatInstanceClan) != 0)
        {
            type_char = 'c';
        }
        else if ((account_instance() & ChatInstanceLobby) != 0)
        {
            type_char = 'L';
        }
    }

    bool render_instance = false;
    switch (account_type())
    {
    case EAccountType::AnonGameServer:
    case EAccountType::Multiseat:
        render_instance = true;
        break;
    case EAccountType::Individual:
        render_instance = account_instance() != DesktopInstance;
        break;
    default:
        break;
    }

    std::ostringstream rendered;
    rendered << '[' << type_char << ':' << static_cast<std::uint32_t>(account_universe()) << ':' << account_id();
    if (render_instance)
    {
        rendered << ':' << account_instance();
    }
    rendered << ']';
    return rendered.str();
}

GameID::GameID(std::uint64_t id) noexcept
    : value_(id)
{
}

GameID::GameID(std::uint32_t app_id) noexcept
    : value_(app_id)
{
}

GameID::GameID(int app_id) noexcept
    : value_(static_cast<std::uint64_t>(app_id))
{
}

GameID::GameID(std::uint32_t app_id, std::string_view mod_path)
{
    set_app_id(app_id);
    set_app_type(GameType::GameMod);
    set_mod_id(crc32(mod_path));
}

GameID::GameID(std::string_view exe_path, std::string_view app_name)
{
    std::string combined;
    combined.reserve(exe_path.size() + app_name.size());
    combined.append(exe_path);
    combined.append(app_name);

    set_app_id(0);
    set_app_type(GameType::Shortcut);
    set_mod_id(crc32(combined));
}

void GameID::set(std::uint64_t id) noexcept
{
    value_ = id;
}

std::uint64_t GameID::to_uint64() const noexcept
{
    return value_;
}

std::uint32_t GameID::app_id() const noexcept
{
    return static_cast<std::uint32_t>(get_bits(0, 0xFFFFFFULL));
}

void GameID::set_app_id(std::uint32_t value) noexcept
{
    set_bits(0, 0xFFFFFFULL, value);
}

GameID::GameType GameID::app_type() const noexcept
{
    return static_cast<GameType>(get_bits(24, 0xFFULL));
}

void GameID::set_app_type(GameType value) noexcept
{
    set_bits(24, 0xFFULL, static_cast<std::uint64_t>(value));
}

std::uint32_t GameID::mod_id() const noexcept
{
    return static_cast<std::uint32_t>(get_bits(32, 0xFFFFFFFFULL));
}

void GameID::set_mod_id(std::uint32_t value) noexcept
{
    set_bits(32, 0xFFFFFFFFULL, value);
    set_bits(63, 0xFFULL, 1);
}

bool GameID::is_mod() const noexcept
{
    return app_type() == GameType::GameMod;
}

bool GameID::is_shortcut() const noexcept
{
    return app_type() == GameType::Shortcut;
}

bool GameID::is_p2p_file() const noexcept
{
    return app_type() == GameType::P2P;
}

bool GameID::is_steam_app() const noexcept
{
    return app_type() == GameType::App;
}

std::uint64_t GameID::get_bits(std::uint32_t offset, std::uint64_t mask) const noexcept
{
    return (value_ >> offset) & mask;
}

void GameID::set_bits(std::uint32_t offset, std::uint64_t mask, std::uint64_t value) noexcept
{
    value_ = (value_ & ~(mask << offset)) | ((value & mask) << offset);
}
}
