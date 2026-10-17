#include <iostream>
#include <vector>
#include "../inc/multy_matrix.h"
#include <check.h>

static void assert_matrix_equal(const matrix_v &a, const matrix_v &b)
{
    ck_assert_int_eq(a.size(), b.size());
    for (size_t i = 0; i < a.size(); i++)
    {
        ck_assert_int_eq(a[i].size(), b[i].size());
        for (size_t j = 0; j < a[i].size(); j++)
        {
            ck_assert_int_eq(a[i][j], b[i][j]);
        }
    }
}

START_TEST(test_multy_2x2)
{
    matrix_v A = {{1, 2}, {3, 4}};
    matrix_v B = {{5, 6}, {7, 8}};
    matrix_v expected = {{19, 22}, {43, 50}};

    matrix_v res1 = standart_multy(A, B);
    matrix_v res2 = vinograd_alg(A, B);
    matrix_v res3 = opt_vinograd_alg(A, B);

    assert_matrix_equal(res1, expected);
    assert_matrix_equal(res2, expected);
    assert_matrix_equal(res3, expected);
}
END_TEST

START_TEST(test_multy_3x2_2x3)
{
    matrix_v A = {{1, 2}, {3, 4}, {5, 6}};
    matrix_v B = {{7, 8, 9}, {10, 11, 12}};
    matrix_v expected = {
        {27, 30, 33},
        {61, 68, 75},
        {95, 106, 117}
    };

    matrix_v res1 = standart_multy(A, B);
    matrix_v res2 = vinograd_alg(A, B);
    matrix_v res3 = opt_vinograd_alg(A, B);

    assert_matrix_equal(res1, expected);
    assert_matrix_equal(res2, expected);
    assert_matrix_equal(res3, expected);
}
END_TEST

START_TEST(test_multy_identity)
{
    matrix_v A = {{1, 2}, {3, 4}};
    matrix_v I = {{1, 0}, {0, 1}};

    matrix_v res1 = standart_multy(A, I);
    matrix_v res2 = vinograd_alg(A, I);
    matrix_v res3 = opt_vinograd_alg(A, I);

    assert_matrix_equal(res1, A);
    assert_matrix_equal(res2, A);
    assert_matrix_equal(res3, A);
}
END_TEST

START_TEST(test_multy_zero_matrix)
{
    matrix_v A = {{1, 2}, {3, 4}};
    matrix_v B = {{0, 0}, {0, 0}};
    matrix_v expected = {{0, 0}, {0, 0}};

    matrix_v res1 = standart_multy(A, B);
    matrix_v res2 = vinograd_alg(A, B);
    matrix_v res3 = opt_vinograd_alg(A, B);

    assert_matrix_equal(res1, expected);
    assert_matrix_equal(res2, expected);
    assert_matrix_equal(res3, expected);
}
END_TEST

START_TEST(test_multy_1x1)
{
    matrix_v A = {{5}};
    matrix_v B = {{7}};
    matrix_v expected = {{35}};

    matrix_v res1 = standart_multy(A, B);
    matrix_v res2 = vinograd_alg(A, B);
    matrix_v res3 = opt_vinograd_alg(A, B);

    assert_matrix_equal(res1, expected);
    assert_matrix_equal(res2, expected);
    assert_matrix_equal(res3, expected);
}
END_TEST

START_TEST(test_multy_2x3_3x2)
{
    matrix_v A = {{1, 2, 3}, {4, 5, 6}};
    matrix_v B = {{7, 8}, {9, 10}, {11, 12}};
    matrix_v expected = {{58, 64}, {139, 154}};

    matrix_v res1 = standart_multy(A, B);
    matrix_v res2 = vinograd_alg(A, B);
    matrix_v res3 = opt_vinograd_alg(A, B);

    assert_matrix_equal(res1, expected);
    assert_matrix_equal(res2, expected);
    assert_matrix_equal(res3, expected);
}
END_TEST

START_TEST(test_multy_row_vector_by_column_vector)
{
    matrix_v A = {{1, 2, 3}};
    matrix_v B = {{4}, {5}, {6}};
    matrix_v expected = {{32}};

    matrix_v res1 = standart_multy(A, B);
    matrix_v res2 = vinograd_alg(A, B);
    matrix_v res3 = opt_vinograd_alg(A, B);

    assert_matrix_equal(res1, expected);
    assert_matrix_equal(res2, expected);
    assert_matrix_equal(res3, expected);
}
END_TEST

START_TEST(test_multy_column_vector_by_row_vector)
{
    matrix_v A = {{1}, {2}, {3}};
    matrix_v B = {{4, 5, 6}};
    matrix_v expected = {{4, 5, 6}, {8, 10, 12}, {12, 15, 18}};

    matrix_v res1 = standart_multy(A, B);
    matrix_v res2 = vinograd_alg(A, B);
    matrix_v res3 = opt_vinograd_alg(A, B);

    assert_matrix_equal(res1, expected);
    assert_matrix_equal(res2, expected);
    assert_matrix_equal(res3, expected);
}
END_TEST

START_TEST(test_multy_incompatible_1x3_2x2)
{
    matrix_v A = {{1, 2, 3}};
    matrix_v B = {{1, 2}, {3, 4}};

    matrix_v res1 = standart_multy(A, B);
    matrix_v res2 = vinograd_alg(A, B);
    matrix_v res3 = opt_vinograd_alg(A, B);

    ck_assert_int_eq(res1.size(), 0);
    ck_assert_int_eq(res2.size(), 0);
    ck_assert_int_eq(res3.size(), 0);
}
END_TEST

START_TEST(test_multy_incompatible_2x2_3x2)
{
    matrix_v A = {{1, 2}, {3, 4}};
    matrix_v B = {{1, 2}, {3, 4}, {5, 6}};

    matrix_v res1 = standart_multy(A, B);
    matrix_v res2 = vinograd_alg(A, B);
    matrix_v res3 = opt_vinograd_alg(A, B);

    ck_assert_int_eq(res1.size(), 0);
    ck_assert_int_eq(res2.size(), 0);
    ck_assert_int_eq(res3.size(), 0);
}
END_TEST

START_TEST(test_multy_empty_matrices)
{
    matrix_v A = {};
    matrix_v B = {{1, 2}, {3, 4}};

    matrix_v res1 = standart_multy(A, B);
    matrix_v res2 = vinograd_alg(A, B);
    matrix_v res3 = opt_vinograd_alg(A, B);

    ck_assert_int_eq(res1.size(), 0);
    ck_assert_int_eq(res2.size(), 0);
    ck_assert_int_eq(res3.size(), 0);
}
END_TEST

START_TEST(test_multy_both_empty_matrices)
{
    matrix_v A = {};
    matrix_v B = {};

    matrix_v res1 = standart_multy(A, B);
    matrix_v res2 = vinograd_alg(A, B);
    matrix_v res3 = opt_vinograd_alg(A, B);

    ck_assert_int_eq(res1.size(), 0);
    ck_assert_int_eq(res2.size(), 0);
    ck_assert_int_eq(res3.size(), 0);
}
END_TEST

Suite *multy_suite(void)
{
    Suite *s;
    TCase *tc_pos;
    TCase *tc_neg;

    s = suite_create("matrix_multiplication");

    tc_pos = tcase_create("positives");
    tcase_add_test(tc_pos, test_multy_2x2);
    tcase_add_test(tc_pos, test_multy_3x2_2x3);
    tcase_add_test(tc_pos, test_multy_identity);
    tcase_add_test(tc_pos, test_multy_zero_matrix);
    tcase_add_test(tc_pos, test_multy_1x1);
    tcase_add_test(tc_pos, test_multy_2x3_3x2);
    tcase_add_test(tc_pos, test_multy_row_vector_by_column_vector);
    tcase_add_test(tc_pos, test_multy_column_vector_by_row_vector);
    suite_add_tcase(s, tc_pos);

    tc_neg = tcase_create("negatives");
    tcase_add_test(tc_neg, test_multy_incompatible_1x3_2x2);
    tcase_add_test(tc_neg, test_multy_incompatible_2x2_3x2);
    tcase_add_test(tc_neg, test_multy_empty_matrices);
    tcase_add_test(tc_neg, test_multy_both_empty_matrices);
    suite_add_tcase(s, tc_neg);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = multy_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? 0 : 1;
}