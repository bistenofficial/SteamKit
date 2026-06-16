#pragma once

#include <chrono>
#include <future>
#include <memory>
#include <utility>

#include <steamkit/types.hpp>

namespace steamkit
{
class AsyncJobBase
{
public:
    explicit AsyncJobBase(JobID job_id)
        : job_id_(job_id)
    {
    }

    virtual ~AsyncJobBase() = default;

    [[nodiscard]] JobID job_id() const noexcept
    {
        return job_id_;
    }

    std::chrono::milliseconds timeout = std::chrono::seconds(10);

private:
    JobID job_id_;
};

template <typename T>
class AsyncJob final : public AsyncJobBase
{
public:
    AsyncJob(JobID job_id, std::shared_future<T> future)
        : AsyncJobBase(job_id), future_(std::move(future))
    {
    }

    static AsyncJob completed(JobID job_id, T value)
    {
        std::promise<T> promise;
        promise.set_value(std::move(value));
        return AsyncJob(job_id, promise.get_future().share());
    }

    [[nodiscard]] bool is_ready() const
    {
        return future_.valid() && future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    T get() const
    {
        return future_.get();
    }

private:
    std::shared_future<T> future_;
};
}
