#include <iostream>
#include "../inc/print_numbers.h"
#include "../inc/cpu_time.h"

int main()
{
    setbuf(stdout, NULL);

    int choose = -1, start, max_len, step;

    std::string menu = "--------------- Menu --------------\n"
                       "1 - Рекурсивный вывод чисел\n"
                       "2 - Итеративный вывод чисел\n"
                       "3 - Оба алгоритма\n"
                       "4 - Замерить время\n"
                       "0 - Выход\n";

    while (choose != 0)
    {
        int a, b;
        std::cout << menu;
        std::cout << "Выбор: ";
        if (!(std::cin >> choose) || choose < 0 || choose > 4)
        {
            std::cout << "Ошибка! Неверный ввод! Нужно ввести число от 0 до 4!\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (choose > 0 && choose < 4)
        {
            std::cout << "Введите два целых числа A и B (A ≤ B): ";
            if (!(std::cin >> a >> b) || a > b)
            {
                std::cout << "Ошибка! Неверный ввод! A должно быть ≤ B!\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }

            if (choose == 1)
            {
                std::cout << "Рекурсивный вывод: ";
                for (int i = a; i <= b; i++) {
                    std::cout << i;
                    if (i < b) std::cout << " ";
                }
                std::cout << std::endl;
            }
            else if (choose == 2)
            {
                std::cout << "Итеративный вывод: ";
                print_numbers_iterative(a, b);
                std::cout << std::endl;
            }
            else if (choose == 3)
            {
                std::cout << "Рекурсивный вывод: ";
                for (int i = a; i <= b; i++) {
                    std::cout << i;
                    if (i < b) std::cout << " ";
                }
                std::cout << std::endl;

                std::cout << "Итеративный вывод: ";
                print_numbers_iterative(a, b);
                std::cout << std::endl;
            }

        }
        else if (choose == 4)
        {
            std::cout << "Введите параметры для замеров времени (начальное количество чисел, максимальное количество, шаг):\n";
            if (!(std::cin >> start >> max_len >> step))
            {
                std::cout << "Ошибка! Вводите числа!\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }
            else
            {
                measure_time(start, max_len, step);
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }
        }
    }

    return 0;
}