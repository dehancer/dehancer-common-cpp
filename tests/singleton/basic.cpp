//
// Created by denn nevera on 2020-01-05.
//

#include "dehancer/Common.h"
#include "dehancer/Semaphore.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "gtest/gtest.h"
#include <list>

struct Object {
  std::atomic_int value = 1;
  std::string name = "Object";
};

class SingletonObject: public dehancer::Singleton<Object>{};

TEST(Singleton, CountTest) {

  std::cout << std::endl;

  std::cout << "shared singleton object started... " << std::endl;

  int  count     = 10;
  auto semaphore = dehancer::Semaphore(-(count-1));

  for (auto i = 0; i < count; ++i) {

    auto th = std::thread([count,&semaphore](){

      auto& s_object = SingletonObject::Instance();

        for (auto j = 0; j < count; ++j) {

          std::cout << "signal["<<std::this_thread::get_id()<<"]: " << j
          << " s_object.value: " << s_object.value++
          << " s_object.name: " << s_object.name
          << std::endl;

          std::this_thread::sleep_for(std::chrono::milliseconds(300));
          semaphore.signal();
        }

    });

    th.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(30));

  }

  std::cout << "main thread: " << std::this_thread::get_id() << std::endl;

  semaphore.wait_for(std::chrono::seconds(1));

  std::cout << "exited... " << std::endl;

}
