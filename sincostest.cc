#include <stdio.h>
#include <math.h>
#include "./sincos.h"

int main() {
  for (int i = -1024; i <= 1024; i++) {
    int16_t expected = round(1024 * sin(i * M_PI * 2 / 1024.0));
    int16_t actual = LookupSin(i);
    if (expected != actual) {
      printf("%d %d %d\n", i, LookupSin(i), expected);
    }
  }
}
