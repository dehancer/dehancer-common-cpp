//
// Created by denn nevera on 2020-01-01.
//

#pragma once

#include <condition_variable>
#include <thread>
#include <mutex>
#include <chrono>

namespace dehancer{

    /**
     * Base semaphore operations
     */

    template <typename Mutex, typename Condition, typename Thread>
    class basic_semaphore {
    public:
        using native_handle_type = typename Thread::native_handle_type;

        /**
         * Create semaphore with counter size
         * @param count - max await signals
         */

        explicit basic_semaphore(int count = 0);

        /**
         * Notify semaphore
         */
        void signal();

        /**
         * Wait semaphore
         */
        void wait();

        /**
         * Check waiting condition
         * @return true/false
         */
        bool try_wait();

        /**
         * Wait for time
         * @tparam Rep - precision
         * @tparam Period - time
         * @param duration - furation time
         * @return finished or not
         */
        template<class Rep, class Period>
        bool wait_for(const std::chrono::duration<Rep, Period>& duration);

        /**
         * Wait until time
         * @tparam Clock - time point type
         * @tparam Duration - time duration type
         * @param time - exact time
         * @return finished or not
         */
        template<class Clock, class Duration>
        bool wait_until(const std::chrono::time_point<Clock, Duration>& t);

        native_handle_type native_handle();

        basic_semaphore(const basic_semaphore&) = delete;
        basic_semaphore(basic_semaphore&&) = delete;
        basic_semaphore& operator=(const basic_semaphore&) = delete;
        basic_semaphore& operator=(basic_semaphore&&) = delete;

    private:
        Mutex     mutex_;
        Condition condition_;
        int       count_;
    };

    template <typename Mutex, typename CondVar, typename Thread>
    basic_semaphore<Mutex, CondVar, Thread>::basic_semaphore(int count)
            : count_{count}
    {}

    template <typename Mutex, typename CondVar,typename Thread>
    void basic_semaphore<Mutex, CondVar, Thread>::signal() {
      std::lock_guard<Mutex> lock{mutex_};
      ++count_;
      condition_.notify_one();
    }

    template <typename Mutex, typename CondVar, typename Thread>
    void basic_semaphore<Mutex, CondVar, Thread>::wait() {
      std::unique_lock<Mutex> lock{mutex_};
      condition_.wait(lock, [&]{ return count_ > 0; });
      --count_;
    }

    template <typename Mutex, typename CondVar, typename Thread>
    bool basic_semaphore<Mutex, CondVar, Thread>::try_wait() {
      std::lock_guard<Mutex> lock{mutex_};

      if (count_ > 0) {
        --count_;
        return true;
      }

      return false;
    }

    template <typename Mutex, typename CondVar, typename Thread>
    template<class Rep, class Period>
    bool basic_semaphore<Mutex, CondVar, Thread>::wait_for(const std::chrono::duration<Rep, Period>& d) {
      std::unique_lock<Mutex> lock{mutex_};
      auto finished = condition_.wait_for(lock, d, [&]{ return count_ > 0; });

      if (finished) --count_;

      return finished;
    }

    template <typename Mutex, typename CondVar, typename Thread>
    template<class Clock, class Duration>
    bool basic_semaphore<Mutex, CondVar, Thread>::wait_until(const std::chrono::time_point<Clock, Duration>& t) {
      std::unique_lock<Mutex> lock{mutex_};
      auto finished = condition_.wait_until(lock, t, [&]{ return count_ > 0; });

      if (finished) --count_;

      return finished;
    }

    template <typename Mutex, typename CondVar, typename Thread>
    typename basic_semaphore<Mutex, CondVar, Thread>::native_handle_type basic_semaphore<Mutex, CondVar, Thread>::native_handle() {
      return condition_.native_handle();
    }

    using Semaphore = basic_semaphore<std::mutex, std::condition_variable, std::thread>;
}


