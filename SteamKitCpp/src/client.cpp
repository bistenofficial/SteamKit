#include <steamkit/client.hpp>

#include <atomic>
#include <utility>

#include <steamkit/exception.hpp>

namespace steamkit
{
namespace
{
std::string next_identifier()
{
    static std::atomic<std::uint64_t> counter = 1;
    return "steamkitcpp-" + std::to_string(counter.fetch_add(1));
}
}

Client::Client()
    : Client(Configuration{})
{
}

Client::Client(Configuration configuration)
    : Client(std::move(configuration), next_identifier())
{
}

Client::Client(Configuration configuration, std::string identifier)
    : configuration_(std::move(configuration)),
      identifier_(std::move(identifier)),
      user_(*this),
      friends_(*this),
      apps_(*this),
      game_coordinator_(*this),
      unified_messages_(*this),
      cdn_(*this)
{
}

const Configuration& Client::configuration() const noexcept
{
    return configuration_;
}

const std::string& Client::identifier() const noexcept
{
    return identifier_;
}

bool Client::is_connected() const noexcept
{
    return connected_;
}

SteamID Client::steam_id() const noexcept
{
    return steam_id_;
}

void Client::connect()
{
    throw NotImplemented("Client::connect");
}

void Client::disconnect()
{
    connected_ = false;
}

void Client::post_callback(std::shared_ptr<CallbackMsg> callback)
{
    {
        std::lock_guard lock(callbacks_mutex_);
        callbacks_.push(std::move(callback));
    }
    callbacks_cv_.notify_one();
}

std::shared_ptr<CallbackMsg> Client::poll_callback()
{
    std::lock_guard lock(callbacks_mutex_);
    if (callbacks_.empty())
    {
        return {};
    }

    auto callback = callbacks_.front();
    callbacks_.pop();
    return callback;
}

std::shared_ptr<CallbackMsg> Client::wait_callback()
{
    std::unique_lock lock(callbacks_mutex_);
    callbacks_cv_.wait(lock, [&] { return !callbacks_.empty(); });

    auto callback = callbacks_.front();
    callbacks_.pop();
    return callback;
}

std::shared_ptr<CallbackMsg> Client::wait_callback(std::chrono::milliseconds timeout)
{
    std::unique_lock lock(callbacks_mutex_);
    if (!callbacks_cv_.wait_for(lock, timeout, [&] { return !callbacks_.empty(); }))
    {
        return {};
    }

    auto callback = callbacks_.front();
    callbacks_.pop();
    return callback;
}

SteamUser& Client::user() noexcept
{
    return user_;
}

SteamFriends& Client::friends() noexcept
{
    return friends_;
}

SteamApps& Client::apps() noexcept
{
    return apps_;
}

SteamGameCoordinator& Client::game_coordinator() noexcept
{
    return game_coordinator_;
}

SteamUnifiedMessages& Client::unified_messages() noexcept
{
    return unified_messages_;
}

CDNClient& Client::cdn() noexcept
{
    return cdn_;
}

SteamUser::SteamUser(Client& client) noexcept
    : client_(client)
{
}

AsyncJob<SteamUser::LoggedOnCallback> SteamUser::log_on(const LogOnDetails& details)
{
    (void)details;
    throw NotImplemented("SteamUser::log_on");
}

void SteamUser::log_off()
{
    throw NotImplemented("SteamUser::log_off");
}

SteamFriends::SteamFriends(Client& client) noexcept
    : client_(client)
{
}

void SteamFriends::set_persona_state(EPersonaState state)
{
    (void)state;
    throw NotImplemented("SteamFriends::set_persona_state");
}

SteamApps::SteamApps(Client& client) noexcept
    : client_(client)
{
}

SteamGameCoordinator::SteamGameCoordinator(Client& client) noexcept
    : client_(client)
{
}

SteamUnifiedMessages::SteamUnifiedMessages(Client& client) noexcept
    : client_(client)
{
}

CDNClient::CDNClient(Client& client) noexcept
    : client_(client)
{
}
}
