#include "graph.h"
#include <fstream>
#include <iostream>

Graph read_graph_from_file(const std::string &filename) {
  Graph g;

  std::ifstream in(filename);
  if (!in) {
    std::cerr << "Error: cannot open file " << filename << "\n";
    return g;
  }

  int n;
  if (!(in >> n) || n <= 0) {
    std::cerr << "Error: incorrect n in file " << filename << "\n";
    return g;
  }

  g.w.assign(n, std::vector<double>(n, 0.0));

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      double val;
      if (!(in >> val)) {
        std::cerr << "Error: not enough data in file " << filename << "\n";
        g.w.clear();
        return g;
      }
      g.w[i][j] = val;
    }
  }

  return g;
}
