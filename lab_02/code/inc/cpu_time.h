#ifndef CPU_TIME_H
#define CPU_TIME_H

#include <ctime>
#include <vector>
#include <iostream>
#include <iomanip>

#define NUMBER_OPER 1000

double get_cpu_time(void (*print_func)(int, int), int a, int b);
void measure_time(int start, int max_len, int step);
void plot_with_gnuplot();

#endif