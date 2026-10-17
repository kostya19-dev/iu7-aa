#include "../inc/cpu_time.h"
#include "../inc/print_numbers.h"

double get_time_sort(void (*print_func)(int, int), int a, int b)
{
    std::clock_t start_t = std::clock();
    print_func(a, b);
    std::clock_t end_t = std::clock();
    double total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC * 1000.0; 
    return total_t;
}

void plot_with_gnuplot()
{
    FILE *gp = popen("gnuplot -persist", "w");
    if (!gp)
    {
        std::cerr << " Ошибка: не удалось запустить gnuplot. Убедитесь, что он установлен." << std::endl;
        return;
    }

    fprintf(gp, "set title 'Сравнение времени работы рекурсивного и итеративного вывода чисел'\n");
    fprintf(gp, "set xlabel 'Количество чисел (B - A + 1)'\n");
    fprintf(gp, "set ylabel 'Время, мс'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key left top\n");
    fprintf(gp, "set datafile separator ','\n");

    fprintf(gp, "plot 'results.csv' using 1:2 with linespoints lw 2 pt 7 ps 0.8 title 'Рекурсивный', \\\n");
    fprintf(gp, "     'results.csv' using 1:3 with linespoints lw 2 pt 9 ps 0.8 title 'Итеративный'\n");

    fflush(gp);
    pclose(gp);
    std::cout << "График отображён через gnuplot." << std::endl;
}

void measure_time(int start, int max_len, int step)
{
    struct Result {
        int n;
        double recursive, iterative;
    };
    std::vector<Result> results;

    for (int i = start; i <= max_len; i += step)
    {
        if (i <= 0) continue;

        int a = 1, b = i;
        double st1 = 0.0, st2 = 0.0;

        print_numbers_recursive(a, b);
        print_numbers_iterative(a, b);

        for (int j = 0; j < NUMBER_OPER; j++)
        {
            st1 += get_time_sort(print_numbers_recursive, a, b);
            st2 += get_time_sort(print_numbers_iterative, a, b);
        }

        st1 /= NUMBER_OPER;
        st2 /= NUMBER_OPER;

        results.push_back({i, st1, st2});
    }

    printf("+--------+---------------------+\n");
    printf("| %6s | %15s |\n", "Кол-во", "Время, мс");
    printf("|        |---------+----------|\n");
    printf("| %6s | %9s | %8s |\n", "", "Рекурс.", "Итер.");
    printf("|--------|---------|----------|\n");

    for (const auto& r : results)
    {
        printf("| %6d | %7.3f | %8.3f |\n", r.n, r.recursive, r.iterative);
    }
    printf("+--------+---------+----------+\n");
    std::cout << std::endl;

    FILE *csv_file = fopen("results.csv", "w");
    if (!csv_file)
    {
        std::cerr << "Ошибка: не удалось создать файл results.csv для записи!" << std::endl;
        return;
    }

    fprintf(csv_file, "N,Recursive,Iterative\n");
    for (const auto& r : results)
    {
        fprintf(csv_file, "%d,%.3f,%.3f\n", r.n, r.recursive, r.iterative);
    }
    fclose(csv_file);
    std::cout << "Данные сохранены в results.csv" << std::endl;

    plot_with_gnuplot();
}