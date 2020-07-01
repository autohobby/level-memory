#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ahm.h"

void sleep_ms(int ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms - ts.tv_sec * 1000) * 1000000;
  nanosleep(&ts, NULL);
}

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

static void test_e1array() {
    const char* filename = "/home/alpc32/tt";
    uint32_t size = 128*8;
    ahm_e1array* e1array = NULL;
    uint32_t remain;
    char cbuf[2048];
    byte* buf;
    int i;
    int j;
    char ch = 'a';
    for (i=0; i<1024; i++) {
        cbuf[i] = 'a';
    }

    e1array = ahm_open_e1array(filename, 1024, 24);
    ahm_e1array_init(e1array);
    // buf = ahm_e1array_tptr(e1array, &remain);
    // memcpy(e1array->b, cbuf, 1024);
    // putchar(e1array->b[1023]);
    // printf("hello world\n");
    // printf("%p\n", e1array->b+992);
    // printf("%p\n", e1array->b+1023);
    // printf("%p\n", e1array->b+1024);
    // *(e1array->b+1023) = 'a';
    // printf("memcpy\n");
    // memcpy(e1array->b+1022, cbuf, 1);
    // return 0;
    // printf("mcb->buf %p\n", e1array->mcb->buf);
    // printf("%p \n", buf);
    // printf("%d \n", remain);

    for (;;)
    for (i=1; i<=128; i++) {
        for (j=0; j<i; j++) {
            cbuf[j] = ch; 
        }
        cbuf[i] = 0;
        if (ch=='z') {
            ch = 'a';
        } else {
            ch ++;
        }
        // printf("%s %d\n", cbuf, i);
        ahm_e1array_pushtop(e1array, cbuf, i);
        // printf("\n\n");
        sleep_ms(1);
    }

    ahm_close(e1array->mcb);
    // ahm_remove(e1array->mcb);
    free(e1array);
}


int main() {
    // test();
    test_e1array();
    return 0;
}
