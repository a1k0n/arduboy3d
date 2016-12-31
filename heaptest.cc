#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

int main() {
  uint8_t heap[200], heapsiz = 0;
  int8_t z[200];
  for (int i = 0; i < 200; i++) {
    int8_t r = rand();
    heapsiz = HeapPush(r, heap, z, heapsiz);
  }
  printf("z: ");
  for (int i = 0; i < heapsiz; i++) {
    printf("%d ", z[i]);
  }
  printf("\n");

  printf("heap: ");
  for (int i = 0; i < heapsiz; i++) {
    printf("%d[%d] ", heap[i], z[heap[i]]);
  }
  printf("\n");

  printf("heap sorted: ");
  while (heapsiz > 0) {
    printf("%d[%d] ", heap[0], z[heap[0]]);
    heapsiz = HeapPop(heap, z, heapsiz);
  }
  printf("\n");
}
