#include <iostream>

int main() {
  int x = 10;
  int *ptr = &x;
  std::cout << ptr << " " << &x; // Should print out 0x.....
  return 0;
}
