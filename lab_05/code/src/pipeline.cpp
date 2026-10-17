#include "pipeline.h"
#include "blocking_queue.h"
#include "components.h"
#include "dot_parser.h"
#include "dot_writer.h"
#include "utils.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

static double cpu_ms() {
  return 1000.0 * (double)std::clock() / (double)CLOCKS_PER_SEC;
}

std::vector<RequestIn> load_requests_list(const std::string &list_file) {
  std::vector<RequestIn> reqs;
  auto lines = read_lines_file(list_file);
  for (auto &s : lines) {
    if (s.empty())
      continue;
    RequestIn r;
    r.path_dot = s;
    reqs.push_back(r);
  }
  return reqs;
}

std::vector<RequestIn> take_first_n(const std::vector<RequestIn> &all, int n) {
  if (n < 0)
    n = 0;
  if ((int)all.size() <= n)
    return all;
  return std::vector<RequestIn>(all.begin(), all.begin() + n);
}

static std::string make_out_path_txt(const std::string &src,
                                     const PipelineConfig &cfg) {
  std::string dir = path_dirname(src);
  std::string stem = path_stem(src);
  std::string outname = stem + cfg.output_suffix_txt;
  return path_join(dir, outname);
}

static std::string make_out_path_dot(const std::string &src,
                                     const PipelineConfig &cfg) {
  std::string dir = path_dirname(src);
  std::string stem = path_stem(src);
  std::string outname = stem + cfg.output_suffix_dot;
  return path_join(dir, outname);
}

static RequestParsed ou1_read(const RequestIn &in) {
  RequestParsed out;
  out.src_path = in.path_dot;
  out.dot_text = read_text_file(in.path_dot);
  return out;
}

static RequestComputed ou2_compute(const RequestParsed &in,
                                  const PipelineConfig &cfg) {
  RequestComputed out;
  out.src_path = in.src_path;
  out.g = parse_dot_graph(in.dot_text);
  out.cc = connected_components_parallel_dsu(out.g, cfg.k_helpers);
  out.dump_text.clear();
  return out;
}

static RequestComputed ou_plus_colorize(RequestComputed in) {
  in.colored_dot = build_colored_dot(in.g, in.cc);
  return in;
}

static void ou3_dump(const RequestComputed &in, const PipelineConfig &cfg) {
  std::string out_txt = make_out_path_txt(in.src_path, cfg);
  write_text_file(out_txt, in.dump_text);

  if (cfg.enable_ou_plus) {
    std::string out_dot = make_out_path_dot(in.src_path, cfg);
    write_text_file(out_dot, in.colored_dot);
  }
}

double run_sequential(const std::vector<RequestIn> &reqs,
                      const PipelineConfig &cfg) {
  double t0 = cpu_ms();

  for (auto &r : reqs) {
    try {
      auto a = ou1_read(r);
      auto b = ou2_compute(a, cfg);
      if (cfg.enable_ou_plus)
        b = ou_plus_colorize(std::move(b));
      ou3_dump(b, cfg);
    } catch (const std::exception &) {
    }
  }

  double t1 = cpu_ms();
  double elapsed = t1 - t0;

  std::cout << "time_ms=" << std::fixed << std::setprecision(3)
            << elapsed << "\n";

  return elapsed;
}

double run_pipeline_3ou(const std::vector<RequestIn> &reqs,
                        const PipelineConfig &cfg) {
  BlockingQueue<RequestIn> q1;
  BlockingQueue<RequestParsed> q2;
  BlockingQueue<RequestComputed> q3;

  for (auto &r : reqs)
    q1.push(r);
  q1.close();

  double t0 = cpu_ms();

  std::thread ou1([&]() {
    RequestIn r;
    while (q1.pop(r)) {
      try {
        q2.push(ou1_read(r));
      } catch (const std::exception &) {
      }
    }
    q2.close();
  });

  std::thread ou2([&]() {
    RequestParsed a;
    while (q2.pop(a)) {
      try {
        q3.push(ou2_compute(a, cfg));
      } catch (const std::exception &) {
      }
    }
    q3.close();
  });

  std::thread ou3([&]() {
    RequestComputed b;
    while (q3.pop(b)) {
      try {
        ou3_dump(b, cfg);
      } catch (const std::exception &) {
      }
    }
  });

  ou1.join();
  ou2.join();
  ou3.join();

  double t1 = cpu_ms();
  double elapsed = t1 - t0;

  std::cout << "[КОНВЕЙЕР_3ОУ] time_ms=" << std::fixed << std::setprecision(3)
            << elapsed << "\n";

  return elapsed;
}

double run_pipeline_4ou_with_plus(const std::vector<RequestIn> &reqs,
                                  const PipelineConfig &cfg) {
  PipelineConfig local = cfg;
  local.enable_ou_plus = true;

  BlockingQueue<RequestIn> q1;
  BlockingQueue<RequestParsed> q2;
  BlockingQueue<RequestComputed> q3;
  BlockingQueue<RequestComputed> q4;

  for (auto &r : reqs)
    q1.push(r);
  q1.close();

  double t0 = cpu_ms();

  std::thread ou1([&]() {
    RequestIn r;
    while (q1.pop(r)) {
      try {
        q2.push(ou1_read(r));
      } catch (const std::exception &) {
      }
    }
    q2.close();
  });

  std::thread ou2([&]() {
    RequestParsed a;
    while (q2.pop(a)) {
      try {
        q3.push(ou2_compute(a, local));
      } catch (const std::exception &) {
      }
    }
    q3.close();
  });

  std::thread ou_plus([&]() {
    RequestComputed b;
    while (q3.pop(b)) {
      try {
        q4.push(ou_plus_colorize(std::move(b)));
      } catch (const std::exception &) {
      }
    }
    q4.close();
  });

  std::thread ou3([&]() {
    RequestComputed b;
    while (q4.pop(b)) {
      try {
        ou3_dump(b, local);
      } catch (const std::exception &) {
      }
    }
  });

  ou1.join();
  ou2.join();
  ou_plus.join();
  ou3.join();

  double t1 = cpu_ms();
  double elapsed = t1 - t0;

  std::cout << "time_ms=" << std::fixed
            << std::setprecision(3) << elapsed << "\n";

  return elapsed;
}

std::vector<BenchmarkRow> bench_sequential(const std::vector<RequestIn> &all,
                                           const PipelineConfig &cfg,
                                           const std::vector<int> &Ns) {
  std::vector<BenchmarkRow> rows;
  for (int n : Ns) {
    auto reqs = take_first_n(all, n);
    double t = run_sequential(reqs, cfg);
    rows.push_back({"последовательно", n, t});
    std::cout << "[замер] последовательно N=" << n << " время_мс=" << std::fixed
              << std::setprecision(3) << t << "\n";
  }
  return rows;
}

std::vector<BenchmarkRow> bench_pipeline3(const std::vector<RequestIn> &all,
                                          const PipelineConfig &cfg,
                                          const std::vector<int> &Ns) {
  std::vector<BenchmarkRow> rows;
  for (int n : Ns) {
    auto reqs = take_first_n(all, n);
    double t = run_pipeline_3ou(reqs, cfg);
    rows.push_back({"конвейер_3ОУ", n, t});
    std::cout << "[замер] конвейер_3ОУ N=" << n << " время_мс=" << std::fixed
              << std::setprecision(3) << t << "\n";
  }
  return rows;
}

std::vector<BenchmarkRow> bench_pipeline4(const std::vector<RequestIn> &all,
                                          const PipelineConfig &cfg,
                                          const std::vector<int> &Ns) {
  std::vector<BenchmarkRow> rows;
  for (int n : Ns) {
    auto reqs = take_first_n(all, n);
    double t = run_pipeline_4ou_with_plus(reqs, cfg);
    rows.push_back({"конвейер_4ОУ_с_ОУплюс", n, t});
    std::cout << "[замер] конвейер_4ОУ_с_ОУплюс N=" << n
              << " время_мс=" << std::fixed << std::setprecision(3) << t
              << "\n";
  }
  return rows;
}

void write_benchmark_csv(const std::string &csv_path,
                         const std::vector<BenchmarkRow> &rows) {
  std::ostringstream ss;
  ss << "mode,N,time_ms\n";
  ss << std::fixed << std::setprecision(6);
  for (auto &r : rows) {
    ss << r.mode << "," << r.N << "," << r.time_ms << "\n";
  }
  write_text_file(csv_path, ss.str());
}
