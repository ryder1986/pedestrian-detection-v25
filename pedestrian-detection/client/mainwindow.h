#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_form.h"
#include "client.h"
//#include "videoprocessor.h"
#include "config.h"
#include "videosrc.h"
#include <QMenu>
#include "searchwidget.h"
using namespace std ;
class VideoThread:public QObject{
    Q_OBJECT
    typedef struct data{
        VideoSrc *p_src;
        QString url;
        bool quit;
        VideoWidget *video_render;
        QByteArray rst;
        mutex lock;
    }data_t;
    data_t d;

private:
    static void fun(data_t *p_data)
    {
        p_data->p_src=new VideoSrc(p_data->url.toStdString().data());
        Mat mt;
        bool flg;
        while(!p_data->quit){

            flg=p_data->p_src->fetch_frame(mt);
            if(flg){
                p_data->lock.lock();
                if(p_data->rst.size()>0)
                    p_data->video_render->set_rects(p_data->rst);
                p_data->rst.clear();
                p_data->lock.unlock();
                p_data->video_render->update_mat(mt);

            }
            this_thread::sleep_for(chrono::milliseconds(30));
        }
        delete p_data->p_src;
    }

public:
    VideoThread(QString url,VideoWidget *widget)
    {
        d.quit=false;
        d.p_src=NULL;
        d.url=url;
        d.video_render=widget;
        p_thread=new std::thread(fun,&d);
    }
    ~VideoThread()
    {
        d.quit=true;
        p_thread->join();;
        delete p_thread;
        p_thread;
    }
public slots:
    void get_data(QByteArray rst)
    {
        prt(debug,"get %s",rst.toStdString().data());
#if 1
        d.lock.lock();
        d.rst.clear();
        d.rst=rst;
        d.lock.unlock();
#else
        QString str(rst.data());
        QStringList list=str.split(":");
        QStringList l;
        foreach (QString s, list) {
            l=s.split(',');
            QRect r;
            r.setRect(l[0].toInt(),l[1].toInt(),l[2].toInt(),l[3].toInt());

            // prt(info,"#### %s",l[0].toStdString().data());
        }
#endif
    }

private:
    std::thread *p_thread;
    // VideoProcessor *p_pro;

};

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_pushButton_search_clicked();

    void on_treeWidget_devices_doubleClicked(const QModelIndex &index);

    void on_treeWidget_devices_customContextMenuRequested(const QPoint &pos);


    void add_camera(bool checked)
    {
        QTreeWidgetItem *tmp_item=new QTreeWidgetItem(QStringList("edit here"));
        p_item_device_root->addChild(tmp_item);
        tmp_item->setFlags(Qt::ItemIsEditable | tmp_item->flags()) ;
        p_item_device_root->setExpanded(true);
    }

    void del_camera(bool checked)
    {
        if(p_video_thread)
        {

            delete p_video_thread;
            p_video_thread=NULL;
        }


        qDebug()<<"del camera "<<checked;
        int now=p_item_device_current->parent()->indexOfChild(p_item_device_current);
        if(now+1>p_cfg->cfg.camera_amount)//remove camera which didnt submit yet
        {
            p_item_device_root->removeChild(p_item_device_current);
        }else{//remove exsited camera
            int del_index=p_item_device_current->parent()->indexOfChild(p_item_device_current)+1;
            if(selected_camera_index>0){//playing
                if(selected_camera_index==del_index)
                {
                    prt(info,"stop playing %d",del_index);
                    //  window->openGLWidget->stop();//TODO:stop playing
                    selected_camera_index=0;//means no camera playing
                }
            }
            //  int size=cam_manager->get_size();
            int size = p_cfg->cfg.camera_amount;
            if(del_index<=size&&del_index>0)
            {
#if 0
                char buf[2000];
                //cam_manager->del_camera(del_index);// delete camera local
                p_cfg->del_camera(del_index);
                Protocol::encode_delcam_request(buf,del_index);//encode buffer
                clt->call_server(buf,Protocol::HEAD_LENGTH);//talk to server
#else
                submit_camera_deling(del_index);
#endif
            }
            p_item_device_root->removeChild(p_item_device_current);

        }
    }

    void submit_camera_adding(bool checked)
    {
        char buf[2000];
        QString ip=p_item_device_current->text((0));
        p_cfg->append_camera(ip);
        QByteArray setting=p_cfg->get_config();//get new config from local database
        int len=Protocol::encode_addcam_request(buf,setting.length());//encode buffer
        memcpy(buf+Protocol::HEAD_LENGTH,setting.data(),setting.length());
        QByteArray rst=clt->call_server(buf,len);//talk to server
        // itm->setFlags(!Qt::ItemIsEditable | itm_root->flags()) ;
        p_item_device_current->setFlags(  p_item_device_current->flags() & ~Qt::ItemIsEditable) ;
    }
    void submit_camera_deling(int del_index)
    {
        char buf[2000];
        p_cfg->del_camera(del_index);
        Protocol::encode_delcam_request(buf,del_index);//encode buffer
        clt->call_server(buf,Protocol::HEAD_LENGTH);//talk to server
    }
    void open_camera_output(int index)
    {
        char buf[2000];
        int len=Protocol::encode_output_request(buf,index);//encode buffer
        QByteArray rst=clt->call_server(buf,len);
    }
public slots:
    void connect_server(QString ip)
    {
        prt(info,"connect %s",ip.toStdString().data());
        if(ip.length()){

            clt->connect_to_server(ip);

            //if connect ok ,then continue;
            //             int request_length=Protocol::encode_configuration_request(buf);//encoder buffer
            //             QByteArray rst=clt->call_server(buf,request_length);//talk to server
            //             rst=rst.remove(0,Protocol::HEAD_LENGTH);//TODO:get the ret value

            clt->get_config();
#if 0
            p_cfg->set_config( clt->get_config());

            //handle tree list
            window->treeWidget_devices->clear();
            p_item_device_root=new QTreeWidgetItem(QStringList(clt->server_ip));
            window->treeWidget_devices->addTopLevelItem(p_item_device_root);
            for(int i=0;i<p_cfg->cfg.camera_amount;i++){
                QTreeWidgetItem *itm1=new QTreeWidgetItem(QStringList(p_cfg->cfg.camera[i].ip));
                p_item_device_root->addChild(itm1);
            }
#endif
        }else{
            prt(info,"no server found");
        }
    }

    void handle_msg(QByteArray rst,int cmd)
    {
        qDebug()<<"get cmd "<<cmd;


        switch (cmd) {
        case Protocol::CMD::NEED_UPDATE:
        {
//            QMessageBox  *message=new QMessageBox(QMessageBox::Warning,"Information","update?",QMessageBox::Yes|QMessageBox::No,NULL);
//            connect(message,SIGNAL(accepted()),this,SLOT(clear_server_dialog()));
//            message->show();
            box1->show();
           // window->treeWidget_devices->clear();
        }
            break;
        case Protocol::CMD::GET_CONFIG:
            p_cfg->set_config( rst);
            //handle tree list
            window->treeWidget_devices->clear();
            p_item_device_root=new QTreeWidgetItem(QStringList(clt->server_ip));
            window->treeWidget_devices->addTopLevelItem(p_item_device_root);
            for(int i=0;i<p_cfg->cfg.camera_amount;i++){
                QTreeWidgetItem *itm1=new QTreeWidgetItem(QStringList(p_cfg->cfg.camera[i].ip));
                p_item_device_root->addChild(itm1);
            }
            break;
        default:
            break;
        }

    }
    void clear_server_dialog()
    {
        qDebug()<<"clear info";
        window->treeWidget_devices->clear();
    }

private:
    Ui::Form *window;
    ServerInfoSearcher *searcher;
    ServerOutputRst *rst_rcver;
    Client *clt;
    CameraConfiguration *p_cfg;
    QTreeWidgetItem *p_item_device_root;
    QTreeWidgetItem *p_item_device_current;
    int selected_camera_index;
    VideoThread *p_video_thread;
    SearchWidget *search_widget;
    QMessageBox  *box1;

};

#endif // MAINWINDOW_H
