#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <time.h>

#include <ahm.h>

using namespace cv;
using namespace std;

void sleep_ms(int ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms - ts.tv_sec * 1000) * 1000000;
  nanosleep(&ts, nullptr);
}

void drawText(Mat & image);

int main()
{
    ahm_mcb _mcb;
    ahm_f1array _f1array;
    ahm_f1array* f1array = &_f1array;
    f1array->mcb = &_mcb;
    uint32_t onesize = 1280*720*3;

    if (!ahm_open_f1array(f1array, "temp1", onesize, 24)) {
        perror("ahm_open_f1array");
        return 0;
    }

    cout << "Built with OpenCV " << CV_VERSION << endl;
    // Mat image;
    Mat image = Mat(720,1280,CV_8UC3,Scalar(255, 255, 255));

    uint32_t index = 0;
    for(;;)
    {
        while (!ahm_f1array_check_index(f1array, index)) {
            sleep_ms(5);
        }

        char* buf = (char*)ahm_f1array_iptr(f1array, index);
        if (buf!=NULL) {
            printf("%p\n", buf);
        }
        cout<<"cpy"<<endl;
        char *from = (char*)buf;
        for (int i=0; i<8; i++) {
            printf("%02X ", (uint8_t)*(from+i));
        }
        memcpy((char*)image.data, buf, 1280*720*3);
        cout<<"cpy1"<<endl;
        index ++;

        if(image.empty())
            break;

        imshow("test", image);

        if(waitKey(10) >= 0)
            break;
    }

    ahm_close(&_mcb);

    return 0;
}