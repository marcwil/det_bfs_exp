#include <chrono>
#include <thread>

#include "timer.hpp"
#include "gtest/gtest.h"

TEST(timer, wait) {
  Timer::create_timer("1st");
  Timer::create_timer("2nd");
  Timer::create_timer("overall");

  Timer::start_timer("overall");

  Timer::start_timer("1st");
  std::this_thread::sleep_for(std::chrono::milliseconds(30));
  Timer::stop_timer("1st");

  Timer::start_timer("2nd");
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  Timer::stop_timer("2nd");

  Timer::start_timer("1st");
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  Timer::stop_timer("1st");

  Timer::stop_timer("overall");

  double delta = 2.0;

  EXPECT_GE(Timer::time("overall").count(), 90);
  EXPECT_LE(Timer::time("overall").count(), 90 + delta);

  EXPECT_GE(Timer::time("1st").count(), 80);
  EXPECT_LE(Timer::time("1st").count(), 80 + delta);

  EXPECT_GE(Timer::time("2nd").count(), 10);
  EXPECT_LE(Timer::time("2nd").count(), 10 + delta);
}
