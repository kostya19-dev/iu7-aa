#include "components.h"
#include "concurrent_dsu.h"

#include <algorithm>
#include <queue>
#include <thread>
#include <unordered_map>

ComponentsResult connected_components_sequential(const Graph &g) {
  ComponentsResult res;
  res.comp_id.assign(g.n, -1);
  res.comp_count = 0;

  std::vector<std::vector<int>> adj(g.n);
  for (auto e : g.edges) {
    int u = e.first, v = e.second;
    if (u < 0 || v < 0 || u >= g.n || v >= g.n)
      continue;
    adj[u].push_back(v);
    adj[v].push_back(u);
  }

  for (int i = 0; i < g.n; ++i) {
    if (res.comp_id[i] != -1)
      continue;

    int cid = res.comp_count++;
    std::queue<int> q;
    q.push(i);
    res.comp_id[i] = cid;

    while (!q.empty()) {
      int u = q.front();
      q.pop();
      for (int v : adj[u]) {
        if (res.comp_id[v] == -1) {
          res.comp_id[v] = cid;
          q.push(v);
        }
      }
    }
  }

  return res;
}

ComponentsResult connected_components_parallel_dsu(const Graph &g,
                                                   int k_helpers) {
  ComponentsResult res;
  res.comp_id.assign(g.n, 0);
  res.comp_count = 0;

  if (g.n == 0)
    return res;
  // Если рёбер нет, то каждая вершина - отдельная компонента                                                  
  if (g.edges.empty()) {
    res.comp_count = g.n;
    for (int i = 0; i < g.n; ++i)
      res.comp_id[i] = i;
    return res;
  }

  if (k_helpers < 1)
    k_helpers = 1;

  ConcurrentDSU dsu(g.n);
  //делит ребра на k_helpers частей и обрабатывает их в отдельных потоках
  int m = (int)g.edges.size();
  int chunk = (m + k_helpers - 1) / k_helpers;

  //создает k_helpers потоков 
  std::vector<std::thread> threads;
  threads.reserve((size_t)k_helpers);
  for (int t = 0; t < k_helpers; ++t) {
    int l = t * chunk;
    int r = std::min(m, (t + 1) * chunk);
    if (l >= r)
      break;

    threads.emplace_back([&dsu, &g, l, r]() {
      for (int i = l; i < r; ++i) {
        int u = g.edges[i].first;
        int v = g.edges[i].second;
        if (u >= 0 && v >= 0)
          dsu.unite(u, v);
      }
    });
  }

  for (auto &th : threads)
    th.join();

  std::unordered_map<int, int> root_to_cid;
  root_to_cid.reserve((size_t)g.n);

  for (int i = 0; i < g.n; ++i) {
    int root = dsu.find_set(i);
    auto it = root_to_cid.find(root);
    if (it == root_to_cid.end()) {
      int cid = (int)root_to_cid.size();
      root_to_cid[root] = cid;
      res.comp_id[i] = cid;
    } else {
      res.comp_id[i] = it->second;
    }
  }

  res.comp_count = (int)root_to_cid.size();
  return res;
}
