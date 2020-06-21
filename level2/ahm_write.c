#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ahm.h"

static void test() {
    ahm_mcb temp;
    ahm_mcb* mcb = &temp;
    int i;
    if (ahm_open(mcb, "/hello/world", 1024*1024*32) == -1) {
        perror("ahm open");
        return;
    }
    for (i=0; i<1024; i+=4) {
        int* t = mcb->buf+i;
        *t = i*1000;
    }
    ahm_close(mcb);
}

static void test_f1array() {
    ahm_mcb _mcb;
    ahm_f1array _f1array;
    ahm_f1array* f1array = &_f1array;
    f1array->mcb = &_mcb;
    uint32_t onesize = 1280*720*3;

    if (ahm_open_f1array(f1array, "/hello/f9", onesize, 24)==-1) {
        perror("ahm_open_f1array");
        return;
    }

    ahm_close(&_mcb);
    ahm_remove(&_mcb);
}

int main() {
    test();
    return 0;
}
