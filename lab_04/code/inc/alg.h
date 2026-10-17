#ifndef ALG_H
#define ALG_H

#include "graph.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>

bool conn_seq(const Graph &g);
bool conn_par(const Graph &g, int threads);

#endif
