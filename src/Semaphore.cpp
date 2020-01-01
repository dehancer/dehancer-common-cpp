//
// Created by denn nevera on 2020-01-01.
//

#include "dehancer/Semaphore.hpp"

namespace dehancer {

    Semaphore::Semaphore (int count_) : count_(count_) {}

    void Semaphore::signal()
    {
      std::unique_lock<std::mutex> lock(mutex_);
      count_++;
      signal_.notify_one();
    }

    void Semaphore::wait()
    {
      std::unique_lock<std::mutex> lock(mutex_);
      while(count_ == 0) signal_.wait(lock);
      count_--;
    }
}