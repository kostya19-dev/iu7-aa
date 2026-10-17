#include <iostream>
#include <sstream>
#include "../inc/print_numbers.h"
#include <check.h>


std::string capture_output(void (*func)(int, int), int a, int b) {
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    func(a, b);
    std::cout.rdbuf(old);
    return buffer.str();
}


START_TEST(test_recursive_basic)
{
    std::string res = capture_output(print_numbers_recursive, 1, 3);
    ck_assert_str_eq(res.c_str(), "123");
}
END_TEST

START_TEST(test_recursive_single)
{
    std::string res = capture_output(print_numbers_recursive, 5, 5);
    ck_assert_str_eq(res.c_str(), "5");
}
END_TEST

START_TEST(test_recursive_invalid)
{

    std::string res = capture_output(print_numbers_recursive, 5, 3);
    ck_assert_str_eq(res.c_str(), "3");
}
END_TEST


START_TEST(test_iterative_basic)
{
    std::string res = capture_output(print_numbers_iterative, 1, 3);
    ck_assert_str_eq(res.c_str(), "1 2 3");
}
END_TEST

START_TEST(test_iterative_equal)
{
    std::string res = capture_output(print_numbers_iterative, 7, 7);
    ck_assert_str_eq(res.c_str(), "7");
}
END_TEST

START_TEST(test_iterative_invalid)
{

    std::string res = capture_output(print_numbers_iterative, 5, 2);
    ck_assert_str_eq(res.c_str(), "");
}
END_TEST


Suite *print_numbers_suite(void)
{
    Suite *s = suite_create("print_numbers");

    TCase *tc_recursive = tcase_create("recursive");
    tcase_add_test(tc_recursive, test_recursive_basic);
    tcase_add_test(tc_recursive, test_recursive_single);
    tcase_add_test(tc_recursive, test_recursive_invalid);
    suite_add_tcase(s, tc_recursive);

    TCase *tc_iterative = tcase_create("iterative");
    tcase_add_test(tc_iterative, test_iterative_basic);
    tcase_add_test(tc_iterative, test_iterative_equal);
    tcase_add_test(tc_iterative, test_iterative_invalid);
    suite_add_tcase(s, tc_iterative);

    return s;
}

int main(void)
{
    Suite *s = print_numbers_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (failed == 0) ? 0 : 1;
}
