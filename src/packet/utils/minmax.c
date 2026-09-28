#include "../../include/utils/minmax.h"


int min_d(int* arr, std::size_t n) {
    int min;
    int val;

    if(!arr || n <= 0)
        return 0;
    min = arr[0];
    for(std::size_t i = 1; i < n; i++) {
        val = arr[i];
        if(val < min)
            min = val;
    }
    return min;
}
int max_d(int* arr, std::size_t n) {
    int max;
    int val;

    if(!arr || n <= 0)
        return 0;
    max = arr[0];
    for(std::size_t i = 1; i < n; i++) {
        val = arr[i];
        if(val > max)
            max = val;
    }
    return max;
}
