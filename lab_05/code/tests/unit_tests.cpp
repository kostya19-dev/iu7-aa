#include <check.h>

#include <string>
#include <vector>

#include "components.h"
#include "dot_parser.h"
#include "dot_writer.h"

static int find_v(const Graph &g, const std::string &name) {
  for (int i = 0; i < (int)g.names.size(); ++i) {
    if (g.names[i] == name)
      return i;
  }
  return -1;
}

static bool same_partition(const ComponentsResult &a,
                           const ComponentsResult &b) {
  if (a.comp_id.size() != b.comp_id.size())
    return false;

  int n = (int)a.comp_id.size();
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      bool ea = (a.comp_id[i] == a.comp_id[j]);
      bool eb = (b.comp_id[i] == b.comp_id[j]);
      if (ea != eb)
        return false;
    }
  }
  return true;
}

START_TEST(test_parse_basic) {
  std::string dot = "graph G {\n"
                    "  a -- b;\n"
                    "  b -- c;\n"
                    "  d;\n"
                    "}\n";

  Graph g = parse_dot_graph(dot);

  ck_assert_int_eq(g.n, 4);
  ck_assert_int_eq((int)g.edges.size(), 2);

  ck_assert_int_ne(find_v(g, "a"), -1);
  ck_assert_int_ne(find_v(g, "b"), -1);
  ck_assert_int_ne(find_v(g, "c"), -1);
  ck_assert_int_ne(find_v(g, "d"), -1);

  ck_assert_int_eq(find_v(g, "G"), -1);
  ck_assert_int_eq(find_v(g, "graph"), -1);
}
END_TEST

START_TEST(test_parse_quotes_attrs) {
  std::string dot = "digraph G {\n"
                    "  node [shape=circle];\n"
                    "  \"node 1\" -> \"node 2\" [label=\"x\"];\n"
                    "  \"node 3\";\n"
                    "}\n";

  Graph g = parse_dot_graph(dot);

  ck_assert_int_eq(g.n, 3);
  ck_assert_int_eq((int)g.edges.size(), 1);

  ck_assert_int_ne(find_v(g, "node 1"), -1);
  ck_assert_int_ne(find_v(g, "node 2"), -1);
  ck_assert_int_ne(find_v(g, "node 3"), -1);

  ck_assert_int_eq(find_v(g, "node"), -1);
  ck_assert_int_eq(find_v(g, "shape"), -1);
  ck_assert_int_eq(find_v(g, "circle"), -1);
  ck_assert_int_eq(find_v(g, "label"), -1);
}
END_TEST

START_TEST(test_components_seq_two_components) {
  std::string dot = "graph G {\n"
                    "  0 -- 1;\n"
                    "  1 -- 2;\n"
                    "  3 -- 4;\n"
                    "}\n";

  Graph g = parse_dot_graph(dot);
  ComponentsResult cc = connected_components_sequential(g);

  ck_assert_int_eq(cc.comp_count, 2);
  ck_assert_int_eq((int)cc.comp_id.size(), g.n);

  int v0 = find_v(g, "0");
  int v1 = find_v(g, "1");
  int v2 = find_v(g, "2");
  int v3 = find_v(g, "3");
  int v4 = find_v(g, "4");

  ck_assert_int_ne(v0, -1);
  ck_assert_int_ne(v1, -1);
  ck_assert_int_ne(v2, -1);
  ck_assert_int_ne(v3, -1);
  ck_assert_int_ne(v4, -1);

  ck_assert_int_eq(cc.comp_id[v0], cc.comp_id[v1]);
  ck_assert_int_eq(cc.comp_id[v1], cc.comp_id[v2]);
  ck_assert_int_ne(cc.comp_id[v0], cc.comp_id[v3]);
  ck_assert_int_eq(cc.comp_id[v3], cc.comp_id[v4]);
}
END_TEST

START_TEST(test_components_parallel_equivalent_to_seq) {
  std::string dot = "graph G {\n"
                    "  a -- b;\n"
                    "  b -- c;\n"
                    "  d -- e;\n"
                    "  f;\n"
                    "}\n";

  Graph g = parse_dot_graph(dot);

  ComponentsResult s = connected_components_sequential(g);
  ComponentsResult p1 = connected_components_parallel_dsu(g, 1);
  ComponentsResult p4 = connected_components_parallel_dsu(g, 4);

  ck_assert_int_eq((int)s.comp_id.size(), g.n);
  ck_assert_int_eq((int)p1.comp_id.size(), g.n);
  ck_assert_int_eq((int)p4.comp_id.size(), g.n);

  ck_assert_int_eq(s.comp_count, p1.comp_count);
  ck_assert_int_eq(s.comp_count, p4.comp_count);

  ck_assert_int_eq(same_partition(s, p1), true);
  ck_assert_int_eq(same_partition(s, p4), true);
}
END_TEST

START_TEST(test_colored_dot_output) {
  std::string dot = "graph G {\n"
                    "  a -- b;\n"
                    "  c;\n"
                    "}\n";

  Graph g = parse_dot_graph(dot);
  ComponentsResult cc = connected_components_sequential(g);

  std::string out = build_colored_dot(g, cc);

  ck_assert_int_ne((int)out.find("graph G"), -1);
  ck_assert_int_ne((int)out.find("fillcolor="), -1);
}
END_TEST

Suite *suite_main() {
  Suite *s = suite_create("PipelineDOT");

  TCase *tc = tcase_create("Core");
  tcase_add_test(tc, test_parse_basic);
  tcase_add_test(tc, test_parse_quotes_attrs);
  tcase_add_test(tc, test_components_seq_two_components);
  tcase_add_test(tc, test_components_parallel_equivalent_to_seq);
  tcase_add_test(tc, test_colored_dot_output);
  suite_add_tcase(s, tc);

  return s;
}

int main() {
  Suite *s = suite_main();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int failed = srunner_ntests_failed(sr);

  srunner_free(sr);
  return failed ? 1 : 0;
}
