#ifndef CO_GENERATOR_H
#define CO_GENERATOR_H

#include <coroutine>
#include <memory>
#include <iostream>

namespace Campfire {

template<typename T>
class CoGenerator
{
    public:
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;
    private:
        handle_type coro;
    public:
        explicit CoGenerator(handle_type h) : coro(h) {}
        CoGenerator(const CoGenerator&) = delete;
        CoGenerator(CoGenerator&& oth) noexcept : coro(oth.coro) {
            oth.coro = nullptr;
        }
        CoGenerator& operator=(const CoGenerator&) = delete;
        CoGenerator& operator=(CoGenerator&& other) noexcept {
            coro = other.coro;
            other.coro = nullptr;
            return *this;
        }
        ~CoGenerator() {
            if (coro) {
                coro.destroy();
            }
        }

        bool next() {
            coro.resume();
            return not coro.done();
        }

        T getValue() {
            return coro.promise().current_value;
        }

        struct promise_type {
        private:
            T current_value{};
            friend class CoGenerator;
        public:
            promise_type() = default;
            ~promise_type() = default;
            promise_type(const promise_type&) = delete;
            promise_type(promise_type&&) = delete;
            promise_type& operator=(const promise_type&) = delete;
            promise_type& operator=(promise_type&&) = delete;

            auto initial_suspend() {
                return std::suspend_always{};
            }

            auto final_suspend() {
                return std::suspend_always{};
            }

            auto get_return_object() {
                return CoGenerator{ handle_type::from_promise(*this) };
            }

            auto return_void() {
                return std::suspend_never{};
            }

            auto yield_value(T some_value) {
                current_value = some_value;
                return std::suspend_always{};
            }

            void unhandled_exception() {
                std::exit(1);
            }
        };
    };
} // Campfire

#endif //CO_GENERATOR_H

//Source:https://github.com/roger-dv/cpp20-coro-generator/blob/master/generator.h