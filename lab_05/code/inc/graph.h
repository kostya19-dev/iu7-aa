#pragma once

#include <string>
#include <utility>
#include <vector>

struct Graph {
  int n = 0;
  std::vector<std::string> names;
  std::vector<std::pair<int, int>> edges;
};
