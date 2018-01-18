#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    window=new Ui::Form;
    window->setupUi(this);
    p_cfg=new CameraConfiguration("config.json");
    searcher=new ServerInfoSearcher;
    rst_rcver=new ProcessedDataReciver ;
    clt=new Client;
    p_video_thread=NULL;
    search_widget=new SearchWidget;
    connect(search_widget,SIGNAL(select_ip(QString)),this,SLOT(connect_server(QString)),Qt::QueuedConnection);
    connect(searcher,SIGNAL(find_ip(QString)),search_widget,SLOT(add_text(QString)),Qt::QueuedConnection);
    connect(clt,SIGNAL(server_msg(QByteArray,int )),this,SLOT(handle_msg(QByteArray,int )),Qt::QueuedConnection);
    box1=new QMessageBox(QMessageBox::Warning,"Information","update?",QMessageBox::Yes|QMessageBox::No,NULL);
    connect(box1,SIGNAL(accepted()),this,SLOT(clear_server_dialog()));

}

MainWindow::~MainWindow()
{

}

void MainWindow::on_pushButton_search_clicked()
{
    if(p_video_thread)
    {

        delete p_video_thread;
        p_video_thread=NULL;
    }

    search_widget->clear_text();
    //   search_widget->setWindowState(Qt::WindowState);
    int w=search_widget->width();
    int h=search_widget->height();
    int x=this->x();
    int y=this->y();

    search_widget->setGeometry((this->width()-w)/2+x,(this->height()-h)/2+y,w,h);
    search_widget->clear_text();
     search_widget->setWindowFlags(Qt::WindowStaysOnTopHint);
    search_widget->show();

    //  char buf[2000];
    searcher->search_device();
    //   QThread::msleep(2000);
    //    QMessageBox *b=new QMessageBox;

    //    b->setButtonText(0,"123");
    //    b->show();
    // QThread::sleep(3);
    //    QString ip=searcher->wait_server_info_reply(2);
    //       QStringList l= searcher->search_rst();
    //    foreach (QString str, l) {
    //        search_widget->add_text(str);
    //    }
    //  delete b;
    // search_widget->add_text(ip);


    //    if(ip.length()){
    //        clt->connect_to_server(ip);
    //        //if connect ok ,then continue;
    //        int request_length=Protocol::encode_configuration_request(buf);//encoder buffer
    //        QByteArray rst=clt->call_server(buf,request_length);//talk to server
    //        rst=rst.remove(0,Protocol::HEAD_LENGTH);//TODO:get the ret value
    //        p_cfg->set_config(rst);

    //        //handle tree list
    //        window->treeWidget_devices->clear();
    //        p_item_device_root=new QTreeWidgetItem(QStringList(clt->server_ip));
    //        window->treeWidget_devices->addTopLevelItem(p_item_device_root);
    //        for(int i=0;i<p_cfg->cfg.camera_amount;i++){
    //            QTreeWidgetItem *itm1=new QTreeWidgetItem(QStringList(p_cfg->cfg.camera[i].ip));
    //            p_item_device_root->addChild(itm1);
    //        }
    //    }else{
    //        prt(info,"no server found");
    //    }
}

void MainWindow::on_treeWidget_devices_doubleClicked(const QModelIndex &index)
{
    p_item_device_current=  window->treeWidget_devices->currentItem();
    if(p_item_device_current){
        if(!p_item_device_current->parent()){//root
        }else{//child
#if 0
            int now=p_item_device_current->parent()->indexOfChild(p_item_device_current);
            selected_camera_index=now+1;

            prt(info,"playing index change to %d",selected_camera_index);
            if(now<p_cfg->cfg.camera_amount){
                QString url=p_item_device_current->text(0);
                qDebug()<<"get "<<url;
                open_camera_output(selected_camera_index);
                if(p_video_thread)
                {
                    delete p_video_thread;
                    p_video_thread=NULL;
                }
                //  else
                VideoThread::init_data_t dat;
                dat.url=url;
                dat.render_init_data=(void *)&p_cfg->cfg.detect_area;
               // p_video_thread=new VideoThread(url,window->openGLWidget);
                p_video_thread=new VideoThread(dat,window->openGLWidget);
                connect(rst_rcver,SIGNAL(send_rst(QByteArray)),p_video_thread,SLOT(get_data(QByteArray)));
                connect(p_video_thread,SIGNAL(check_rst(int,int)),this,SLOT(show_process_record(int,int)),Qt::DirectConnection);
                //                f->openGLWidget->render_set_mat(mat);
                //                f->openGLWidget->update();
                // window->openGLWidget->start(p_item_device_current->text(0));//TODO:start playing
            }
#endif
        }
    }
}

void MainWindow::on_treeWidget_devices_customContextMenuRequested(const QPoint &pos)
{
#if 1
    p_item_device_current= window->treeWidget_devices->itemAt(pos);
    if(p_item_device_current){
        if(!p_item_device_current->parent()){
            QMenu *cmenu = new QMenu(window->treeWidget_devices);
            QAction *action_add = cmenu->addAction("add new camera");
            connect(action_add, SIGNAL(triggered(bool)), this, SLOT(add_camera(bool)));
            cmenu->exec(QCursor::pos());
        }else{
            QMenu *cmenu = new QMenu(window->treeWidget_devices);
            QAction *action_del = cmenu->addAction("del this camera");

            int now=p_item_device_current->parent()->indexOfChild(p_item_device_current);
            if(now+1>p_cfg->cfg.camera_amount)
            {
                QAction *action_submit = cmenu->addAction("confirm add this cammera");
                connect(action_submit, SIGNAL(triggered(bool)), this, SLOT(submit_camera_adding(bool)));
            }else{
                QAction *action_mod = cmenu->addAction("confirm modified cammera");
                connect(action_mod, SIGNAL(triggered(bool)), this, SLOT(confirm_camera_modify(bool)));
                QAction *action_preview = cmenu->addAction("preview cammera");
                connect(action_preview, SIGNAL(triggered(bool)), this, SLOT(preview_camera(bool)));
            }
            connect(action_del, SIGNAL(triggered(bool)), this, SLOT(del_camera(bool)));
            cmenu->exec(QCursor::pos());
        }
    }
#else
    QMenu *cmenu = new QMenu(f->treeWidget_devices);
    QAction *action_add = cmenu->addAction("add");

    connect(action_add, SIGNAL(triggered(bool)), this, SLOT(add_camera(bool)));

    cmenu->exec(QCursor::pos());
#endif

}

void MainWindow::on_openGLWidget_customContextMenuRequested(const QPoint &pos)
{
    prt(info,"%d,%d",pos.x(),pos.y());

}
