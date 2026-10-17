#ifndef CPU_TIME_H
#define CPU_TIME_H

#include "graph.h"

double measure_execution_time(bool (*f)(const Graph &), const Graph &g);
double measure_execution_time(bool (*f)(const Graph &, int), const Graph &g,
                              int threads);

#endif
