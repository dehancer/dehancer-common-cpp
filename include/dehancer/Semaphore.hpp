//
// Created by denn nevera on 2020-01-01.
//

#
#pragma once

#include <condition_variable>
#include <thread>
#include <mutex>

namespace dehancer{

    /**
     * Base semaphore operations
     */
    class Semaphore {
    public:
        /**
         * Create semaphore with counter size
         * @param count - max await signals
         */
        Semaphore (int count = 0);

        /**
         * One signal to semaphore
         */
        void signal();

        /**
         * Wait signals
         */
        void wait();

    private:
        std::mutex              mutex_;
        std::condition_variable signal_;
        int                     count_;
    };
}


