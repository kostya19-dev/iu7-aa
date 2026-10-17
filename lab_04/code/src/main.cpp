#include "../inc/alg.h"
#include "../inc/graph.h"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

static std::string make_graph_path(const std::string &name) {
  if (name.find('/') != std::string::npos ||
      name.find('\\') != std::string::npos)
    return name;
  return "tests/" + name;
}

static bool file_exists(const std::string &path) {
  std::ifstream f(path.c_str());
  return (bool)f;
}

static Graph prefix_subgraph(const Graph &g, int n) {
  if (n > g.n)
    n = g.n;
  Graph r;
  r.n = n;
  r.directed = g.directed;
  r.adj.assign(n, std::vector<int>());

  for (int u = 0; u < n; ++u) {
    for (int v : g.adj[u]) {
      if (v >= 0 && v < n)
        r.adj[u].push_back(v);
    }
  }
  return r;
}

static Graph remove_vertex_compact(const Graph &g, int k) {
  if (g.n == 0)
    return g;
  if (k < 0)
    k = 0;
  if (k >= g.n)
    k = g.n - 1;

  std::vector<int> map_old_to_new(g.n, -1);
  int new_n = 0;
  for (int i = 0; i < g.n; ++i) {
    if (i == k)
      continue;
    map_old_to_new[i] = new_n++;
  }

  Graph r;
  r.n = new_n;
  r.directed = g.directed;
  r.adj.assign(new_n, std::vector<int>());

  for (int u = 0; u < g.n; ++u) {
    if (u == k)
      continue;
    int nu = map_old_to_new[u];
    for (int v : g.adj[u]) {
      if (v == k)
        continue;
      if (v < 0 || v >= g.n)
        continue;
      int nv = map_old_to_new[v];
      if (nv < 0)
        continue;
      r.adj[nu].push_back(nv);
    }
  }
  return r;
}

static std::vector<int> make_thread_values() {
  unsigned q = std::thread::hardware_concurrency();
  if (q == 0)
    q = 1;
  int max_k = static_cast<int>(8 * q);

  std::vector<int> vals;
  for (int k = 1; k <= max_k; k *= 2)
    vals.push_back(k);

  return vals;
}

static double measure_seq_ms(const Graph &g, int runs) {
  std::vector<double> times;
  times.reserve(runs);

  for (int r = 0; r < runs; ++r) {
    clock_t t0 = clock();
    volatile bool ok = conn_seq(g);
    (void)ok;
    clock_t t1 = clock();
    double ms = static_cast<double>(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;
    times.push_back(ms);
  }

  std::sort(times.begin(), times.end());
  return times[times.size() / 2];
}

static double measure_par_ms(const Graph &g, int threads, int runs) {
  std::vector<double> times;
  times.reserve(runs);

  for (int r = 0; r < runs; ++r) {
    clock_t t0 = clock();
    volatile bool ok = conn_par(g, threads);
    (void)ok;
    clock_t t1 = clock();
    double ms = static_cast<double>(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;
    times.push_back(ms);
  }

  std::sort(times.begin(), times.end());
  return times[times.size() / 2];
}
static std::vector<int> build_sizes(int total_n) {
  std::vector<int> sizes;
  if (total_n <= 0)
    return sizes;

  const int steps = 10;
  int step_val = total_n / steps;
  if (step_val < 100)
    step_val = 100;

  for (int s = step_val; s <= total_n; s += step_val)
    sizes.push_back(s);
  if (sizes.empty() || sizes.back() != total_n)
    sizes.push_back(total_n);

  std::sort(sizes.begin(), sizes.end());
  sizes.erase(std::unique(sizes.begin(), sizes.end()), sizes.end());
  return sizes;
}

static Graph generate_random_graph(int n, int max_deg = 15) {
  Graph g;
  g.n = n;
  g.directed = false;
  g.adj.assign(n, std::vector<int>());

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> deg_dist(0, max_deg);
  std::uniform_int_distribution<int> v_dist(0, n - 1);

  std::vector<int> deg(n, 0);

  std::unordered_set<unsigned long long> edges;
  edges.reserve(static_cast<size_t>(n) * 8);

  auto key_uv = [](int u, int v) -> unsigned long long {
    if (u > v)
      std::swap(u, v);
    return (static_cast<unsigned long long>(u) << 32) |
           static_cast<unsigned long long>(v);
  };

  for (int u = 0; u < n; ++u) {
    int want = deg_dist(rng);
    int tries = 0;
    while (deg[u] < want && tries < 100) {
      ++tries;
      int v = v_dist(rng);
      if (v == u)
        continue;
      if (deg[v] >= max_deg)
        continue;

      unsigned long long k = key_uv(u, v);
      if (edges.find(k) != edges.end())
        continue;

      edges.insert(k);
      ++deg[u];
      ++deg[v];
    }
  }

  for (auto k : edges) {
    int u = static_cast<int>(k >> 32);
    int v = static_cast<int>(k & 0xffffffffu);
    g.adj[u].push_back(v);
    g.adj[v].push_back(u);
  }

  return g;
}

static void save_graph_as_txt_edges(const Graph &g, const std::string &path) {
  std::ofstream out(path.c_str());
  for (int u = 0; u < g.n; ++u) {
    for (int v : g.adj[u]) {
      if (u < v)
        out << u << " " << v << "\n";
    }
  }
}

static void print_menu() {
  std::cout << "\n----------МЕНЮ----------\n";
  std::cout << "1. Последовательный алгоритм \n";
  std::cout << "2. Параллельный алгоритм \n";
  std::cout << "3. Получить замеры и сохранить CSV\n";
  std::cout << "4. Сгенерировать случайный граф (TXT)\n";
  std::cout << "5. Выход\n";
  std::cout << "Выберите пункт: ";
}

static void run_one_check(bool parallel) {
  std::string file;
  std::cout << "Файл (например graph.txt): ";
  std::cin >> file;

  std::string path = make_graph_path(file);
  if (!file_exists(path)) {
    std::cout << "Файл не найден: " << path << "\n";
    return;
  }

  Graph g = load_graph(path, false);

  if (g.n == 0) {
    std::cout << "Граф пуст.\n";
    return;
  }

  int k;
  std::cout << "Удаляемая вершина k (0.." << (g.n - 1) << "): ";
  std::cin >> k;

  Graph g2 = remove_vertex_compact(g, k);

  if (!parallel) {
    std::cout << (conn_seq(g2) ? "Связный\n" : "Не связный\n");
    return;
  }

  int t;
  std::cout << "Потоки (1,2,4,8,16,32,64): ";
  std::cin >> t;
  std::cout << (conn_par(g2, t) ? "Связный\n" : "Не связный\n");
}

static void run_bench_all_threads() {
  std::string file;
  std::cout << "Файл (например graph.txt): ";
  std::cin >> file;

  std::string path = make_graph_path(file);
  if (!file_exists(path)) {
    std::cout << "Файл не найден: " << path << "\n";
    return;
  }

  Graph g = load_graph(path, false);
  if (g.n == 0) {
    std::cout << "Граф пуст.\n";
    return;
  }

  int k;
  std::cout << "Удаляемая вершина k (0.." << (g.n - 1) << "): ";
  std::cin >> k;

  unsigned q = std::thread::hardware_concurrency();
  if (q == 0)
    q = 1;
  std::vector<int> threadValues = make_thread_values();

  std::cout << "q=" << q << ", замеры для потоков: ";
  for (size_t i = 0; i < threadValues.size(); ++i) {
    std::cout << threadValues[i] << (i + 1 == threadValues.size() ? "\n" : ",");
  }

  std::vector<int> sizes = build_sizes(g.n);

  const int runs = 7;

  std::vector<std::ofstream> csvs;
  csvs.reserve(threadValues.size());

  for (int t : threadValues) {
    std::string csv_file = "tests/results_" + std::to_string(t) + ".csv";
    csvs.emplace_back(csv_file.c_str(), std::ios::trunc);
    if (!csvs.back()) {
      std::cout << "Не удалось открыть " << csv_file << " для записи\n";
      return;
    }
    csvs.back() << "size,seq,par\n";
  }

  std::cout << "runs=" << runs << "\n";
  std::cout << "size\tseq_ms\tpar_ms(1)\t...\n";

  for (int n : sizes) {
    Graph gs = prefix_subgraph(g, n);

    int k_eff = k;
    if (k_eff < 0)
      k_eff = 0;
    if (k_eff >= n)
      k_eff = n - 1;

    Graph work = remove_vertex_compact(gs, k_eff);

    // прогрев
    conn_seq(work);
    conn_par(work, 1);

    double seqAvg = measure_seq_ms(work, runs);

    std::cout << n << "\t" << std::fixed << std::setprecision(3) << seqAvg;

    for (size_t i = 0; i < threadValues.size(); ++i) {
      int t = threadValues[i];

      conn_par(work, t);

      double parAvg = measure_par_ms(work, t, runs);

      csvs[i] << n << "," << seqAvg << "," << parAvg << "\n";

      if (t == 1) {
        std::cout << "\t" << std::fixed << std::setprecision(3) << parAvg;
      }
    }
    std::cout << "\n";
  }

  for (auto &f : csvs)
    f.close();

  std::cout << "\nГотово. CSV сохранены в папку tests/:\n";
  for (int t : threadValues) {
    std::cout << "  tests/results_" << t << ".csv\n";
  }
}

int main() {
  while (true) {
    print_menu();
    int c;
    if (!(std::cin >> c))
      return 0;

    if (c == 5)
      break;

    if (c == 1) {
      run_one_check(false);
    } else if (c == 2) {
      run_one_check(true);
    } else if (c == 3) {
      run_bench_all_threads();
    } else if (c == 4) {
      int n;
      std::string name;
      std::cout << "Количество вершин: ";
      std::cin >> n;
      std::cout << "Имя файла (например mygraph.txt): ";
      std::cin >> name;

      if (n <= 0) {
        std::cout << "n должно быть > 0\n";
        continue;
      }

      Graph g = generate_random_graph(n, 15);
      std::string out_path = "tests/" + name;
      save_graph_as_txt_edges(g, out_path);
      std::cout << "Сохранено в " << out_path << "\n";
    } else {
      std::cout << "Неизвестный пункт.\n";
    }
  }
  return 0;
}
