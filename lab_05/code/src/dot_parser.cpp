#include "dot_parser.h"

#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>

static bool is_keyword(const std::string &tok) {
  return tok == "graph" || tok == "digraph" || tok == "strict" ||
         tok == "node" || tok == "edge" || tok == "subgraph";
}

static std::string strip_quotes(const std::string &s) {
  if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
    return s.substr(1, s.size() - 2);
  return s;
}

static void skip_line_comment(const std::string &t, size_t &i) {
  while (i < t.size() && t[i] != '\n')
    i++;
}

static void skip_block_comment(const std::string &t, size_t &i) {

  i += 2;
  while (i + 1 < t.size()) {
    if (t[i] == '*' && t[i + 1] == '/') {
      i += 2;
      return;
    }
    i++;
  }
}

static void skip_attr_block(const std::string &t, size_t &i) {

  int depth = 0;
  while (i < t.size()) {
    char c = t[i];

    if (c == '"') {

      i++;
      while (i < t.size()) {
        if (t[i] == '\\' && i + 1 < t.size()) {
          i += 2;
          continue;
        }
        if (t[i] == '"') {
          i++;
          break;
        }
        i++;
      }
      continue;
    }

    if (c == '[')
      depth++;
    if (c == ']') {
      depth--;
      i++;
      if (depth <= 0)
        return;
      continue;
    }
    i++;
  }
}

static bool is_id_start(char c) {
  return std::isalnum((unsigned char)c) || c == '_' || c == '"';
}

static std::string read_id(const std::string &t, size_t &i) {

  if (i >= t.size())
    return "";

  if (t[i] == '"') {
    size_t start = i;
    i++; // skip opening "
    while (i < t.size()) {
      if (t[i] == '\\' && i + 1 < t.size()) {
        i += 2;
        continue;
      }
      if (t[i] == '"') {
        i++;
        break;
      }
      i++;
    }
    return t.substr(start, i - start);
  }

  size_t start = i;
  while (i < t.size()) {
    char c = t[i];
    if (!(std::isalnum((unsigned char)c) || c == '_'))
      break;
    i++;
  }
  return t.substr(start, i - start);
}

static int get_id(std::unordered_map<std::string, int> &mp,
                  std::vector<std::string> &names,
                  const std::string &name_raw) {
  std::string name = strip_quotes(name_raw);
  auto it = mp.find(name);
  if (it != mp.end())
    return it->second;

  int id = (int)mp.size();
  mp[name] = id;
  if ((int)names.size() <= id)
    names.resize((size_t)id + 1);
  names[(size_t)id] = name;
  return id;
}

Graph parse_dot_graph(const std::string &dot_text) {
  std::unordered_map<std::string, int> ids;
  std::vector<std::string> names;
  std::vector<std::pair<int, int>> edges;
  edges.reserve(256);

  bool skip_next_graph_name = false;

  size_t i = 0;
  while (i < dot_text.size()) {

    if (dot_text[i] == '/' && i + 1 < dot_text.size()) {
      if (dot_text[i + 1] == '/') {
        skip_line_comment(dot_text, i);
        continue;
      }
      if (dot_text[i + 1] == '*') {
        skip_block_comment(dot_text, i);
        continue;
      }
    }

    if (dot_text[i] == '[') {
      skip_attr_block(dot_text, i);
      continue;
    }

    if (!is_id_start(dot_text[i])) {
      i++;
      continue;
    }

    size_t j = i;
    std::string tok1 = read_id(dot_text, j);
    if (tok1.empty()) {
      i++;
      continue;
    }

    std::string plain1 = strip_quotes(tok1);

    if (is_keyword(plain1)) {
      if (plain1 == "graph" || plain1 == "digraph" || plain1 == "subgraph")
        skip_next_graph_name = true;
      i = j;
      continue;
    }

    if (skip_next_graph_name) {

      skip_next_graph_name = false;
      i = j;
      continue;
    }

    int u = get_id(ids, names, tok1);

    size_t k = j;
    while (k < dot_text.size() && std::isspace((unsigned char)dot_text[k]))
      k++;

    bool is_edge = false;
    if (k + 1 < dot_text.size()) {
      if (dot_text[k] == '-' && dot_text[k + 1] == '-') {
        is_edge = true;
        k += 2;
      } else if (dot_text[k] == '-' && dot_text[k + 1] == '>') {
        is_edge = true;
        k += 2;
      }
    }

    if (is_edge) {
      while (k < dot_text.size() && std::isspace((unsigned char)dot_text[k]))
        k++;

      if (k < dot_text.size() && is_id_start(dot_text[k])) {
        std::string tok2 = read_id(dot_text, k);
        std::string plain2 = strip_quotes(tok2);

        if (!tok2.empty() && !is_keyword(plain2)) {
          int v = get_id(ids, names, tok2);
          edges.push_back({u, v});
        }
      }

      i = k;
    } else {

      i = j;
    }
  }

  Graph g;
  g.n = (int)ids.size();
  g.names = std::move(names);
  g.edges = std::move(edges);
  return g;
}
