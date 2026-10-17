#ifndef GRAPH_H
#define GRAPH_H

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Graph {
  int n;
  bool directed;
  std::vector<std::vector<int>> adj;
};

Graph load_graph(const std::string &filename, bool directed);
Graph remove_vertex(const Graph &g, int k);
void print_graph(const Graph &g);

#endif
