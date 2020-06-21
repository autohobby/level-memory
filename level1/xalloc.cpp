#include "base.h"

int main()
{
    Type1 t1, t2, t3;
    t1.a = 123443210;
    char hello[25];
    void *test;
    test = malloc(1024);
    memcpy(test, &t1, sizeof(t1));
    memcpy(&t2, test, sizeof(t2));
    printf("%d\n", t2.a);
    return 0;
}