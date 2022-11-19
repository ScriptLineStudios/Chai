#include <stdio.h>

int fibb(int x) {
    if (x < 2) {
        return x;
    }
    else {
        return fibb(x - 1) + fibb(x - 2);
    }
}