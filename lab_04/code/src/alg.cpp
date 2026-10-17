#include "../inc/alg.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class TerminationDetector {
  std::atomic<int> active_tasks_{0};
  std::mutex mtx_;
  std::condition_variable cv_;

public:
  void add_tasks(int n) {
    active_tasks_.fetch_add(n, std::memory_order_relaxed);
  }

  void task_completed() {
    if (active_tasks_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      std::lock_guard<std::mutex> lock(mtx_);
      cv_.notify_all();
    }
  }

  void wait_until_finished() {
    std::unique_lock<std::mutex> lock(mtx_);
    cv_.wait(lock, [&] {
      return active_tasks_.load(std::memory_order_acquire) == 0;
    });
  }
};

bool conn_seq(const Graph &g) {
  if (g.n == 0)
    return true;

  int start_node = 0;

  std::vector<unsigned char> visited(g.n, 0);
  std::queue<int> q;

  visited[start_node] = 1;
  q.push(start_node);

  int visited_count = 0;

  while (!q.empty()) {
    int u = q.front();
    q.pop();
    visited_count++;

    for (int v : g.adj[u]) {
      if (!visited[v]) {
        visited[v] = 1;
        q.push(v);
      }
    }
  }

  return visited_count == g.n;
}

bool conn_par(const Graph &g, int num_threads) {
  if (num_threads <= 0)
    return conn_seq(g);
  if (g.n == 0)
    return true;

  int start_node = 0;

  std::vector<std::atomic<unsigned char>> visited(g.n);
  for (int i = 0; i < g.n; ++i) {
    visited[i].store(0, std::memory_order_relaxed);
  }

  std::queue<int> global_queue;
  std::mutex queue_mutex;
  std::condition_variable cv_worker;
  std::atomic<bool> stop_flag{false};

  TerminationDetector terminator;

  visited[start_node].store(1, std::memory_order_relaxed);
  global_queue.push(start_node);
  terminator.add_tasks(1);

  auto worker_func = [&]() {
    std::vector<int> local_buffer;
    local_buffer.reserve(128);

    while (true) {
      int u = -1;

      {
        std::unique_lock<std::mutex> lock(queue_mutex);
        cv_worker.wait(lock, [&] {
          return !global_queue.empty() ||
                 stop_flag.load(std::memory_order_relaxed);
        });

        if (global_queue.empty()) {
          if (stop_flag.load(std::memory_order_relaxed)) {
            return;
          }
          continue;
        }

        u = global_queue.front();
        global_queue.pop();
      }

      for (int v : g.adj[u]) {
        unsigned char expected = 0;
        if (visited[v].load(std::memory_order_relaxed) == 0 &&
            visited[v].compare_exchange_strong(expected, 1,
                                               std::memory_order_relaxed)) {
          local_buffer.push_back(v);
        }
      }

      if (!local_buffer.empty()) {
        terminator.add_tasks((int)local_buffer.size());
        {
          std::lock_guard<std::mutex> lock(queue_mutex);
          for (int node : local_buffer) {
            global_queue.push(node);
          }
        }
        cv_worker.notify_all();
        local_buffer.clear();
      }

      terminator.task_completed();
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(num_threads);
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(worker_func);
  }

  terminator.wait_until_finished();

  stop_flag.store(true, std::memory_order_relaxed);
  cv_worker.notify_all();

  for (auto &t : threads) {
    if (t.joinable())
      t.join();
  }

  for (int i = 0; i < g.n; ++i) {
    if (visited[i].load(std::memory_order_relaxed) == 0) {
      return false;
    }
  }

  return true;
}
