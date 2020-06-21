#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ahm.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")

// static void test_parse_null() {
//     lept_value v;
//     v.type = LEPT_FALSE;
//     EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "null"));
//     EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
// }

// static void test_parse_expect_value() {
//     lept_value v;

//     v.type = LEPT_FALSE;
//     EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE, lept_parse(&v, ""));
//     EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));

//     v.type = LEPT_FALSE;
//     EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE, lept_parse(&v, " "));
//     EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
// }

// static void test_parse_invalid_value() {
//     lept_value v;
//     v.type = LEPT_FALSE;
//     EXPECT_EQ_INT(LEPT_PARSE_INVALID_VALUE, lept_parse(&v, "nul"));
//     EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));

//     v.type = LEPT_FALSE;
//     EXPECT_EQ_INT(LEPT_PARSE_INVALID_VALUE, lept_parse(&v, "?"));
//     EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
// }

// static void test_parse_root_not_singular() {
//     lept_value v;
//     v.type = LEPT_FALSE;
//     EXPECT_EQ_INT(LEPT_PARSE_ROOT_NOT_SINGULAR, lept_parse(&v, "null x"));
//     EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
// }

// static void test_parse() {
//     test_parse_null();
//     test_parse_expect_value();
//     test_parse_invalid_value();
//     test_parse_root_not_singular();
// }

static void test() {
    ahm_mcb temp;
    ahm_mcb* mcb = &temp;
    int i;
    if (ahm_open(mcb, "/hello/world", 1024*1024*32) == -1) {
        perror("ahm open");
        return;
    }

    printf("id %d %p buf %p\n", mcb->shmid, &mcb->shmid, mcb->buf);
    for (i=0; i<1024; i+=4) {
        int* t = mcb->buf+i;
        printf("%d ", *t);
    }
    printf("\n");
    ahm_close(mcb);
    ahm_remove(mcb);
}

int main() {
    // test_parse();
    // printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    test();
    return main_ret;
}
