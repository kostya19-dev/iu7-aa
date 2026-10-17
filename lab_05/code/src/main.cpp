#include "pipeline.h"

#include <iostream>
#include <string>
#include <vector>

static void print_menu() {
  std::cout << "\nМеню:\n";
  std::cout << "1) Замеры: последовательно (N = 25,50,75,100,125) -> "
               "tests/bench_seq.csv\n";
  std::cout << "2) Замеры: конвейер 3 ОУ + k в ОУ2 -> tests/bench_pipe3.csv\n";
  std::cout << "3) Замеры: конвейер 4 ОУ (с ОУ+) + k в ОУ2 -> "
               "tests/bench_pipe4.csv\n";
  std::cout << "4) Обработать первые N последовательно с ОУ+ (создать "
               "*_colored.dot)\n";
  std::cout << "5) Установить k (вспомогательные потоки ОУ2)\n";
  std::cout << "6) Установить путь к файлу заявок (requests.txt)\n";
  std::cout << "0) Выход\n";
  std::cout << "Выбор: ";
}

int main() {
  PipelineConfig cfg;
  cfg.k_helpers = 4;
  cfg.enable_ou_plus = false;
  cfg.output_suffix_txt = "_out.txt";
  cfg.output_suffix_dot = "_colored.dot";

  std::string list_file = "requests.txt";
  std::vector<int> Ns = {25, 50, 75, 100, 125};

  for (;;) {
    print_menu();

    int cmd = -1;
    if (!(std::cin >> cmd))
      return 0;
    if (cmd == 0)
      return 0;

    if (cmd == 5) {
      std::cout << "Введите k (>=1): ";
      int k = 1;
      std::cin >> k;
      if (k < 1)
        k = 1;
      cfg.k_helpers = k;
      std::cout << "k = " << cfg.k_helpers << "\n";
      continue;
    }

    if (cmd == 6) {
      std::cout << "Введите путь к файлу заявок: ";
      std::cin >> list_file;
      std::cout << "Файл заявок: " << list_file << "\n";
      continue;
    }

    auto all = load_requests_list(list_file);
    if (all.empty()) {
      std::cout << "Не удалось прочитать заявки из: " << list_file << "\n";
      continue;
    }

    if (cmd == 1) {
      PipelineConfig local = cfg;
      local.enable_ou_plus = false;
      auto rows = bench_sequential(all, local, Ns);
      write_benchmark_csv("tests/bench_seq.csv", rows);
      std::cout << "Сохранено: tests/bench_seq.csv\n";
      continue;
    }

    if (cmd == 2) {
      PipelineConfig local = cfg;
      local.enable_ou_plus = false;
      auto rows = bench_pipeline3(all, local, Ns);
      write_benchmark_csv("tests/bench_pipe3.csv", rows);
      std::cout << "Сохранено: tests/bench_pipe3.csv\n";
      continue;
    }

    if (cmd == 3) {
      PipelineConfig local = cfg;
      auto rows = bench_pipeline4(all, local, Ns);
      write_benchmark_csv("tests/bench_pipe4.csv", rows);
      std::cout << "Сохранено: tests/bench_pipe4.csv\n";
      continue;
    }

    if (cmd == 4) {
      std::cout << "Введите N: ";
      int N = 0;
      std::cin >> N;
      if (N < 0)
        N = 0;

      PipelineConfig local = cfg;
      local.enable_ou_plus = true;

      auto reqs = take_first_n(all, N);
      double t = run_sequential(reqs, local);

      std::vector<BenchmarkRow> rows;
      rows.push_back({"последовательно_с_ОУплюс", N, t});
      write_benchmark_csv("tests/bench_seq_plus.csv", rows);

      std::cout << "Готово. Сохранено: tests/bench_seq_plus.csv\n";
      continue;
    }

    std::cout << "Неизвестная команда.\n";
  }
}
