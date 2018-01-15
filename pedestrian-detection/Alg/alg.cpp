#include "alg.h"

#include <cstdio>
using namespace pd;
Alg::Alg()
{
    printf("alg init\n");
}
void Alg::work(Mat &mt,string &rst)
{
    CascadeClassifier cascade;
    bool ret=false;
    vector<Rect> objs;
    string cascade_name = "hogcascade_pedestrians.xml";
    if (!cascade.load(cascade_name))
    {
     //   prt(info,"can't load cascade");
        exit(0);
    }
    // while (1)
    {

        Mat frame(mt);
        std::string str;
        str.clear();
        rst.clear();
        if (!frame.empty())
        {    //  if (frame_num % 3 == 0)
            if (1)
            {
                resize(frame,frame,Size(frame.cols / 2, frame.rows / 2),CV_INTER_LINEAR);
                cvtColor(frame, gray_frame, CV_BGR2GRAY);
                cascade.detectMultiScale(gray_frame, objs, 1.1, 3);
                vector<Rect>::iterator it = objs.begin();
                while (it != objs.end() && objs.size() != 0)
                {
                    pedestrian_num++;
                    pedestrians = frame(*it);

                    Rect rct = *it;
                    {
                        rectangle(frame, rct, Scalar(0, 255, 0), 2);

                        str.append( std::to_string(rct.x)).append(",").append( std::to_string(rct.y)).append(",").append( std::to_string(rct.width)).append(",").append( std::to_string(rct.height)).append(":");
                        rst.append(str.data());

                        ret=true;
                        //  break;//TODO, now we get first one
                    }
                    it++;
                }
                objs.clear();
            }
        }
        else{
           // prt(info,"opencv handle frame error !");
        }
    }

}
