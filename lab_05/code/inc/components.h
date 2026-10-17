#pragma once

#include "graph.h"
#include <vector>

struct ComponentsResult {
  std::vector<int> comp_id;
  int comp_count = 0;
};

ComponentsResult connected_components_sequential(const Graph &g);
ComponentsResult connected_components_parallel_dsu(const Graph &g,
                                                   int k_helpers);
