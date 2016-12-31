#ifndef HEAP_H_
#define HEAP_H_

// returns new heap size
static uint8_t HeapPush(int8_t value, uint8_t *heap,
    int8_t *compare_values, uint8_t heap_siz) {
  compare_values[heap_siz] = value;
  heap[heap_siz] = heap_siz;
  uint8_t heapptr = heap_siz;
  while (heapptr > 0) {
    uint8_t parent = (heapptr - 1) >> 1;
    if (compare_values[heap[parent]] > value) {
      break;  // heap element is in proper place
    }
    // since we are larger, swap w/ parent
    heap[heapptr] = heap[parent];
    heap[parent] = heap_siz;
    heapptr = parent;
  }
  return heap_siz + 1;
}

// returns new heap size
static uint8_t HeapPop(uint8_t *heap,
    int8_t *compare_values, uint8_t heap_siz) {
  heap_siz --;
  heap[0] = heap[heap_siz];  // last element temporarily becomes root
  uint8_t heapptr = 0;
  int8_t newroot_value = compare_values[heap[0]];
  for (;;) {
    uint8_t child = (heapptr << 1) + 1;  // left child
    if (child >= heap_siz) break;
    // find largest child compare_value
    int8_t child_compare = compare_values[heap[child]];
    if (child < heap_siz-1) {
      int8_t right_child_compare = compare_values[heap[child + 1]];
      if (child_compare < right_child_compare) {
        child++;  // right child is larger, use that one
        child_compare = right_child_compare;
      }
    }
    if (newroot_value < child_compare) {
      // swap with left child
      uint8_t t = heap[child];
      heap[child] = heap[heapptr];
      heap[heapptr] = t;
      heapptr = child;  // and recurse
    } else {
      // we're >= either child, we're good
      break;
    }
  }
  return heap_siz;
}

#endif  // HEAP_H_
