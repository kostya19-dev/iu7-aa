#include "utils.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

static std::string slurp(std::ifstream &in) {
  std::ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

std::string read_text_file(const std::string &path) {
  std::ifstream in(path, std::ios::in | std::ios::binary);
  if (!in)
    throw std::runtime_error("Cannot open file for reading: " + path);
  return slurp(in);
}

void write_text_file(const std::string &path, const std::string &text) {
  std::ofstream out(path, std::ios::out | std::ios::binary);
  if (!out)
    throw std::runtime_error("Cannot open file for writing: " + path);
  out << text;
}

std::vector<std::string> read_lines_file(const std::string &path) {
  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("Cannot open list file: " + path);

  std::vector<std::string> lines;
  std::string s;
  while (std::getline(in, s)) {
    if (!s.empty() && s.back() == '\r')
      s.pop_back();
    if (!s.empty())
      lines.push_back(s);
  }
  return lines;
}

static bool is_sep(char c) { return c == '/' || c == '\\'; }

std::string path_dirname(const std::string &path) {
  for (int i = (int)path.size() - 1; i >= 0; --i) {
    if (is_sep(path[i])) {
      if (i == 0)
        return std::string(1, path[0]);
      return path.substr(0, (size_t)i);
    }
  }
  return ".";
}

std::string path_basename(const std::string &path) {
  for (int i = (int)path.size() - 1; i >= 0; --i) {
    if (is_sep(path[i])) {
      return path.substr((size_t)i + 1);
    }
  }
  return path;
}

std::string path_ext(const std::string &path) {
  std::string base = path_basename(path);
  auto pos = base.rfind('.');
  if (pos == std::string::npos)
    return "";
  return base.substr(pos);
}

std::string path_stem(const std::string &path) {
  std::string base = path_basename(path);
  auto pos = base.rfind('.');
  if (pos == std::string::npos)
    return base;
  return base.substr(0, pos);
}

std::string path_join(const std::string &a, const std::string &b) {
  if (a.empty())
    return b;
  if (b.empty())
    return a;
  if (is_sep(a.back()))
    return a + b;
  return a + "/" + b;
}

bool file_exists(const std::string &path) {
  std::ifstream f(path);
  return (bool)f;
}
