#include "../inc/cpu_time.h"

#define NUMBER_OPER 1000


void random_mtr(matrix_v &mtr)
{
    for (size_t i = 0; i < mtr.size(); i++)
    {
        for (size_t j = 0; j < mtr[0].size(); j++)
        {
            mtr[i][j] = rand() % 100;
        }
    }
}

void resize_mtr(matrix_v &mtr, size_t n, size_t m)
{
    array_v row;
    row.resize(m, 0);
    mtr.resize(n, row);
}

double get_time_sort(matrix_v (*multy_mtr)(const matrix_v &, const matrix_v &),
                     const matrix_v &mtr1, const matrix_v &mtr2)
{
    std::clock_t start_t = std::clock();
    matrix_v res = multy_mtr(mtr1, mtr2);
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


    fprintf(gp, "set title 'Сравнение времени работы алгоритмов умножения матриц'\n");
    fprintf(gp, "set xlabel 'Размер матрицы N (N×N)'\n");
    fprintf(gp, "set ylabel 'Время, мс'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set key left top\n");
    fprintf(gp, "set datafile separator ','\n");

    fprintf(gp, "plot 'results.csv' using 1:2 with linespoints lw 2 pt 7 ps 0.8 title 'Классический', \\\n");
    fprintf(gp, "     'results.csv' using 1:3 with linespoints lw 2 pt 9 ps 0.8 title 'Винограда', \\\n");
    fprintf(gp, "     'results.csv' using 1:4 with linespoints lw 2 pt 11 ps 0.8 title 'Опт. Винограда'\n");

    fflush(gp);
    pclose(gp);
    std::cout << "График отображён через gnuplot." << std::endl;
}


void measure_time(size_t start, size_t max_len, size_t step)
{
    struct Result {
        size_t n;
        double classic, vinograd, opt_vinograd;
    };
    std::vector<Result> results;

    for (size_t i = start; i <= max_len; i += step)
    {
        if (i == 0) continue;

        matrix_v mtr1, mtr2;
        double st1 = 0.0, st2 = 0.0, st3 = 0.0;

        resize_mtr(mtr1, i, i);
        resize_mtr(mtr2, i, i);

        random_mtr(mtr1);
        random_mtr(mtr2);

        for (size_t j = 0; j < NUMBER_OPER; j++)
        {
            st1 += get_time_sort(standart_multy, mtr1, mtr2);
            st2 += get_time_sort(vinograd_alg, mtr1, mtr2);
            st3 += get_time_sort(opt_vinograd_alg, mtr1, mtr2);
        }

        st1 /= NUMBER_OPER;
        st2 /= NUMBER_OPER;
        st3 /= NUMBER_OPER;

        results.push_back({i, st1, st2, st3});
    }


    printf("+--------+--------------------------------------------+\n");
    printf("| %5s | %16s %s %15s |\n", "Размер", "", "Время, мс", "");
    printf("|        |------------+---------------+---------------|\n");
    printf("| %6s | %15s | %20s | %21s |\n", "", "Класс.", " Винограда ", "(опт.)Винограда");
    printf("|--------|------------|---------------|---------------|\n");

    for (const auto& r : results)
    {
        printf("| %6zu | %10.3f | %13.3f | %13.3f |\n", r.n, r.classic, r.vinograd, r.opt_vinograd);
    }
    printf("+--------+------------+---------------+---------------+\n");
    std::cout << std::endl;

    FILE *csv_file = fopen("results.csv", "w");
    if (!csv_file)
    {
        std::cerr << "Ошибка: не удалось создать файл results.csv для записи!" << std::endl;
        return;
    }

    fprintf(csv_file, "N,Classic,Vinograd,OptVinograd\n");
    for (const auto& r : results)
    {
        fprintf(csv_file, "%zu,%.3f,%.3f,%.3f\n", r.n, r.classic, r.vinograd, r.opt_vinograd);
    }
    fclose(csv_file);
    std::cout << "Данные сохранены в results.csv" << std::endl;

    plot_with_gnuplot();
}