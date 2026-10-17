#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

struct Graph {
  std::vector<std::vector<double>> w;
};

Graph read_graph_from_file(const std::string &filename);

#endif
