#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include <steamkit/types.hpp>

namespace steamkit
{
class CallbackMsg
{
public:
    explicit CallbackMsg(JobID job_id = JobID::invalid()) noexcept
        : job_id_(job_id)
    {
    }

    virtual ~CallbackMsg() = default;

    [[nodiscard]] JobID job_id() const noexcept
    {
        return job_id_;
    }

    void set_job_id(JobID job_id) noexcept
    {
        job_id_ = job_id;
    }

private:
    JobID job_id_;
};

class Client;
class CallbackManager;

class Subscription
{
public:
    Subscription() = default;
    Subscription(CallbackManager* manager, std::uint64_t id) noexcept;
    Subscription(const Subscription&) = delete;
    Subscription& operator=(const Subscription&) = delete;
    Subscription(Subscription&& other) noexcept;
    Subscription& operator=(Subscription&& other) noexcept;
    ~Subscription();

    void unsubscribe() noexcept;

private:
    CallbackManager* manager_ = nullptr;
    std::uint64_t id_ = 0;
};

class CallbackManager
{
public:
    explicit CallbackManager(Client& client);

    bool run_callbacks();
    bool run_wait_callbacks(std::chrono::milliseconds timeout);
    void run_wait_all_callbacks(std::chrono::milliseconds timeout);

    template <typename TCallback, typename TFunc>
    Subscription subscribe(TFunc&& callback)
    {
        return subscribe<TCallback>(JobID::invalid(), std::forward<TFunc>(callback));
    }

    template <typename TCallback, typename TFunc>
    Subscription subscribe(JobID job_id, TFunc&& callback)
    {
        static_assert(std::is_base_of_v<CallbackMsg, TCallback>, "TCallback must derive from CallbackMsg");

        auto invoker = [job_id, fn = std::forward<TFunc>(callback)](const CallbackMsg& message) mutable {
            if (job_id.is_valid() && message.job_id() != job_id)
            {
                return false;
            }

            const auto* typed = dynamic_cast<const TCallback*>(&message);
            if (typed == nullptr)
            {
                return false;
            }

            fn(*typed);
            return true;
        };

        return register_callback(std::move(invoker));
    }

private:
    friend class Subscription;

    using CallbackInvoker = std::function<bool(const CallbackMsg&)>;

    Subscription register_callback(CallbackInvoker invoker);
    void unregister(std::uint64_t id) noexcept;
    void handle(const std::shared_ptr<CallbackMsg>& callback);

    Client& client_;

    struct Entry
    {
        std::uint64_t id;
        CallbackInvoker invoker;
    };

    std::uint64_t next_subscription_id_ = 1;
    std::vector<Entry> entries_;
};
}
