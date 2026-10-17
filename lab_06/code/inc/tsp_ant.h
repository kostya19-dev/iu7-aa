#ifndef TSP_ANT_H
#define TSP_ANT_H

#include "graph.h"
#include <cmath>
#include <limits>
#include <random>
#include <vector>

struct AntParams {
  int ants;
  int iters;
  double alpha;
  double beta;
  double rho;
  double q;
  double tau0;
  double tau_min;
};

double ant_solve(const Graph &g, double max_time, const AntParams &params,
                 std::vector<int> &best_route);

#endif
