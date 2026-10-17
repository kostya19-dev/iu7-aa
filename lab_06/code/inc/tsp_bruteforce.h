#ifndef TSP_BRUTEFORCE_H
#define TSP_BRUTEFORCE_H

#include "graph.h"
#include <algorithm>
#include <limits>
#include <numeric>
#include <vector>

double brute_solve(const Graph &g, double max_time,
                   std::vector<int> &best_route);

#endif
