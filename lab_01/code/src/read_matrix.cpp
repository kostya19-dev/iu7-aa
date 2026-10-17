#include "../inc/read_matrix.h"
#include <iostream>

int read_size(size_t &n)
{
    int temp;
    if (!(std::cin >> temp) || temp <= 0)
        return 1;
    n = static_cast<size_t>(temp);
    return 0;
}

int read_mtr(matrix_v &mtr, std::string mode)
{
    size_t n, m;
    std::cout << "Введите размер матрицы: ";
    if (read_size(n))
    {
        std::cout << "Ошибка! Размер N введен неверно!" << std::endl;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return 1;
    }
    
    if (mode == "square")
        m = n;
    else
    {
        if (read_size(m))
        {
            std::cout << "Ошибка! Размер M введен неверно!" << std::endl;
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            return 1;
        }
    }

    array_v row;
    row.resize(m, 0);
    mtr.resize(n, row);

    std::cout << "Введите элементы матрицы (" << n << " строк по " << m << " элементов):\n";
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < m; j++)
        {
            if (!(std::cin >> mtr[i][j]))
            {
                std::cout << "Ошибка! Элемент матрицы введен неверно!" << std::endl;
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                return 1;
            }
        }
    }
    return 0;
}