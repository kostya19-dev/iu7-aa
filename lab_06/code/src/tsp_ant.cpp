#include "tsp_ant.h"

static double route_length(const Graph &g, const std::vector<int> &path) {
  double sum = 0.0;
  for (size_t i = 0; i + 1 < path.size(); ++i) {
    sum += g.w[path[i]][path[i + 1]];
  }
  return sum;
}

static int choose_next_city(int current, const std::vector<bool> &used,
                            const Graph &g,
                            const std::vector<std::vector<double>> &pher,
                            const std::vector<std::vector<double>> &heur,
                            const AntParams &params, std::mt19937 &gen,
                            std::uniform_real_distribution<double> &dist01) {
  int n = g.w.size();

  std::vector<int> candidates;
  candidates.reserve(n);
  for (int i = 0; i < n; ++i) {
    if (!used[i]) {
      candidates.push_back(i);
    }
  }

  std::vector<double> weights;
  weights.reserve(candidates.size());
  double sum_weights = 0.0;

  for (int v : candidates) {
    double t = pher[current][v];
    if (t < params.tau_min) {
      t = params.tau_min;
    }
    double e = heur[current][v];
    double w = std::pow(t, params.alpha) * std::pow(e, params.beta);
    weights.push_back(w);
    sum_weights += w;
  }

  if (candidates.empty()) {
    return current;
  }

  int next = candidates.back();

  if (sum_weights <= 0.0) {
    std::uniform_int_distribution<int> dist_int(
        0, static_cast<int>(candidates.size()) - 1);
    next = candidates[dist_int(gen)];
    return next;
  }

  double r = dist01(gen) * sum_weights;
  double acc = 0.0;
  for (size_t i = 0; i < candidates.size(); ++i) {
    acc += weights[i];
    if (r <= acc) {
      next = candidates[i];
      break;
    }
  }

  return next;
}

double ant_solve(const Graph &g, double max_time, const AntParams &params,
                 std::vector<int> &best_route) {
  int n = g.w.size();
  best_route.clear();

  if (n <= 1) {
    return std::numeric_limits<double>::infinity();
  }

  std::mt19937 gen(777);
  std::uniform_real_distribution<double> dist01(0.0, 1.0);

  std::vector<std::vector<double>> pher(n, std::vector<double>(n, params.tau0));
  std::vector<std::vector<double>> heur(n, std::vector<double>(n, 0.0));

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      if (i != j && g.w[i][j] > 0.0) {
        heur[i][j] = 1.0 / g.w[i][j];
      } else {
        heur[i][j] = 0.0;
      }
    }
  }

  double global_best = std::numeric_limits<double>::infinity();

  for (int iter = 0; iter < params.iters; ++iter) {
    std::vector<std::vector<int>> ant_paths(params.ants);
    std::vector<double> ant_lengths(params.ants,
                                    std::numeric_limits<double>::infinity());
    std::vector<bool> ant_ok(params.ants, false);

    for (int a = 0; a < params.ants; ++a) {
      std::vector<int> path;
      path.reserve(n + 1);
      std::vector<bool> used(n, false);

      int cur = 0;
      path.push_back(cur);
      used[cur] = true;

      for (int step = 1; step < n; ++step) {
        int next =
            choose_next_city(cur, used, g, pher, heur, params, gen, dist01);
        path.push_back(next);
        used[next] = true;
        cur = next;
      }

      path.push_back(0);

      double len = route_length(g, path);
      ant_paths[a] = path;
      ant_lengths[a] = len;

      if (len <= max_time) {
        ant_ok[a] = true;
        if (len < global_best) {
          global_best = len;
          best_route = path;
        }
      }
    }

    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        pher[i][j] *= (1.0 - params.rho);
        if (pher[i][j] < params.tau_min) {
          pher[i][j] = params.tau_min;
        }
      }
    }

    for (int a = 0; a < params.ants; ++a) {
      if (!ant_ok[a])
        continue;
      double len = ant_lengths[a];
      double delta = params.q / len;
      const std::vector<int> &path = ant_paths[a];
      for (size_t k = 0; k + 1 < path.size(); ++k) {
        int i = path[k];
        int j = path[k + 1];
        pher[i][j] += delta;
        pher[j][i] += delta;
      }
    }

    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        if (pher[i][j] < params.tau_min) {
          pher[i][j] = params.tau_min;
        }
      }
    }
  }

  return global_best;
}
