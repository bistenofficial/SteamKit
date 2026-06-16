#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <string>

#include <steamkit/callbacks.hpp>
#include <steamkit/configuration.hpp>
#include <steamkit/handlers.hpp>
#include <steamkit/types.hpp>

namespace steamkit
{
class Client
{
public:
    Client();
    explicit Client(Configuration configuration);
    Client(Configuration configuration, std::string identifier);

    [[nodiscard]] const Configuration& configuration() const noexcept;
    [[nodiscard]] const std::string& identifier() const noexcept;
    [[nodiscard]] bool is_connected() const noexcept;
    [[nodiscard]] SteamID steam_id() const noexcept;

    void connect();
    void disconnect();

    void post_callback(std::shared_ptr<CallbackMsg> callback);
    std::shared_ptr<CallbackMsg> poll_callback();
    std::shared_ptr<CallbackMsg> wait_callback();
    std::shared_ptr<CallbackMsg> wait_callback(std::chrono::milliseconds timeout);

    SteamUser& user() noexcept;
    SteamFriends& friends() noexcept;
    SteamApps& apps() noexcept;
    SteamGameCoordinator& game_coordinator() noexcept;
    SteamUnifiedMessages& unified_messages() noexcept;
    CDNClient& cdn() noexcept;

private:
    Configuration configuration_;
    std::string identifier_;
    bool connected_ = false;
    SteamID steam_id_;

    std::mutex callbacks_mutex_;
    std::condition_variable callbacks_cv_;
    std::queue<std::shared_ptr<CallbackMsg>> callbacks_;

    SteamUser user_;
    SteamFriends friends_;
    SteamApps apps_;
    SteamGameCoordinator game_coordinator_;
    SteamUnifiedMessages unified_messages_;
    CDNClient cdn_;
};
}
