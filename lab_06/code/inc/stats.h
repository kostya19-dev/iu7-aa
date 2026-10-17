#ifndef STATS_H
#define STATS_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

double max_dev(const std::vector<double> &vals, double opt);
double med_dev(std::vector<double> vals, double opt);

#endif
