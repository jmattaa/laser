#include "sort.h"
#include "utils.h"

// HELPER FUNCTIONS
void *median_of_three(void *a, void *b, void *c,
                      int(cmp)(const void *, const void *, const void *),
                      const void *arg)
{
    return cmp(a, b, arg) < 0
               ? (cmp(b, c, arg) < 0 ? b : (cmp(a, c, arg) < 0 ? c : a))
               : (cmp(a, c, arg) < 0 ? a : (cmp(b, c, arg) < 0 ? c : b));
}

void *partition(void *base, size_t n, size_t size,
                int(cmp)(const void *, const void *, const void *),
                const void *arg)
{
    char *array = base;
    // first middle and last
    char *pivot = median_of_three(array, array + (n / 2) * size,
                                  array + (n - 1) * size, cmp, arg);

    laser_swap(pivot, array + ((n - 1) * size), size);

    pivot = array + ((n - 1) * size);
    char *i = array;

    for (char *j = array; j < pivot; j += size)
    {
        if (cmp(j, pivot, arg) < 0)
        {
            laser_swap(i, j, size);
            i += size;
        }
    }
    laser_swap(i, pivot, size);
    return i;
}

// ------------------------MAIN FUNCTIONS------------------------------------

void laser_sort(void *base, size_t elem_count, size_t elem_size,
                int(cmp)(const void *a, const void *b, const void *arg),
                const void *arg)
{
    laser_quicksort(base, elem_count, elem_size, cmp, arg);
}

#define SWITCH_SORT_UNDER 8
void laser_quicksort(void *base, size_t elem_count, size_t elem_size,
                     int(cmp)(const void *a, const void *b, const void *arg),
                     const void *arg)
{
    while (elem_count > SWITCH_SORT_UNDER)
    {
        void *pivot = partition(base, elem_count, elem_size, cmp, arg);

        size_t left_size = ((char *)pivot - (char *)base) / elem_size;
        size_t right_size = elem_count - left_size - 1;

        if (left_size < right_size)
        {
            laser_quicksort(base, left_size, elem_size, cmp, arg);
            base = (char *)pivot + elem_size;
            elem_count = right_size;
        }
        else
        {
            laser_quicksort((char *)pivot + elem_size, right_size, elem_size,
                            cmp, arg);
            elem_count = left_size;
        }
    }

    // when array is small we do
    laser_insertsort(base, elem_count, elem_size, cmp, arg);
}

void laser_insertsort(void *base, size_t elem_count, size_t elem_size,
                      int cmp(const void *, const void *, const void *),
                      const void *arg)
{
    char *array = base;
    for (size_t i = 1; i < elem_count; i++)
    {
        char *target = array + i * elem_size;
        size_t left = 0, right = i;

        while (left < right)
        {
            size_t mid = left + (right - left) / 2;
            if (cmp(target, array + mid * elem_size, arg) < 0)
            {
                right = mid;
            }
            else
            {
                left = mid + 1;
            }
        }

        if (left != i)
        {
            char temp[elem_size];
            memcpy(temp, target, elem_size);
            memmove(array + (left + 1) * elem_size, array + left * elem_size,
                    (i - left) * elem_size);
            memcpy(array + left * elem_size, temp, elem_size);
        }
    }
}
