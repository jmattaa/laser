#ifndef LASER_SORT_QSORT_H
#define LASER_SORT_QSORT_H

#include <stddef.h>

// this does quicksort and then switches to insertsort for small arrays
// me big brain
void laser_sort(void *base, size_t elem_count, size_t elem_size,
                int cmp(const void *, const void *, const void *),
                const void *arg);

void laser_quicksort(void *base, size_t elem_count, size_t elem_size,
                     int cmp(const void *, const void *, const void *),
                     const void *arg);
void laser_insertsort(void *base, size_t elem_count, size_t elem_size,
                      int(cmp)(const void *a, const void *b, const void *arg),
                      const void *arg);

#endif
