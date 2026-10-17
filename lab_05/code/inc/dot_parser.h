#pragma once

#include "graph.h"
#include <string>

Graph parse_dot_graph(const std::string &dot_text);
