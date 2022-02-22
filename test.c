#include <stdio.h>
#include <dc_network/options.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include <stdint-gcc.h>

int share = 0;

int stastic(int *numbers, int size) {
    int i = 1;
    int dif, min, max = 0;
    while (i < size) {
        if (numbers[i] != (numbers[i - 1] + 1)) {
            dif = numbers[i] - numbers[i - 1];
            //printf("dif: %d\n", dif);
            if (min == 0) {
                min = dif;
            }
            if (max == 0) {
                max = dif;
            }
            if (dif < min) {
                min = dif;
            }
            if (dif > max) {
                max = dif;
            }
            // printf("dif:%d %d %d\n", dif, numbers[i], numbers[i - 1]);
        }
        i++;
    }
    printf("min: %d max: %d\n", min, max);
    return 0;
}

void out_of_order(int *numbers, int size) {
    int i = 1, dif = 0, min = 0, max = 0;
    while (i < size) {
        if (numbers[i-1] == i) {
            dif++;
            printf("dif0: %d\n", dif);
        } else {
            printf("dif: %d\n", dif);
            if (min == 0) {
                min = dif;
            }
            if (max == 0) {
                max = dif;
            }
            if (dif < min && dif != 0) {
                min = dif;
            }
            if (dif > max) {
                max = dif;
            }
            dif = 0;
        }
        i++;
    }
    if (dif != 0) {
        printf("dif2: %d\n", dif);
        if (min == 0) {
            min = dif;
        }
        if (max == 0) {
            max = dif;
        }
        if (dif < min && dif != 0) {
            min = dif;
        }
        if (dif > max) {
            max = dif;
        }
        dif = 0;
    }
    printf("min: %d max: %d\n", min, max);
}

int main() {
    int arr[] = {1, 3, 2, 4, 7, 6, 5};
    //stastic(arr, 10);
    out_of_order(arr, 10);
    return (0);
}

