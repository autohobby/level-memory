#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>

#include <ahm.h>

using namespace cv;
using namespace std;

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
    Mat image;
    VideoCapture capture;
    capture.open("sample.mp4");
    int fid = 0;

    if(capture.isOpened())
    {
        cout << "Capture is opened" << endl;
        for(;;)
        {
            if (!capture.isOpened()) {
                return 0;
            }
            capture >> image;
            cout<<fid<<endl;
            fid ++;

            // cout<<image.cols<<endl;
            // cout<<image.rows<<endl;
            // cout<<image.channels()<<endl;
            // printf("%p\n", image.data);
            // cout<< "add" <<endl;
            char* buf = (char*)ahm_f1array_tptr(f1array);
            if (buf!=NULL) {
                printf("%p\n", buf);
            }
            // cout<< "cpy" <<endl;
            // for (int i=0; i<8; i++) {
            //     printf("%02X ", image.data+i);
            // }
            char *from = (char*)image.data;
            for (int i=0; i<8; i++) {
                printf("%02X ", (uint8_t)*(from+i));
            }
            printf("\n");
            memcpy(buf, from, 1280*720*3);
            
            // for (int i=0; i<1280; i++) {
            //     cout<< 'g' << i <<endl;
            //     *(buf+i) = *(image.data+i);
            // }
            // cout<< "push" <<endl;
            ahm_f1array_push(f1array);
            // cout<< *f1array->top <<endl;

            if(image.empty())
                break;

            imshow("Sample", image);


            if(waitKey(20) >= 0)
                break;
        }
    }
    ahm_close(&_mcb);

    return 0;
}
