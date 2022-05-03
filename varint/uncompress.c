#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t generate_number()
{
    const int r = rand();
    const int p = r % 100;
    if (p < 90) {
        return r % 128;
    }
    if (p < 95) {
        return r % 16384;
    }
    if (p < 99) {
        return r % 2097152;
    }
    return r % 268435455;
}

int main(){
    for(int i = 0; i < 1000000; i++) {
        printf("%d\n", generate_number());
    }
}