#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

template <class T> class BlockingQueue {
public:
  void push(T value) {
    {
      std::lock_guard<std::mutex> lk(m_);
      q_.push(std::move(value));
    }
    cv_.notify_one();
  }

  bool pop(T &out) {
    std::unique_lock<std::mutex> lk(m_);
    cv_.wait(lk, [&] { return closed_ || !q_.empty(); });
    if (q_.empty())
      return false;
    out = std::move(q_.front());
    q_.pop();
    return true;
  }

  void close() {
    {
      std::lock_guard<std::mutex> lk(m_);
      closed_ = true;
    }
    cv_.notify_all();
  }

private:
  std::queue<T> q_;
  std::mutex m_;
  std::condition_variable cv_;
  bool closed_ = false;
};
