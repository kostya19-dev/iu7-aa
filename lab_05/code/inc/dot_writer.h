#pragma once

#include "components.h"
#include "graph.h"
#include <string>

std::string build_colored_dot(const Graph &g, const ComponentsResult &cc);
