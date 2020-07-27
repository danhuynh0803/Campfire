#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <condition_variable>
#include <thread>
#include <queue>

template <typename T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue() {}

    ThreadSafeQueue(const ThreadSafeQueue& other)
    {
        std::lock_guard<std::mutex> lock(other.mutex);
        data = other.data;
    }

    //ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    void Push(T newEntry)
    {
        std::lock_guard<std::mutex> lock(mutex);
        data.push(std::move(newEntry));
        dataCond.notify_one();
    }

    void WaitAndPop(T& value)
    {
        std::unique_lock<std::mutex> lock(mutex);
        dataCond.wait(lock, [this] { return !data.empty(); });
        value = std::move(data.front());
        data.pop();
    }

    std::shared_ptr<T> WaitAndPop()
    {
        std::unique_lock<std::mutex> lock(mutex);
        dataCond.wait(lock, [this] { return !data.empty(); });
        std::shared_ptr<T> res(
            std::make_shared<T>(std::move(data.front()))
        );
        data.pop();

        return res;
    }

    bool TryPop(T& value)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (data.empty()) { return false; }
        value = std::move(data.front());
        data.pop();

        return true;
    }

    bool Empty() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return data.empty();
    }

private:
    std::queue<T> data;
    mutable std::mutex mutex;
    std::condition_variable dataCond;
};

#endif // THREAD_SAFE_QUEUE_H
