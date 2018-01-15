#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QObject>
#include <QtNetwork>
#include <QMessageBox>
#include <QTcpSocket>
#include <QDataStream>
#include <QFile>
#include <QThread>
#include "pd.h"
#include "tool1.h"
#include <thread>
#include <QThread>
using namespace std;
class ServerReplyCheckRouting : public QObject
{
    Q_OBJECT

public slots:
    void check_reply(    QUdpSocket *udp_skt_find_server) {
        QString str;
        str.clear();
        int try_times=100;
        while(try_times--){
            if(udp_skt_find_server->hasPendingDatagrams()){
                datagram.resize((udp_skt_find_server->pendingDatagramSize()));
                udp_skt_find_server->readDatagram(datagram.data(),datagram.size());
                prt(info,"get server info : %s",datagram.data());
                server_ip.clear();
                server_ip.append(datagram.split(',')[0]);
                prt(info,"ip : %s",server_ip.toStdString().data());
                ip_list.append(server_ip);
                emit resultReady(server_ip);
            }else{
                //      prt(info,"searching");
            }
            QThread::msleep(10);
        }
    }

signals:
    void resultReady(  QString result);
private:
    //  QUdpSocket *udp_skt_find_server;

    QByteArray datagram;
    QString server_ip;
    QStringList ip_list;
};

class ServerInfoSearcher : public QObject{
    Q_OBJECT
    QThread check_thread;
    ServerReplyCheckRouting *p_checker;

public:
    ServerInfoSearcher()
    {

        udp_skt_find_server=new QUdpSocket(this);
        udp_skt_find_server->bind(Protocol::CLIENT_REPORTER_PORT,QUdpSocket::ShareAddress);
        //     connect(udp_skt_find_server,SIGNAL(readyRead()),this,SLOT(get_reply()),Qt::QueuedConnection);

        //   connect();

        p_checker=new ServerReplyCheckRouting;
        p_checker->moveToThread(&check_thread);
        connect(&check_thread,&QThread::finished,p_checker,&QObject::deleteLater);
        connect(this,SIGNAL(begin_search(QUdpSocket*)),p_checker,SLOT(check_reply(QUdpSocket*)),Qt::QueuedConnection);
        connect(p_checker,SIGNAL(resultReady(QString)),this,SLOT(ip_found(QString)),Qt::QueuedConnection);
    }
    ~ServerInfoSearcher()
    {
        check_thread.quit();
        check_thread.wait();
    }
    void broadcast_info()
    {
        QByteArray b;
        b.append("pedestrian");
        udp_skt_find_server->writeDatagram(b.data(), b.size(),
                                           QHostAddress::Broadcast, Protocol::SERVER_REPORTER_PORT);
        prt(info,"finding server ...");

    }
    void search()
    {

        check_thread.start();
        emit begin_search(udp_skt_find_server);
    }

    void search_device()
    {
        ip_list.clear();
        broadcast_info();
        search();
        //   p_find_server_thread=new std::thread(find_server);
        //   QThread::msleep(3000);

    }
    QStringList search_rst()
    {
        return ip_list;
    }
    static void find_server()
    {
        prt(info," find server thread");
        int times=10;
        while(times--){

        }
    }
signals:
    void begin_search( QUdpSocket *udp_skt_find_server);
    void find_ip(QString ip);

public slots:
    void ip_found(QString ip)
    {
        qDebug()<<ip;
        emit find_ip(ip);
    }

    void get_reply()
    {
        //  while(udp_skt->hasPendingDatagrams())
        if(udp_skt_find_server->hasPendingDatagrams())
        {
            datagram.resize((udp_skt_find_server->pendingDatagramSize()));
            udp_skt_find_server->readDatagram(datagram.data(),datagram.size());
            prt(info,"get server info : %s",datagram.data());
            server_ip.append(datagram.split(',')[0]);
            ip_list.append(server_ip);
        }
    }

private :
    QUdpSocket *udp_skt_find_server;

    QByteArray datagram;
    QString server_ip;
    QStringList ip_list;
};
class ProcessedDataReciver: public QObject{
    Q_OBJECT
public:
    ProcessedDataReciver()
    {
        udp_skt_alg_output=new QUdpSocket(this);
        udp_skt_alg_output->bind(Protocol::SERVER_DATA_OUTPUT_PORT,QUdpSocket::ShareAddress);
        connect(udp_skt_alg_output,SIGNAL(readyRead()),this,SLOT(get_rst()),Qt::QueuedConnection);
    }
signals:
    void send_rst(QByteArray);
public slots:
    void get_rst()
    {
        QByteArray datagram_rst;
        if(udp_skt_alg_output->hasPendingDatagrams())
        {
            //   int size=udp_skt_alg_output->pendingDatagramSize();
            datagram_rst.resize((udp_skt_alg_output->pendingDatagramSize()));
            udp_skt_alg_output->readDatagram(datagram_rst.data(),datagram_rst.size());
            //        udp_skt_alg_output->readDatagram(sss,500);
            //          datagram_rst= udp_skt_alg_output->readAll();
#if 0
            QList <QByteArray > bl= datagram_rst.split(':');
            QByteArray b_index=bl[0];
            int index=*(b_index);

            prt(info,"get cam   %d rst",index);

            QByteArray b_loc=bl[1];
#else

            prt(debug,"get data %s",datagram_rst.data());
            emit send_rst(datagram_rst);
#endif

            //   emit send_camera_rst(index,b_loc);
            //    QList <QByteArray > xy= b_loc.split(',');
            //            int x=xy[0].toInt();
            //            int y=xy[1].toInt();
            //           prt(info," %d : %d",x,y);

        }
    }

private:
    QUdpSocket *udp_skt_alg_output;
    QByteArray rst;
};

class Client : public QObject
{
    Q_OBJECT
public:
    QString server_ip;
    Client(){
        need_read=false;
        server_ip.clear();
        tcp_socket=new QTcpSocket();
        in.setDevice(tcp_socket);
        in.setVersion(QDataStream::Qt_1_0);
        connect(tcp_socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
        connect(tcp_socket,SIGNAL(readyRead()),this,SLOT(handle_server_msg()),Qt::DirectConnection);
        connect(tcp_socket,SIGNAL(connected()),this,SLOT(handle_connected()),Qt::DirectConnection);
    }
    void pack_tcp_data(char *c,int length){

        quint16  *pos_version=( quint16  *)c+2;
        quint16 *pos_length=( quint16  *)c;
        *pos_version=1;
        *pos_length=length;
    }
    char buf[10000];
    void get_client_setting(){
        qDebug()<<"try to get server setting";
        char *data=buf;
        quint16 *pos_op=(quint16 *)(data+4);
        *pos_op=0;

        pack_tcp_data(buf,0);
        int ret=tcp_socket->write(buf,6);
        qDebug()<<"ret "<<ret;
    }

    void read_msg()
    {
        static  char buf[100];
        int ret=  in.readRawData((char *)buf,3);
        prt(info,"read msg now ");
        qDebug()<<QString::fromRawData((QChar *)buf,3);
    }
    QByteArray get_config()
    {
        char buf[2000];
        int request_length=Protocol::encode_configuration_request(buf);//encoder buffer
        QByteArray rst= call_server(buf,request_length);//talk to server
        return rst.remove(0,Protocol::HEAD_LENGTH);//TODO:get the ret value;
    }

    QByteArray  call_server(char *buf,int len)
    {

        QByteArray ret;
        ret.resize(0);
        int write_bytes=0;
        prt(info,"state %d ",tcp_socket->state());
        write_bytes=tcp_socket->write(buf,len);
        return ret;
    }


signals:
    // void send_camera_rst(int index, QByteArray ba);
public slots:
    void handle_connected()
    {
        prt(info,"connetde now");
    }

    void handle_server_msg()
    {
        prt(info,"get msg from server");
        lock.lock();
        ret_ba=tcp_socket->readAll();
        if(ret_ba.size()>0)
            need_read=true;
        lock.unlock();
        prt(info,"state %d",tcp_socket->state());
        int op=Protocol::decode_head_op(ret_ba.data());
        if(ret_ba.size()>0)
            emit server_msg(ret_ba.remove(0,Protocol::HEAD_LENGTH),Protocol::get_operation(ret_ba.data()));
    }

    void connect_to_server(QString ip)
    {
        prt(info,"conneting %s",server_ip.toStdString().data());
        if(ip.size()){
            prt(info,"state %d ",tcp_socket->state());
            if(tcp_socket->state()==QAbstractSocket::ConnectedState)
                tcp_socket->disconnectFromHost();
            prt(info,"new ip %s connect (%d)",server_ip.toStdString().data(),tcp_socket->state());
            if(ip==server_ip){
                prt(info,"same ip reconnect");
                tcp_socket->connectToHost(server_ip,Protocol::SERVER_PORT);
            }else{
                server_ip=ip;
                tcp_socket->connectToHost(server_ip,Protocol::SERVER_PORT);
                //                while(1){
                //                    prt(info,"new ip %s connect (%d)",server_ip.toStdString().data(),tcp_socket->state());
                //                    QThread::sleep(1);
                //                }
            }
            prt(info,"new ip %s connect (%d)",server_ip.toStdString().data(),tcp_socket->state());
            prt(info,"state %d ",tcp_socket->state());
        }else{
            server_ip=ip;
            tcp_socket->connectToHost(server_ip,Protocol::SERVER_PORT);
        }
        prt(info,"connecting to %s",server_ip.toStdString().data());
    }
    void  displayError(QAbstractSocket::SocketError socketError)
    {
        prt(info,"err when connecting to server");
        switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            qDebug()<<"1";
            break;
        case QAbstractSocket::ConnectionRefusedError:
            qDebug()<<"1";
            break;
        default:
            qDebug()<<"1";
        }
    }
signals:
    void server_msg(QByteArray msg,int);
private:
    mutex lock;
    QByteArray ret_ba;
    bool need_read;


    QTcpSocket *tcp_socket;
    QDataStream in;
    QFile *f;
    QUdpSocket *udp_skt_find_server;
};

#endif // CLIENT_H
