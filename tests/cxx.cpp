// cxx.cpp - C++ on 32-bit OS/2: exceptions (with destructors run on the
// way out, rethrow and std:: exceptions), RTTI, the standard containers
// and strings, iostreams, std::thread with std::mutex and
// std::condition_variable, exceptions in several threads at once,
// thread_local objects and steady_clock.
//
// Copyright (C) clang-retro contributors.
// SPDX-License-Identifier: AGPL-3.0-only
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

static int failed;

static void check(const char *what, bool ok) {
  std::printf("%-40s %s\n", what, ok ? "ok" : "FAILED");
  if (!ok)
    failed++;
}

static int destroyed;

struct Guard {
  ~Guard() { destroyed++; }
};

struct MyError : std::runtime_error {
  int code;
  explicit MyError(int c) : std::runtime_error("my error"), code(c) {}
};

[[gnu::noinline]] static void thrower(int n) {
  Guard g;
  if (n == 0)
    throw MyError(42);
  thrower(n - 1);
}

static int rethrow() {
  try {
    try {
      thrower(2);
    } catch (...) {
      throw;
    }
  } catch (const MyError &e) {
    return e.code;
  }
  return 0;
}

struct Base {
  virtual ~Base() = default;
};
struct Derived : Base {
  int x = 7;
};

static int ctors, dtors;
struct PerThread {
  int value = 0;
  PerThread() { ctors++; }
  ~PerThread() { dtors++; }
};
static thread_local PerThread per_thread;

int main() {
  destroyed = 0;
  int code = 0;
  try {
    thrower(3);
  } catch (const MyError &e) {
    code = e.code;
  }
  check("throw and catch", code == 42);
  check("destructors during unwinding", destroyed == 4);
  check("rethrow", rethrow() == 42);

  bool caught = false;
  try {
    std::vector<int> v(2);
    (void)v.at(5);
  } catch (const std::out_of_range &) {
    caught = true;
  }
  check("std::out_of_range from vector::at", caught);

  caught = false;
  try {
    throw 3.5;
  } catch (int) {
  } catch (double d) {
    caught = d == 3.5;
  }
  check("catch by type", caught);

  std::unique_ptr<Base> b(new Derived);
  Derived *d = dynamic_cast<Derived *>(b.get());
  check("dynamic_cast and typeid",
        d && d->x == 7 && typeid(*b) == typeid(Derived));

  std::map<std::string, int> m;
  for (const char *w : {"pear", "apple", "fig", "apple"})
    m[w]++;
  std::ostringstream os;
  for (auto &kv : m)
    os << kv.first << '=' << kv.second << ' ';
  check("map, string and ostringstream", os.str() == "apple=2 fig=1 pear=1 ");

  std::vector<int> v{5, 3, 9, 1};
  std::sort(v.begin(), v.end());
  check("sort", v == std::vector<int>({1, 3, 5, 9}));

  long total = 0;
  std::mutex mu;
  std::vector<std::thread> threads;
  for (int i = 1; i <= 4; i++)
    threads.emplace_back([&, i] {
      for (int j = 0; j < 1000; j++) {
        std::lock_guard<std::mutex> lock(mu);
        total += i;
      }
    });
  for (auto &t : threads)
    t.join();
  check("std::thread and std::mutex", total == 10000);

  std::condition_variable cv;
  int stage = 0;
  std::thread waiter([&] {
    std::unique_lock<std::mutex> lock(mu);
    cv.wait(lock, [&] { return stage == 1; });
    stage = 2;
    cv.notify_one();
  });
  {
    std::unique_lock<std::mutex> lock(mu);
    stage = 1;
    cv.notify_one();
    cv.wait(lock, [&] { return stage == 2; });
  }
  waiter.join();
  check("std::condition_variable", stage == 2);

  std::atomic<int> caught_in_threads{0};
  threads.clear();
  for (int i = 0; i < 4; i++)
    threads.emplace_back([&] {
      for (int j = 0; j < 50; j++) {
        try {
          thrower(j % 4);
        } catch (const MyError &e) {
          if (e.code == 42)
            caught_in_threads++;
        }
        std::this_thread::yield();
      }
    });
  for (auto &t : threads)
    t.join();
  check("exceptions in four threads at once", caught_in_threads == 200);

  per_thread.value = 1;
  std::thread([] { per_thread.value = 5; }).join();
  check("thread_local objects", per_thread.value == 1 && ctors == 2 &&
                                    dtors == 1);

  auto t0 = std::chrono::steady_clock::now();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - t0)
                .count();
  check("sleep_for and steady_clock", ms >= 90 && ms < 1000);

  std::cout << "iostream: " << 12345 << ' ' << std::hex << 255 << std::endl;
  std::printf("%s\n", failed ? "FAILED" : "all ok");
  return failed != 0;
}
