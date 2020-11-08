#ifndef JOB_STEALING_QUEUE_H
#define JOB_STEALING_QUEUE_H

#include <thread>
#include <queue>

template <typename T>
class JobStealingQueue
{
public:
    JobStealingQueue() {}
    JobStealingQueue(const JobStealingQueue& other) = delete;
    JobStealingQueue& operator=(const JobStealingQueue&) = delete;

    void Push(T newEntry)
    {
        std::lock_guard<std::mutex> lock(mutex);
        data.push(std::move(newEntry));
    }

    bool TryPop(T& value)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (data.empty()) { return false; }
        value = std::move(data.front());
        data.pop_front();

        return true;
    }

    bool TrySteal(T& value)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (data.empty()) { return false; }
        value = std::move(data.back());
        data.pop_back();

        return true;
    }

    bool Empty() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return data.empty();
    }

private:
    std::deque<T> data;
    mutable std::mutex mutex;
};

#endif // JOB_STEALING_QUEUE_H