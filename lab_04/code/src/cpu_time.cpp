#include "../inc/cpu_time.h"

#include <ctime>

static inline double ticks_to_ms(clock_t ticks) {
  return 1000.0 * static_cast<double>(ticks) /
         static_cast<double>(CLOCKS_PER_SEC);
}

double measure_execution_time(bool (*f)(const Graph &), const Graph &g) {
  clock_t t0 = clock();

  volatile bool ok = f(g);
  (void)ok;

  clock_t t1 = clock();

  double ms = ticks_to_ms(t1 - t0);
  return (ms < 0.0) ? 0.0 : ms;
}

double measure_execution_time(bool (*f)(const Graph &, int), const Graph &g,
                              int threads) {
  clock_t t0 = clock();

  volatile bool ok = f(g, threads);
  (void)ok;

  clock_t t1 = clock();

  double ms = ticks_to_ms(t1 - t0);
  return (ms < 0.0) ? 0.0 : ms;
}
