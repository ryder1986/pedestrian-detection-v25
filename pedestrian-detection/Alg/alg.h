#ifndef ALG_H
#define ALG_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "alg_global.h"
#include "cstdio"
namespace pd {
using namespace cv;
class ALGSHARED_EXPORT Alg
{

public:
    Alg();
    void work(Mat &mt,string &rst);
private:

    Mat gray_frame;
    int pedestrian_num = 0;
    int frame_num = 0;
    Mat pedestrians;

};
}
#endif // ALG_H
