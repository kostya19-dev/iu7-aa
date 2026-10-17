#include "stats.h"

double max_dev(const std::vector<double> &vals, double opt) {
  double m = 0.0;
  for (double v : vals) {
    if (!std::isfinite(v))
      continue;
    double d = (v - opt) / opt * 100.0;
    if (d > m)
      m = d;
  }
  return m;
}

double med_dev(std::vector<double> vals, double opt) {
  std::vector<double> devs;
  devs.reserve(vals.size());
  for (double v : vals) {
    if (!std::isfinite(v))
      continue;
    devs.push_back((v - opt) / opt * 100.0);
  }
  if (devs.empty()) {
    return std::numeric_limits<double>::quiet_NaN();
  }
  std::sort(devs.begin(), devs.end());
  size_t m = devs.size() / 2;
  if (devs.size() % 2 == 1) {
    return devs[m];
  } else {
    return 0.5 * (devs[m - 1] + devs[m]);
  }
}
