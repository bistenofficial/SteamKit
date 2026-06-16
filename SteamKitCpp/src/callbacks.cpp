#include <steamkit/callbacks.hpp>

#include <algorithm>

#include <steamkit/client.hpp>

namespace steamkit
{
Subscription::Subscription(CallbackManager* manager, std::uint64_t id) noexcept
    : manager_(manager), id_(id)
{
}

Subscription::Subscription(Subscription&& other) noexcept
    : manager_(other.manager_), id_(other.id_)
{
    other.manager_ = nullptr;
    other.id_ = 0;
}

Subscription& Subscription::operator=(Subscription&& other) noexcept
{
    if (this != &other)
    {
        unsubscribe();
        manager_ = other.manager_;
        id_ = other.id_;
        other.manager_ = nullptr;
        other.id_ = 0;
    }
    return *this;
}

Subscription::~Subscription()
{
    unsubscribe();
}

void Subscription::unsubscribe() noexcept
{
    if (manager_ != nullptr)
    {
        manager_->unregister(id_);
        manager_ = nullptr;
        id_ = 0;
    }
}

CallbackManager::CallbackManager(Client& client)
    : client_(client)
{
}

bool CallbackManager::run_callbacks()
{
    auto callback = client_.poll_callback();
    if (!callback)
    {
        return false;
    }

    handle(callback);
    return true;
}

bool CallbackManager::run_wait_callbacks(std::chrono::milliseconds timeout)
{
    auto callback = client_.wait_callback(timeout);
    if (!callback)
    {
        return false;
    }

    handle(callback);
    return true;
}

void CallbackManager::run_wait_all_callbacks(std::chrono::milliseconds timeout)
{
    if (!run_wait_callbacks(timeout))
    {
        return;
    }

    while (run_callbacks())
    {
    }
}

Subscription CallbackManager::register_callback(CallbackInvoker invoker)
{
    const auto id = next_subscription_id_++;
    entries_.push_back(Entry{id, std::move(invoker)});
    return Subscription(this, id);
}

void CallbackManager::unregister(std::uint64_t id) noexcept
{
    entries_.erase(
        std::remove_if(entries_.begin(), entries_.end(), [id](const Entry& entry) {
            return entry.id == id;
        }),
        entries_.end());
}

void CallbackManager::handle(const std::shared_ptr<CallbackMsg>& callback)
{
    const auto snapshot = entries_;
    for (const auto& entry : snapshot)
    {
        entry.invoker(*callback);
    }
}
}
