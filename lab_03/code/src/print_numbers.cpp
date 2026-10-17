#include "../inc/print_numbers.h"

void print_numbers_recursive(int a, int b) {
  if (b > a) {
    print_numbers_recursive(a, b - 1);
  }
  std::cout << b << " ";
}

void print_numbers_iterative(int a, int b) {
  if (a <= b) {
    for (int i = a; i <= b; i++) {
      std::cout << i << " ";
    }
  }
}