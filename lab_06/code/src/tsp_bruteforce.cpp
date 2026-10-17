#include "tsp_bruteforce.h"

double brute_solve(const Graph &g, double max_time,
                   std::vector<int> &best_route) {
  int n = g.w.size();
  std::vector<int> perm(n - 1);
  std::iota(perm.begin(), perm.end(), 1);
  double best = std::numeric_limits<double>::infinity();
  best_route.clear();
  do {
    double len = 0.0;
    int cur = 0;
    bool ok = true;
    for (int v : perm) {
      len += g.w[cur][v];
      if (len > max_time) {
        ok = false;
        break;
      }
      cur = v;
    }
    if (!ok)
      continue;
    len += g.w[cur][0];
    if (len <= max_time && len < best) {
      best = len;
      best_route.clear();
      best_route.push_back(0);
      for (int v : perm)
        best_route.push_back(v);
      best_route.push_back(0);
    }
  } while (std::next_permutation(perm.begin(), perm.end()));
  return best;
}
