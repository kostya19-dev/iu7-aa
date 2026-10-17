#include <check.h>
#include <cmath>
#include <limits>
#include <random>
#include <vector>

#include "graph.h"
#include "tsp_ant.h"
#include "tsp_bruteforce.h"

START_TEST(test_bruteforce_simple_cycle) {
  Graph g;
  g.w.assign(4, std::vector<double>(4, 0.0));

  g.w[0][1] = g.w[1][0] = 1.0;
  g.w[1][2] = g.w[2][1] = 1.0;
  g.w[2][3] = g.w[3][2] = 1.0;
  g.w[3][0] = g.w[0][3] = 1.0;

  double big = 100.0;
  g.w[0][2] = g.w[2][0] = big;
  g.w[1][3] = g.w[3][1] = big;

  std::vector<int> best_route;
  double res = brute_solve(g, 80.0, best_route);

  ck_assert_double_eq_tol(res, 4.0, 1e-6);
  ck_assert_int_eq((int)best_route.size(), 5);
  ck_assert_int_eq(best_route.front(), 0);
  ck_assert_int_eq(best_route.back(), 0);
}
END_TEST

START_TEST(test_bruteforce_two_cycles) {
  Graph g;
  g.w.assign(4, std::vector<double>(4, 0.0));

  g.w[0][1] = g.w[1][0] = 1.0;
  g.w[1][2] = g.w[2][1] = 2.0;
  g.w[2][3] = g.w[3][2] = 3.0;
  g.w[3][0] = g.w[0][3] = 10.0;

  g.w[1][3] = g.w[3][1] = 1.0;
  g.w[2][0] = g.w[0][2] = 1.0;

  std::vector<int> best_route;
  double res = brute_solve(g, 80.0, best_route);

  ck_assert_double_eq_tol(res, 6.0, 1e-6);
}
END_TEST

START_TEST(test_bruteforce_no_feasible_route) {
  Graph g;
  g.w.assign(4, std::vector<double>(4, 0.0));

  double w = 50.0;
  for (size_t i = 0; i < g.w.size(); ++i) {
    for (size_t j = i + 1; j < g.w.size(); ++j) {
      g.w[i][j] = g.w[j][i] = w;
    }
  }

  std::vector<int> best_route;
  double res = brute_solve(g, 80.0, best_route);

  ck_assert(!std::isfinite(res));
  ck_assert_int_eq((int)best_route.size(), 0);
}
END_TEST

START_TEST(test_ant_uniform_graph_feasible) {
  Graph g;
  g.w.assign(5, std::vector<double>(5, 0.0));

  double w = 10.0;
  for (size_t i = 0; i < g.w.size(); ++i) {
    for (size_t j = i + 1; j < g.w.size(); ++j) {
      g.w[i][j] = g.w[j][i] = w;
    }
  }

  AntParams p;
  p.ants = static_cast<int>(g.w.size());
  p.iters = 20;
  p.alpha = 1.0;
  p.beta = 2.0;
  p.rho = 0.5;
  p.q = 100.0;
  p.tau0 = 1.0;
  p.tau_min = 0.01;

  std::vector<int> best_route;

  double res = ant_solve(g, 80.0, p, best_route);

  ck_assert(std::isfinite(res));
  ck_assert_double_eq_tol(res, 50.0, 1e-6);
  ck_assert_int_eq((int)best_route.size(), 6);
  ck_assert_int_eq(best_route.front(), 0);
  ck_assert_int_eq(best_route.back(), 0);
}
END_TEST

START_TEST(test_ant_no_feasible_route) {
  Graph g;
  g.w.assign(5, std::vector<double>(5, 0.0));

  double w = 30.0;
  for (size_t i = 0; i < g.w.size(); ++i) {
    for (size_t j = i + 1; j < g.w.size(); ++j) {
      g.w[i][j] = g.w[j][i] = w;
    }
  }

  AntParams p;
  p.ants = static_cast<int>(g.w.size());
  p.iters = 20;
  p.alpha = 1.0;
  p.beta = 2.0;
  p.rho = 0.5;
  p.q = 100.0;
  p.tau0 = 1.0;
  p.tau_min = 0.01;

  std::vector<int> best_route;

  double res = ant_solve(g, 80.0, p, best_route);

  ck_assert(!std::isfinite(res));
  ck_assert_int_eq((int)best_route.size(), 0);
}
END_TEST

Suite *tsp_suite(void) {
  Suite *s = suite_create("tsp");

  TCase *tc_bruteforce = tcase_create("bruteforce");
  tcase_add_test(tc_bruteforce, test_bruteforce_simple_cycle);
  tcase_add_test(tc_bruteforce, test_bruteforce_two_cycles);
  tcase_add_test(tc_bruteforce, test_bruteforce_no_feasible_route);
  suite_add_tcase(s, tc_bruteforce);

  TCase *tc_ant = tcase_create("ant");
  tcase_add_test(tc_ant, test_ant_uniform_graph_feasible);
  tcase_add_test(tc_ant, test_ant_no_feasible_route);
  suite_add_tcase(s, tc_ant);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = tsp_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? 0 : 1;
}