#pragma once

#include <string>
#include <vector>

#include "components.h"
#include "graph.h"

struct RequestIn {
  std::string path_dot;
};

struct RequestParsed {
  std::string src_path;
  std::string dot_text;
};

struct RequestComputed {
  std::string src_path;
  Graph g;
  ComponentsResult cc;
  std::string dump_text;
  std::string colored_dot;
};

struct PipelineConfig {
  int k_helpers = 4;
  bool enable_ou_plus = false;
  std::string output_suffix_txt = "_out.txt";
  std::string output_suffix_dot = "_colored.dot";
};

struct BenchmarkRow {
  std::string mode;
  int N = 0;
  double time_ms = 0.0;
};

std::vector<RequestIn> load_requests_list(const std::string &list_file);
std::vector<RequestIn> take_first_n(const std::vector<RequestIn> &all, int n);

double run_sequential(const std::vector<RequestIn> &reqs,
                      const PipelineConfig &cfg);
double run_pipeline_3ou(const std::vector<RequestIn> &reqs,
                        const PipelineConfig &cfg);
double run_pipeline_4ou_with_plus(const std::vector<RequestIn> &reqs,
                                  const PipelineConfig &cfg);

std::vector<BenchmarkRow> bench_sequential(const std::vector<RequestIn> &all,
                                           const PipelineConfig &cfg,
                                           const std::vector<int> &Ns);
std::vector<BenchmarkRow> bench_pipeline3(const std::vector<RequestIn> &all,
                                          const PipelineConfig &cfg,
                                          const std::vector<int> &Ns);
std::vector<BenchmarkRow> bench_pipeline4(const std::vector<RequestIn> &all,
                                          const PipelineConfig &cfg,
                                          const std::vector<int> &Ns);

void write_benchmark_csv(const std::string &csv_path,
                         const std::vector<BenchmarkRow> &rows);
