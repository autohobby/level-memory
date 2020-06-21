#include "base.h"
#include "can.h"
#include <time.h>

struct QC {
    int l;
    int r;
    int size;
} qc;

cancontent qv[1024];
qc.size = 1024;
qc.l = 0;
qc.r = 0;

bool empty(QC qc) {
    return qc.l == qc.r;
}

int nextr(QC qc) {
    return qc.r+1 == qc.size?0:qc.r+1;
}

int nextl(QC qc) {
    return qc.l+1 == qc.size?0:qc.l+1;
}

bool full(QC qc) {
    return qc.l == nextr(qc);
}

/*
l == r -> queue empty()
r+1 == l -> queue full()
*/

void product() {
    while (true) {
        addrandom(qc, qv);
        sleep(0.01);
    }
}

void consumer() {

}

int main() 
{
    return 0;
}