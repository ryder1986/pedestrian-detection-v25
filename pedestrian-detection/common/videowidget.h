#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H
#include <QtOpenGL/QtOpenGL>
#include <QMouseEvent>
#include "tool1.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>
using namespace cv;
class VideoWidget : public QOpenGLWidget
{
    Mat frame;
    int wi;
    int he;
    int tick;
    int pos_x;
    int pos_y;
    QStringList list;
    QList <QRect> rcts;
    void *init_data;
    void *extra_data;
public:
    void set_init_data(void *d)
    {
        init_data=d;
    }

//    VideoWidget();
    VideoWidget(QWidget *w):QOpenGLWidget(w)
    {
        wi=640;
        he=480;
        tick=0;
        pos_x=0;
        pos_y=0;
        rcts.clear();
        rcts.append(QRect(333,333,444,444));
        init_data=NULL;
        click_record=0;
    }
    void set_rects(QByteArray rst)
    {
      // this->width()/640;
           //prt(info," pic width %d",this->width());
//        if(init_data){
//            QString *p=(QString *)init_data;
//            if(p->length()>0){
//                prt(info,"%s",p->toStdString().data());
//            }
//        }
        QString str(rst.data());
        QStringList list=str.split(":");
        QStringList l;
        foreach (QString s, list) {
            l=s.split(',');
            QRect r;
            if(l.size()==4){
                r.setRect(l[0].toInt()*this->width()/wi*2,l[1].toInt()*this->height()/he*2,l[2].toInt()*this->width()/wi*2,l[3].toInt()*this->height()/he*2);
                rcts.append(r);
            }
        }
    }
    int update_mat(Mat frame_mat)
    {
        int size=frame_mat.total();

        if(size>0)
        {
            frame=frame_mat;
                prt(debug,"render set frame  %d %d",he=frame.rows,wi=frame.cols);

        }else{
            prt(info,"render set frame fail");
        }
        this->update();
        return 1;
    }
protected:
    void paintEvent(QPaintEvent *)
    {
        int len=frame.cols;
        if(len>0){
            tick++;
            QPainter painter(this);
            paint_layout1(painter);
            paint_layout2(painter);
            paint_layout4(painter);
                  }else{
            //prt(info,"invalid frame");
        }
    }
    void  initializeGL()
    {

    }

    void paint_layout1(QPainter &painter){
#if 0
        painter.beginNativePainting();
        makeCurrent();
        CONVERT_YUV420PtoRGB24((unsigned char *)yuv_buf,(unsigned char *)rgb_buf,video_width,video_height);
        glPixelZoom(1.0f*this->width()/video_width, 1.0f*this->height()/video_height);
#if 0
        glDrawPixels(video_width, video_height,GL_RGB, GL_UNSIGNED_BYTE, rgb_buf);
#else
        QImage img((uchar *)rgb_buf,video_width,video_height,QImage::Format_RGB888);
        //  QImage img((uchar *)rgb_buf,video_width,video_height,QImage::Format_RGB888);
        //QImage  img = QImage((const uchar*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_Indexed8);
        //   QImage  img = QImage((const uchar*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_Grayscale8);

        painter.drawImage(QRect(0,0,this->width(),this->height()),img);
#endif
        painter.endNativePainting();
#else
        painter.beginNativePainting();
        makeCurrent();
        Mat bgr_frame=frame;
        Mat rgb_frame;
        // imwrite("test.jpg",rgb_frame);
        //  cvtColor(rgb_frame,yuv_frame,CV_RGB2GRAY);
        cvtColor(bgr_frame,rgb_frame,CV_BGR2RGB);
        QImage  img = QImage((const uchar*)(rgb_frame.data),
                             rgb_frame.cols,rgb_frame.rows,
                             rgb_frame.cols*rgb_frame.channels(),
                             QImage::Format_RGB888);
        painter.drawImage(QRect(0,0,this->width(),this->height()),img);
        painter.endNativePainting();

#endif
    }
    void paint_layout2(QPainter &painter){
        //  painter.beginNativePainting();
        // makeCurrent();
        QBrush red_brush_trans(QColor(0,0,200,100));
        painter.setBrush(red_brush_trans);
       // painter.drawEllipse(pos_x++%500,pos_y++%500,50,50);
        pos_x+=10;
        painter.drawEllipse(pos_x%300,0,50,50);
//        foreach (QRect r, rcts) {
//            painter.drawRect(r);
//        }

            if(rcts.size()>0)
            { painter.drawRect(rcts.first());
                 rcts.clear();
            }
        //  painter.drawEllipse(500,500,50,50);
        //    painter.endNativePainting();

    }
    void paint_layout4(QPainter &painter){
        //  painter.beginNativePainting();
        // makeCurrent();
      QBrush red_brush_trans(QColor(110,110,111,0));
        painter.setBrush(red_brush_trans);

          QPen p(QColor(0,100,0),10);
          painter.setPen(p);


       // painter.drawEllipse(pos_x++%500,pos_y++%500,50,50);

        QString s;
        if(init_data){
            QString *p=(QString *)init_data;
            if(p->length()>0){
            //    prt(info,"%s",p->toStdString().data());
                QStringList lst=p->split(',');
                if(lst.length()>=4){
                  //  painter.drawRect(lst[0].,lst[1].toInt(),lst[2].toInt(),lst[3].toInt());
                    QString x=lst[0];
                    QString y=lst[1];
                    QString width=lst[2];
                    QString height=lst[3];
                    painter.drawRect(x.toInt(),y.toInt(),width.toInt(),height.toInt());
                }

            }
        }
      //  painter.drawRect();
//        foreach (QRect r, rcts) {
//            painter.drawRect(r);
//        }

//            if(rcts.size()>0)
//            { painter.drawRect(rcts.first());
//                 rcts.clear();
//            }
        //  painter.drawEllipse(500,500,50,50);
        //    painter.endNativePainting();

    }

    void set_data(QByteArray ba)
    {
        QList<QByteArray> xy=ba.split(',');
        int x=xy[0].toInt();
        int y=xy[1].toInt();
        pos_x=x;
        pos_y=y;
    }
    void paint_layout3(QPainter &painter){
#if 0
        QBrush red_brush_trans(QColor(0,0,200,100));
        painter.setBrush(red_brush_trans);
        // pressed_x=0;
        // pressed_y=0;
        //  if(flg++>10)


        painter.drawEllipse(pressed_x+tick%300,pressed_y,100,100);
        QPen testpen(red_brush_trans,10);
        painter.setPen(testpen);

        for(int i=0;i<8;i++){
            painter.drawLine(pt[i*2],pt[i*2+1]);
        }


        for(int i=0;i<8;i++){
            painter.drawLine(pt[i*2],pt[i*2+1]);
        }

        for(int i=0;i<8;i++){
            painter.drawLine(pt_rear[i*2],pt_rear[i*2+1]);
        }
        painter.drawEllipse(p1,10,10);
        painter.drawEllipse(p2,10,10);
        painter.drawEllipse(p3,10,10);
#endif
    }

    bool get_area(int &x,int &y, int &w, int &h)
    {
        if(init_data){
            QString *p=(QString *)init_data;
            if(p->length()>0){
            //    prt(info,"%s",p->toStdString().data());
                QStringList lst=p->split(',');
                if(lst.length()>=4){
                  //  painter.drawRect(lst[0].,lst[1].toInt(),lst[2].toInt(),lst[3].toInt());
                    QString str_x=lst[0];
                    QString str_y=lst[1];
                    QString str_w=lst[2];
                    QString str_h=lst[3];
                    //painter.drawRect(400,y.toInt(),width.toInt(),height.toInt());
//                    p->clear();
//                    p->append(QString::number(e->x()));
//                    p->append(",");
//                    p->append(QString::number(e->y()));
//                    p->append(",");
//                    p->append(width);
//                    p->append(",");
//                    p->append(height);
                    x=str_x.toInt();
                    y=str_y.toInt();
                    w=str_w.toInt();
                    h=str_h.toInt();
                    return true;
                }

            }
        }
        return false;
    }
    bool set_area(int &x,int &y, int &w, int &h)
    {
        if(init_data){
            QString *p=(QString *)init_data;
            if(p->length()>0){
                //    prt(info,"%s",p->toStdString().data());
                QStringList lst=p->split(',');
                if(lst.length()>=4){
                    p->clear();
                    //  painter.drawRect(lst[0].,lst[1].toInt(),lst[2].toInt(),lst[3].toInt());
//                    QString str_x=lst[0];
//                    QString str_y=lst[1];
//                    QString str_w=lst[2];
//                    QString str_h=lst[3];
                    //painter.drawRect(400,y.toInt(),width.toInt(),height.toInt());
                    //                    p->clear();
                    p->append(QString::number(x));
                    p->append(",");
                    p->append(QString::number(y));
                    p->append(",");
                    p->append(QString::number(w));
                    p->append(",");
                    p->append(QString::number(h));
                }
            }
        }
    }

private:
    void  mousePressEvent(QMouseEvent *e)
    {

//        if(init_data){
//            QString *p=(QString *)init_data;
//            if(p->length()>0){
//            //    prt(info,"%s",p->toStdString().data());
//                QStringList lst=p->split(',');
//                if(lst.length()>=4){
//                  //  painter.drawRect(lst[0].,lst[1].toInt(),lst[2].toInt(),lst[3].toInt());
//                    QString x=lst[0];
//                    QString y=lst[1];
//                    QString width=lst[2];
//                    QString height=lst[3];
//                    //painter.drawRect(400,y.toInt(),width.toInt(),height.toInt());
//                    p->clear();
//                    p->append(QString::number(e->x()));
//                    p->append(",");
//                    p->append(QString::number(e->y()));
//                    p->append(",");
//                    p->append(width);
//                    p->append(",");
//                    p->append(height);
//                }

//            }
//        }
        qDebug()<<e->pos().x();


        int x,y,w,h;
        int new_x=e->x();
        int new_y=e->y();
        int new_w;
        int new_h;

          if(get_area(x,y,w,h)){
               new_w=new_x-x;
               new_h=new_y-y;
              if(e->button()!=Qt::RightButton){
                set_area(new_x,new_y,w,h);
              }else{
                set_area(x,y,new_w,new_h);
              }
          }


    }
    void mouseDoubleClickEvent(QMouseEvent *e)
    {
//        click_record++;
//        int x,y,w,h;
//        int new_x=e->x();
//        int new_y=e->y();
//        int new_w;
//        int new_h;

//          if(get_area(x,y,w,h)){
//               new_w=new_x-x;
//               new_h=new_y-y;
//              if(click_record%2){
//                set_area(new_x,new_y,w,h);
//              }else{
//                set_area(x,y,new_w,new_h);
//              }
//          }
    }
    void mouseReleaseEvent(QMouseEvent *)
    {

    }
    int click_record;
};

#endif // VIDEOWIDGET_H
