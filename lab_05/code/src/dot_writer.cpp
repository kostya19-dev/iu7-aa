#include "dot_writer.h"

#include <sstream>

static const char *PALETTE[] = {
    "red",          "green",       "blue",    "orange", "purple",     "cyan",
    "gold",         "brown",       "pink",    "gray",   "chartreuse", "coral",
    "darkseagreen", "deepskyblue", "magenta", "navy"};

static int palette_size() {
  return (int)(sizeof(PALETTE) / sizeof(PALETTE[0]));
}

static bool is_simple_id(const std::string &s) {
  if (s.empty())
    return false;
  for (char c : s) {
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') || (c == '_')))
      return false;
  }
  return true;
}

static std::string escape_id(const std::string &s) {
  if (is_simple_id(s))
    return s;

  std::string out = "\"";
  for (char c : s) {
    if (c == '"')
      out += "\\\"";
    else
      out += c;
  }
  out += "\"";
  return out;
}

std::string build_colored_dot(const Graph &g, const ComponentsResult &cc) {
  std::ostringstream out;
  out << "graph G {\n";
  out << "  node [style=filled];\n";

  for (int i = 0; i < g.n; ++i) {
    int cid = (i < (int)cc.comp_id.size() ? cc.comp_id[i] : 0);
    const char *color = PALETTE[cid % palette_size()];

    std::string name =
        (i < (int)g.names.size() ? g.names[i] : std::to_string(i));
    out << "  " << escape_id(name) << " [fillcolor=" << color << "];\n";
  }

  for (auto e : g.edges) {
    std::string a = (e.first < (int)g.names.size() ? g.names[e.first]
                                                   : std::to_string(e.first));
    std::string b = (e.second < (int)g.names.size() ? g.names[e.second]
                                                    : std::to_string(e.second));
    out << "  " << escape_id(a) << " -- " << escape_id(b) << ";\n";
  }

  out << "}\n";
  return out.str();
}
