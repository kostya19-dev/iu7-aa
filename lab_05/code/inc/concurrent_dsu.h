#pragma once

#include <mutex>
#include <vector>

struct ConcurrentDSU {
  std::vector<int> parent;
  std::vector<int> rnk;
  std::mutex m;

  explicit ConcurrentDSU(int n);

  int find_set(int v);
  void unite(int a, int b);
};
