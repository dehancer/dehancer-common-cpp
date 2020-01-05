//
// Created by denn nevera on 2020-01-05.
//

#include "dehancer/Semaphore.h"
#include "gtest/gtest.h"
#include <list>

TEST(Semaphore, BasicTest) {

  std::cout << std::endl;

  std::cout << "sema started... " << std::endl;

  auto semaphore = dehancer::Semaphore(-1);

  semaphore.signal();
  semaphore.signal();

  semaphore.wait_for(std::chrono::seconds(1));

  std::cout << "exited... " << std::endl;

}

TEST(Semaphore, CountTest) {

  std::cout << std::endl;

  std::cout << "sema started... " << std::endl;

  int  count     = 10;
  auto semaphore = dehancer::Semaphore(-(count-1));

  auto th = std::thread([count,&semaphore](){

      for (int i = 0; i < count; ++i) {
        std::cout << "signal["<<std::this_thread::get_id()<<"]: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        semaphore.signal();
      }

  });


  std::cout << "main thread: " << std::this_thread::get_id() << std::endl;

  semaphore.wait_for(std::chrono::seconds(1));

  th.join();

  std::cout << "exited... " << std::endl;

}
