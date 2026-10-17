#include "../inc/alg.h"
#include "../inc/graph.h"

#include <check.h>

#include <cstdio>
#include <fstream>
#include <vector>

static Graph make_graph(int n, const std::vector<std::vector<int>> &adj,
                        bool directed = false) {
  Graph g;
  g.n = n;
  g.directed = directed;
  g.adj = adj;
  return g;
}

START_TEST(test_empty_graph) {
  Graph g = make_graph(0, {});
  ck_assert_int_eq(conn_seq(g), true);
  ck_assert_int_eq(conn_par(g, 1), true);
}
END_TEST

START_TEST(test_single_vertex) {
  Graph g = make_graph(1, {{}});
  ck_assert_int_eq(conn_seq(g), true);
  ck_assert_int_eq(conn_par(g, 2), true);
}
END_TEST

START_TEST(test_two_isolated_vertices) {
  Graph g = make_graph(2, {{}, {}});
  ck_assert_int_eq(conn_seq(g), false);
  ck_assert_int_eq(conn_par(g, 4), false);
}
END_TEST

START_TEST(test_connected_chain_seq_par) {
  Graph g = make_graph(4, {{1}, {0, 2}, {1, 3}, {2}});
  ck_assert_int_eq(conn_seq(g), true);
  ck_assert_int_eq(conn_par(g, 1), true);
  ck_assert_int_eq(conn_par(g, 4), true);
}
END_TEST

START_TEST(test_disconnected_two_components_seq_par) {
  Graph g = make_graph(4, {{1}, {0}, {3}, {2}});
  ck_assert_int_eq(conn_seq(g), false);
  ck_assert_int_eq(conn_par(g, 2), false);
}
END_TEST

START_TEST(test_conn_par_threads_leq0_fallback) {
  Graph g = make_graph(4, {{1}, {0}, {3}, {2}});
  ck_assert_int_eq(conn_par(g, 0), conn_seq(g));
  ck_assert_int_eq(conn_par(g, -5), conn_seq(g));
}
END_TEST

START_TEST(test_remove_vertex_changes_connectivity) {
  Graph g = make_graph(4, {{1}, {0, 2}, {1, 3}, {2}});
  ck_assert_int_eq(conn_seq(g), true);

  Graph r = remove_vertex(g, 1);
  ck_assert_int_eq(r.n, 3);
  ck_assert_int_eq(conn_seq(r), false);
  ck_assert_int_eq(conn_par(r, 4), false);
}
END_TEST

START_TEST(test_remove_vertex_out_of_range_returns_original) {
  Graph g = make_graph(3, {{1}, {0, 2}, {1}});
  Graph r1 = remove_vertex(g, -1);
  Graph r2 = remove_vertex(g, 3);

  ck_assert_int_eq(r1.n, g.n);
  ck_assert_int_eq(r2.n, g.n);
  ck_assert_int_eq(conn_seq(r1), conn_seq(g));
  ck_assert_int_eq(conn_seq(r2), conn_seq(g));
}
END_TEST

START_TEST(test_load_graph_undirected) {
  const char *fname = "tmp_graph_undirected.txt";
  {
    std::ofstream out(fname);
    out << "0 1\n";
    out << "1 2\n";
    out << "garbage line\n";
    out << "2 2\n";
  }

  Graph g = load_graph(fname, false);
  std::remove(fname);

  ck_assert_int_eq(g.n, 3);
  ck_assert(g.adj[0].size() >= 1);
  ck_assert(g.adj[1].size() >= 2);
  ck_assert_int_eq(conn_seq(g), true);
  ck_assert_int_eq(conn_par(g, 2), true);
}
END_TEST

START_TEST(test_load_graph_directed) {
  const char *fname = "tmp_graph_directed.txt";
  {
    std::ofstream out(fname);
    out << "0 1\n";
    out << "1 2\n";
  }

  Graph g = load_graph(fname, true);
  std::remove(fname);

  ck_assert_int_eq(g.n, 3);
  ck_assert_int_eq(conn_seq(g), true);
  ck_assert_int_eq(conn_par(g, 4), true);
}
END_TEST

Suite *graph_suite() {
  Suite *s = suite_create("GraphConnectivity");

  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_empty_graph);
  tcase_add_test(tc_core, test_single_vertex);
  tcase_add_test(tc_core, test_two_isolated_vertices);
  tcase_add_test(tc_core, test_connected_chain_seq_par);
  tcase_add_test(tc_core, test_disconnected_two_components_seq_par);
  tcase_add_test(tc_core, test_conn_par_threads_leq0_fallback);
  suite_add_tcase(s, tc_core);

  TCase *tc_graph = tcase_create("GraphOps");
  tcase_add_test(tc_graph, test_remove_vertex_changes_connectivity);
  tcase_add_test(tc_graph, test_remove_vertex_out_of_range_returns_original);
  tcase_add_test(tc_graph, test_load_graph_undirected);
  tcase_add_test(tc_graph, test_load_graph_directed);
  suite_add_tcase(s, tc_graph);

  return s;
}

int main() {
  Suite *s = graph_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int failed = srunner_ntests_failed(sr);

  srunner_free(sr);
  return failed ? 1 : 0;
}
