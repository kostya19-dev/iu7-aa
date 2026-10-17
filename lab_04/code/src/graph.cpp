#include "../inc/graph.h"

struct Edge {
  int u;
  int v;
};

static int extract_two_integers(const std::string &s, int nums[2]) {
  int found = 0;
  bool in_num = false;
  int value = 0;

  for (int i = 0; i <= (int)s.size(); ++i) {
    char c = (i < (int)s.size()) ? s[i] : ' ';
    if (c >= '0' && c <= '9') {
      if (!in_num) {
        in_num = true;
        value = 0;
      }
      value = value * 10 + (c - '0');
    } else {
      if (in_num) {
        in_num = false;
        if (found < 2) {
          nums[found] = value;
          found++;
        }
        if (found == 2)
          break;
      }
    }
  }

  return found;
}

Graph load_graph(const std::string &filename, bool directed) {
  std::ifstream in(filename.c_str());
  if (!in) {
    std::cerr << "cannot open file\n";
    std::exit(1);
  }

  std::vector<Edge> edges;
  int maxv = -1;
  std::string line;

  while (std::getline(in, line)) {
    int nums[2];
    int c = extract_two_integers(line, nums);
    if (c != 2)
      continue;

    int u = nums[0];
    int v = nums[1];
    if (u < 0 || v < 0)
      continue;

    Edge e;
    e.u = u;
    e.v = v;
    edges.push_back(e);

    if (u > maxv)
      maxv = u;
    if (v > maxv)
      maxv = v;
  }

  Graph g;
  g.directed = directed;
  g.n = (maxv >= 0 ? maxv + 1 : 0);
  g.adj.assign(g.n, std::vector<int>());

  for (std::size_t i = 0; i < edges.size(); ++i) {
    int u = edges[i].u;
    int v = edges[i].v;
    if (u >= g.n || v >= g.n)
      continue;
    g.adj[u].push_back(v);
    if (!g.directed)
      g.adj[v].push_back(u);
  }

  return g;
}

Graph remove_vertex(const Graph &g, int k) {
  Graph r;
  if (k < 0 || k >= g.n)
    return g;

  r.directed = g.directed;
  r.n = g.n - 1;
  r.adj.assign(r.n, std::vector<int>());

  auto map_idx = [k](int v) -> int { return (v < k) ? v : (v - 1); };

  for (int u = 0; u < g.n; ++u) {
    if (u == k)
      continue;
    int nu = map_idx(u);

    for (int v : g.adj[u]) {
      if (v == k)
        continue;
      int nv = map_idx(v);
      r.adj[nu].push_back(nv);
    }
  }

  return r;
}

void print_graph(const Graph &g) {
  std::cout << "N: " << g.n << "\n";
  for (int i = 0; i < g.n; ++i) {
    std::cout << i << ": ";
    for (std::size_t j = 0; j < g.adj[i].size(); ++j) {
      std::cout << g.adj[i][j] << " ";
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}