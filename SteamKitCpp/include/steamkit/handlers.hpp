#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <steamkit/async_job.hpp>
#include <steamkit/enums.hpp>
#include <steamkit/exception.hpp>
#include <steamkit/messages.hpp>
#include <steamkit/types.hpp>

namespace steamkit
{
class Client;

class SteamUser
{
public:
    struct LogOnDetails
    {
        std::string username;
        std::string password;
        std::string two_factor_code;
        std::string access_token;
        bool should_remember_password = false;
    };

    struct LoggedOnCallback final : CallbackMsg
    {
        explicit LoggedOnCallback(EResult result = EResult::Invalid)
            : result(result)
        {
        }

        EResult result;
        EResult extended_result = EResult::Invalid;
    };

    explicit SteamUser(Client& client) noexcept;

    AsyncJob<LoggedOnCallback> log_on(const LogOnDetails& details);
    void log_off();

private:
    Client& client_;
};

class SteamFriends
{
public:
    explicit SteamFriends(Client& client) noexcept;
    void set_persona_state(EPersonaState state);

private:
    Client& client_;
};

class SteamApps
{
public:
    explicit SteamApps(Client& client) noexcept;

private:
    Client& client_;
};

class SteamGameCoordinator
{
public:
    struct MessageCallback final : CallbackMsg
    {
        std::uint32_t emsg = 0;
        std::vector<std::byte> payload;
    };

    explicit SteamGameCoordinator(Client& client) noexcept;

    template <typename TBody>
    void send(const ClientGCMsg<TBody>&, std::uint32_t app_id)
    {
        (void)app_id;
        throw NotImplemented("SteamGameCoordinator::send");
    }

private:
    Client& client_;
};

class SteamUnifiedMessages
{
public:
    explicit SteamUnifiedMessages(Client& client) noexcept;

private:
    Client& client_;
};

class CDNClient
{
public:
    explicit CDNClient(Client& client) noexcept;

private:
    Client& client_;
};
}
