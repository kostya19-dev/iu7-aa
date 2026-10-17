#include "concurrent_dsu.h"

ConcurrentDSU::ConcurrentDSU(int n) : parent(n), rnk(n, 0) {
  for (int i = 0; i < n; ++i)
    parent[i] = i;
}

int ConcurrentDSU::find_set(int v) {
  while (parent[v] != v) {
    parent[v] = parent[parent[v]];
    v = parent[v];
  }
  return v;
}

void ConcurrentDSU::unite(int a, int b) {
  std::lock_guard<std::mutex> lk(m);

  a = find_set(a);
  b = find_set(b);
  if (a == b)
    return;

  if (rnk[a] < rnk[b])
    std::swap(a, b);
  parent[b] = a;
  if (rnk[a] == rnk[b])
    rnk[a]++;
}
