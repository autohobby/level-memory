#include "base.h"

void data_write() {
    Type1 t1, t2, t3;
    t1.a = 2;
    t1.b = 3;
    FILE *fid;
    fid = fopen("bin.dat", "wb");
    if (fid == NULL) {
        printf("Error");
        return;
    }
    fwrite(&t1, sizeof(Type1), 1, fid);
    fwrite(&t1, sizeof(Type1), 1, fid);
    fwrite(&t1, sizeof(Type1), 1, fid);
    fclose(fid);
}

int main()
{
    Type1 t1, t2, t3;
    char hello[25];
    t1.a = 1;
    t1.b = 2;
    t1.c[0] = '0';
    t1.c[1] = '1';
    strcpy(hello, "hello world");
    printf("%s\n", hello);
    data_write();
    cout<<sizeof(unsigned char)<<endl;
    // cout<<sizeof(byte)<<endl;
    cout<<"hello world"<<endl;
    cout<<sizeof(Type1)<<endl;
    memcpy(buffer, &t1, sizeof(t1));
    memcpy(&t2, &t1, sizeof(t1));
    t3 = t1;
    for (int i=0; i<20; i++) {
        // cout<<buffer[i]-0<<endl;
        printf("%02X %02X\n", (uint8_t)buffer[i], buffer[i]);
    }
    cout<<t2.b<<endl;
    cout<<t3.b<<endl;
    return 0;
}