#ifndef JOB_H
#define JOB_H

#include <functional>
#include <tuple>
#include <utility>
#include <memory>

//template <typename ... Args>
class Job
{
    struct ImplBase
    {
        virtual void Call() = 0;
        virtual ~ImplBase() {}
    };

    template <typename F>
    struct ImplType : ImplBase
    {
        F fn;
        ImplType(F&& f) : fn(std::move(f)) {}
        void Call() { fn(); }
    };

    std::unique_ptr<ImplBase> impl;

public:
    template <typename F>
    Job(F&& f)
        : impl(new ImplType<F>(std::move(f)))
    {
    }

    Job() = default;

    Job(Job&& other)
        : impl(std::move(other.impl))
    {
    }

    Job& operator=(Job&& other)
    {
        impl = std::move(other.impl);
        return *this;
    }

    Job(const Job&) = delete;
    Job(Job&) = delete;
    Job& operator=(const Job&) = delete;

    void operator()() { impl->Call(); }

    /*
    template <typename F, typename ... Args>
    Job(F&& func, Args&& ... args)
        : fn(std::forward<F>(func)),
          args(std::forward<Args>(args)...)
    {
    }
    */

//private:
    //std::function<void(Args...)> fn;
    //std::tuple<Args...> args;
};

#endif // JOB_H
