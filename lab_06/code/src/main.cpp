#include "graph.h"
#include "stats.h"
#include "tsp_ant.h"
#include "tsp_bruteforce.h"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

// печать списка графа в формате, близком к примеру
static void print_graph_list(const Graph &g) {
  int n = static_cast<int>(g.w.size());
  std::cout << "\nГраф загружен:\n";
  std::cout << "Список графа\n";

  for (int i = 0; i < n; ++i) {
    std::cout << i << " -> ";
    bool first = true;
    for (int j = 0; j < n; ++j) {
      if (i == j)
        continue;
      double w = g.w[i][j];
      if (w <= 0.0)
        continue;
      if (!first)
        std::cout << ' ';
      // веса у нас время, в файлах целые — выводим как целое
      std::cout << j << "(" << static_cast<int>(w) << ")";
      first = false;
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

static void run_single(const Graph &g) {
  int n = static_cast<int>(g.w.size());
  const double MAX_TIME = 80.0;
  const int RUNS = 10;

  std::cout << std::fixed << std::setprecision(2);
  std::cout << "Режим 1: решение задачи для исходного графа\n";
  std::cout << "Граф, n = " << n << "\n";

  std::vector<int> best_brute_route;
  std::clock_t start_brute = std::clock();
  double opt = brute_solve(g, MAX_TIME, best_brute_route);
  std::clock_t end_brute = std::clock();

  double brute_time_ms =
      1000.0 * static_cast<double>(end_brute - start_brute) / CLOCKS_PER_SEC;

  if (!std::isfinite(opt)) {
    std::cout << "  Нет допустимого маршрута (<= 80).\n";
    std::cout << "  Время полного перебора (CPU): " << brute_time_ms << " мс\n";
    return;
  }

  std::cout << "  Эталонная длина (полный перебор): " << opt << "\n";
  std::cout << "  Время полного перебора (CPU): " << brute_time_ms << " мс\n";

  AntParams p;
  p.ants = n;
  p.iters = 50;
  p.alpha = 1.0;
  p.beta = 3.0;
  p.rho = 0.5;
  p.q = 100.0;
  p.tau0 = 1.0;
  p.tau_min = 0.01;

  std::vector<double> results;
  results.reserve(RUNS);

  std::clock_t start_ant = std::clock();

  for (int k = 0; k < RUNS; ++k) {
    std::vector<int> best_ant_route;
    double best_len = ant_solve(g, MAX_TIME, p, best_ant_route);
    results.push_back(best_len);
  }

  std::clock_t end_ant = std::clock();

  double ant_time_total_ms =
      1000.0 * static_cast<double>(end_ant - start_ant) / CLOCKS_PER_SEC;
  double ant_time_avg_ms = ant_time_total_ms / RUNS;

  double dmax = max_dev(results, opt);
  double dmed = med_dev(results, opt);

  std::cout << "  Муравьиный алгоритм (alpha=" << p.alpha << ", beta=" << p.beta
            << ", rho=" << p.rho << ")\n";
  std::cout << "    max dev = " << dmax << " %\n";
  std::cout << "    med dev = " << dmed << " %\n";
  std::cout << "    среднее CPU-время одного запуска: " << ant_time_avg_ms
            << " мс\n";
}

static void run_research(const Graph &full) {
  int full_n = static_cast<int>(full.w.size());
  const double MAX_TIME = 80.0;
  const int RUNS = 10;

  std::vector<double> alpha_vals = {1.0, 2.0};
  std::vector<double> beta_vals = {2.0, 3.0};
  std::vector<double> rho_vals = {0.3, 0.5};

  std::ofstream time_file("time_results.csv");
  if (!time_file) {
    std::cerr << "Error: cannot open time_results.csv for writing\n";
    return;
  }
  time_file << "n,method,alpha,beta,rho,time_ms\n";

  std::cout << std::fixed << std::setprecision(2);
  std::cout << "Режим 2: исследование по числу городов и параметрам "
               "муравьиного алгоритма\n";

  for (int sz = 4; sz <= full_n; ++sz) {
    Graph g;
    g.w.assign(sz, std::vector<double>(sz, 0.0));
    for (int i = 0; i < sz; ++i) {
      for (int j = 0; j < sz; ++j) {
        g.w[i][j] = full.w[i][j];
      }
    }

    std::cout << "Граф, n = " << g.w.size() << "\n";

    std::vector<int> best_brute_route;
    std::clock_t start_brute = std::clock();
    double opt = brute_solve(g, MAX_TIME, best_brute_route);
    std::clock_t end_brute = std::clock();

    double brute_time_ms =
        1000.0 * static_cast<double>(end_brute - start_brute) / CLOCKS_PER_SEC;

    if (!std::isfinite(opt)) {
      std::cout << "  Нет допустимого маршрута (<= 80).\n";
      time_file << g.w.size() << ",bruteforce,0,0,0," << brute_time_ms << "\n";
      continue;
    }

    std::cout << "  Эталонная длина (полный перебор): " << opt << "\n";
    std::cout << "  Время полного перебора (CPU): " << brute_time_ms << " мс\n";

    time_file << g.w.size() << ",bruteforce,0,0,0," << brute_time_ms << "\n";

    for (double a : alpha_vals) {
      for (double b : beta_vals) {
        for (double r : rho_vals) {
          AntParams p;
          p.ants = g.w.size();
          p.iters = 50;
          p.alpha = a;
          p.beta = b;
          p.rho = r;
          p.q = 100.0;
          p.tau0 = 1.0;
          p.tau_min = 0.01;

          std::vector<double> results;
          results.reserve(RUNS);

          std::clock_t start_ant = std::clock();

          for (int k = 0; k < RUNS; ++k) {
            std::vector<int> best_ant_route;
            double best_len = ant_solve(g, MAX_TIME, p, best_ant_route);
            results.push_back(best_len);
          }

          std::clock_t end_ant = std::clock();

          double ant_time_total_ms = 1000.0 *
                                     static_cast<double>(end_ant - start_ant) /
                                     CLOCKS_PER_SEC;
          double ant_time_avg_ms = ant_time_total_ms / RUNS;

          double dmax = max_dev(results, opt);
          double dmed = med_dev(results, opt);

          std::cout << "  alpha=" << a << " beta=" << b << " rho=" << r
                    << "  max dev=" << dmax << " %" << "  med dev=" << dmed
                    << " %" << "  avg CPU time=" << ant_time_avg_ms << " мс\n";

          time_file << g.w.size() << ",ant," << a << "," << b << "," << r << ","
                    << ant_time_avg_ms << "\n";
        }
      }
    }

    std::cout << "\n";
  }

  std::cout
      << "Результаты временных замеров сохранены в файл time_results.csv\n";
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <graph_file>\n";
    return 1;
  }

  Graph full = read_graph_from_file(argv[1]);
  if (full.w.size() == 0) {
    std::cerr << "Error: failed to read graph\n";
    return 1;
  }

  // вывод списка графа после загрузки
  print_graph_list(full);

  while (true) {
    std::cout << "\nМеню:\n";
    std::cout << "  1 --- решить задачу для исходного графа\n";
    std::cout << "  2 --- выполнить исследование (параметризация и замеры по "
                 "числу городов)\n";
    std::cout << "  0 --- выход\n";
    std::cout << "Ваш выбор: ";

    int choice;
    if (!(std::cin >> choice)) {
      break;
    }

    if (choice == 0) {
      break;
    } else if (choice == 1) {
      run_single(full);
    } else if (choice == 2) {
      run_research(full);
    } else {
      std::cout << "Некорректный пункт меню.\n";
    }
  }

  return 0;
}
