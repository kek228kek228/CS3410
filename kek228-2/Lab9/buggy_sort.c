#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

// TODO : Debug using GDB

/**
 * Returns the index of the smallest element in int array arr with length len.
 */
int smallest_idx(int *arr, int len)
{
    int i;
    int smallest_i = 0;
    int smallest = arr[0];

    for (i = 1; i < len; i++)
    {
        if (arr[i] < smallest)
        {
            smallest = arr[i];
            smallest_i = i;
        }
    }

    return smallest_i;
}

/**
 * Swaps the values stored at the memory addresses pointed to by a and b.
 */
void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/**
 * Performs an in-place selection sort on int array arr with length len.
 */
void selection_sort(int *arr, int len)
{
    int i, swap_idx;

    for (i = 0; i < len; i++)
    {
        swap_idx = smallest_idx(&arr[i], len - i);

        swap(&arr[i], &arr[swap_idx+i]);
    }
}

/**
 * Prints the int array arr with length len to stdout.
 */
void print_array(int *arr, int len)
{
    int i;
    while (i < len)
    {
        printf("%d ", arr[i]);
        i++;
    }
    puts("");
}

int main(int argc, char *argv[])
{
    // TODO : Add 5 Test Cases
    // example
    int array[5] = {4,3,2,1,0};
    selection_sort(array, 5);
    print_array(array, 5);
}
