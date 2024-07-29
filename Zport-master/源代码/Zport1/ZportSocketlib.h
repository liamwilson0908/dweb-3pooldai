//
//  ZportSocketlib.h
//  Zport
//
//  Created by Zarror Wang on 2022/4/20.
//

#ifndef ZportSocketlib_h
#define ZportSocketlib_h
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <vector>
#include <map>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string>
#include <string.h>
#include "ZportPackagelib.h"
#include "ZportErrorlib.h"
using namespace std;

class Zsocket //处理两端套接字连接，信息交换服务
{
public: //套接字相关字段信息
    // int socket_r,socket_s;//r表示发送段，s表示接受段
    map<string, vector<string>> peer_socket_info; //记录相邻链的网络地址信息，容器共两维，第一维表示IP，第二维表示port
    string listen_port;                           //本进程监听端口
    map<string,int> sock_id_sender;//记录每个链的传输套接字，因为向多个传输，所以有多个
    int socket_receiver_id;//接收端套接字，因为只监听一个端口，所以只有一个
    ZportError * _Zerror;//报错结构体
    
public:
    Zsocket(int mode, ZportError * _zerror):_Zerror(_zerror) //初始化，建立套接字,mode用于区分不同的端点
    {
        if (mode == 1) //主机一
        {
            peer_socket_info["test"].push_back("127.0.0.1");//IP地址
            peer_socket_info["test"].push_back("5740");//监听端口
            listen_port = "5780";
        }
        else //主机二
        {
            peer_socket_info["test"].push_back("127.0.0.1");
            peer_socket_info["test"].push_back("5780");
            listen_port = "5740";
        }
    };
    int Zmessage_sender(ZportPackage* send_package,string targetChainName) //建立好套接字之后，进行消息传输
    {
        int len=(int)send(this->sock_id_sender[targetChainName], (char*)send_package,sizeof(ZportPackage), 0);
        if(len<=0)//消息发送失败
        {
            _Zerror->error(7,"");
            return -1;
        }
        else
        {
            //_Zerror->package_info(0,send_package);
        }

        return 1;//发送成功
    };
    ~Zsocket()
    {
        _Zerror->msg(0, "释放套接字管理类", "", "", 0);
    }
};


//class Zsocket_sender//负责发送消息的类，参与维护共享内存，以同接受类进程通信
//{
//private:
//    ZportError *Zerror;//报错结构体
//    int socket_sender;//用于发送消息的套接字
//
//public:
//    int Zmessage_sender(ZportPackage* send_package) //建立好套接字之后，进行消息传输
//    {
//        //char message[BUF_SIZE];
//        //bzero(message,BUF_SIZE);
////        zpackage* Zmsg=new zpackage;
////        Zmsg->package_flag=1;
////        time(&Zmsg->time_lock);//记录锁定时间
////        Zmsg->time_lock+=10;//预置时间
////        strcpy(Zmsg->message,messasge_value.c_str());
//        cout<<"发送进程准备发出消息到"<<this->peer_socket_info["test"][0]<<this->peer_socket_info["test"][1]<<endl;
//        fflush(stdout);
//        int len=(int)send(socket_sender, (char*)send_package,sizeof(ZportPackage), 0);
//        if(len<=0)//消息发送失败
//        {
//            Zerror->error(7,"");
//            return -1;
//        }
//        else
//        {
//            Zerror->package_info(0,send_package);
//        }
//        if(send_package->package_flag==8)//消息发送结束
//        {
//            Zerror->msg(3, "","","",0);
//            close(this->socket_sender);
//            return 1;
//        }
//        return 1;//发送成功
//    };
//    int Init_Zsocket_sender(string targetChainName) //具体初始化套接字<socket_r>//目标链名称，涉及到传输信息配置
//    {
//        int socket_sender;
//        struct sockaddr_in socket_sender_addr;           //套接字连接地址信息
//        socket_sender = socket(PF_INET, SOCK_STREAM, 0); //申请<socket_sender>套接字
//        if (socket_sender == -1)                         //套接字<socket_sender>创建失败
//        {
//            Zerror->error(1,"");
//            return -1;
//        }
//        memset(&socket_sender_addr, 0, sizeof(socket_sender_addr)); //设置<socket_sender>连接参数
//        socket_sender_addr.sin_family = AF_INET;
//        socket_sender_addr.sin_addr.s_addr = inet_addr(peer_socket_info[targetChainName][0].c_str());
//        socket_sender_addr.sin_port = htons(atoi(peer_socket_info[targetChainName][1].c_str()));
//        Zerror->msg(1,peer_socket_info[targetChainName][1],"","",0);
//        if (connect(socket_sender, (struct sockaddr *)&socket_sender_addr, sizeof(socket_sender_addr)) == -1)
//        {
//            Zerror->error(2,"");
//            return -1;
//        }
//        this->socket_sender=socket_sender;
//        cout<<"sneder进程连接成功"<<endl;
//        fflush(stdout);
//        return 1;
//    };
//    Zsocket_sender(int mode,ZportError *_Zerror):Zsocket(mode, _Zerror)//构造函数
//    {
//        Zerror=_Zerror;
//    }
    
//
//};



//class Zsocket_receiver//负责接收消息的类，参与维护共享内存，以同发送类进程通信
//{
//private:
//    int socket_receiver_service;
//    ZportError *Zerror;//报错结构体
//public:
//    time_t time_lock_stamp;
//    int pipe_symbol[2];//管道通信符号
//
//public:
//    int Zmessgae_receiver() //监听函数功能分发
//    {
//        time_t time_y;
//        time(&time_y);
//        cout<<ctime(&time_y)<<"循环监听地址"<<this->listen_port<<endl;
//        fflush(stdout);
//        while (1)
//        {
//            ZportPackage* pak=new ZportPackage;
//            pak->package_flag=-1;
//            read(socket_receiver_service, (char*)pak, sizeof(ZportPackage));
//            //Zerror->package_info(1,pak);
//            //加入事务逻辑,通过管道传输,传输到主进程
//            if(pak->package_flag!=-1)
//            {
//                cout<<"收到数据包"<<endl;
//                fflush(stdout);
//                close(this->pipe_symbol[0]);
//                write(this->pipe_symbol[1],(char*)pak,sizeof(ZportPackage));
//                close(this->pipe_symbol[1]);
//            }
//            if(pak->package_flag==7)
//            {
//                Zerror->msg(4, "","","",0);
//                delete pak;
//                break;
//            }
//            delete pak;
//        }
//        close(socket_receiver_service);
//        //return 1;
//        exit(0);
//    };
//    Zsocket_receiver(int mode,ZportError *_Zerror):Zsocket(mode, _Zerror)
//    {
//        Zerror=_Zerror;
//        this->time_lock_stamp=0;
//        //this->Init_Zsocket_receiver();
//    }
//    int Init_Zsocket_receiver() //初始化监听函数以及监听本机端口
//    {
//        int socket_receiver, socket_receiver_service; // socket_receiver_service为专门处理服务设计
//        struct sockaddr_in socket_receiver_addr, socket_receiver_service_addr;
//        socklen_t socket_receiver_service_size;
//        socket_receiver = socket(PF_INET, SOCK_STREAM, 0);
//        if (socket_receiver == -1) //套接字<socket_receiver>创建失败
//        {
//            Zerror->error(3,"");
//            return -1;
//        }
//
//        memset(&socket_receiver_addr, 0, sizeof(socket_receiver_addr));
//        socket_receiver_addr.sin_family = AF_INET;
//        socket_receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);         //自动获取计算机的IP地址
//        socket_receiver_addr.sin_port = htons(atoi(listen_port.c_str())); // atoi (表示ascii to integer)是把字符串转换成整型数的一个函数
//        Zerror->msg(2, listen_port,"","",0);
//        if (::bind(socket_receiver, (struct sockaddr *)&socket_receiver_addr, sizeof(socket_receiver_addr)) == -1) //绑定<socket_receiver>监听地址
//        {
//            Zerror->error(4,"");
//            perror("bind");
//            fflush(stdout);
//            return -1;
//        }
//        if (listen(socket_receiver, 5) == -1)
//        {
//            Zerror->error(5,"");
//            return -1;
//        }
//        socket_receiver_service_size = sizeof(socket_receiver_service_addr);
//        socket_receiver_service = accept(socket_receiver, (sockaddr *)&socket_receiver_addr, &socket_receiver_service_size);
//        if (socket_receiver_service == -1)
//        {
//            Zerror->error(6,"");
//            perror("accept");
//            fflush(stdout);
//            return -1;
//        }
//        this->socket_receiver_service=socket_receiver_service;
//        time_t time_y;
//        time(&time_y);
//        cout<<ctime(&time_y)<<"绑定receiver进程监听地址成功"<<endl;
//        fflush(stdout);
//        return 1; //返回建立好的套接字
//    };
//};



struct Zsocket_sender_thread_arg{
    string tatgetChainName;
    Zsocket* _Zsock;
};

struct Zsocket_receiver_thread_arg{
    Zsocket* _Zsock;
    int pipe_symbol[2];//管道通信符号
};//用于创建轮询监听的套接字线程






void *Init_Zsocket_sender(void* arg)
{
//    cout<<"创建发送初始化线程"<<endl;
//    fflush(stdout);
    sleep(10);
    string targetChainName=((Zsocket_sender_thread_arg*)arg)->tatgetChainName;
    Zsocket* _Zsock=((Zsocket_sender_thread_arg*)arg)->_Zsock;
    int socket_sender;
    struct sockaddr_in socket_sender_addr;           //套接字连接地址信息
    socket_sender = socket(PF_INET, SOCK_STREAM, 0); //申请<socket_sender>套接字
    if (socket_sender == -1)                         //套接字<socket_sender>创建失败
    {
        _Zsock->_Zerror->error(1,"");
        //return -1;
    }
    memset(&socket_sender_addr, 0, sizeof(socket_sender_addr)); //设置<socket_sender>连接参数
    socket_sender_addr.sin_family = AF_INET;
    socket_sender_addr.sin_addr.s_addr = inet_addr(_Zsock->peer_socket_info[targetChainName][0].c_str());
    socket_sender_addr.sin_port = htons(atoi(_Zsock->peer_socket_info[targetChainName][1].c_str()));
    _Zsock->_Zerror->msg(1,_Zsock->peer_socket_info[targetChainName][1],"","",0);
    if (connect(socket_sender, (struct sockaddr *)&socket_sender_addr, sizeof(socket_sender_addr)) == -1)
    {
        _Zsock->_Zerror->error(2,"");
        pthread_exit(0);
    }
    _Zsock->sock_id_sender[targetChainName]=socket_sender;//将套接字记录在map区中
//    cout<<"发送连接准备完成"<<endl;
//    fflush(stdout);
    pthread_exit(0);
}
void * Init_Zsocket_receiver(void* arg) //初始化监听函数以及监听本机端口
{
//    cout<<"创建接受初始化线程"<<endl;
//    fflush(stdout);
    Zsocket* _Zsock=(Zsocket*)arg;
    int socket_receiver, socket_receiver_service; // socket_receiver_service为专门处理服务设计
    struct sockaddr_in socket_receiver_addr, socket_receiver_service_addr;
    socklen_t socket_receiver_service_size;
    socket_receiver = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_receiver == -1) //套接字<socket_receiver>创建失败
    {
        _Zsock->_Zerror->error(3,"");
        pthread_exit(0);
    }
    
    memset(&socket_receiver_addr, 0, sizeof(socket_receiver_addr));
    socket_receiver_addr.sin_family = AF_INET;
    socket_receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);         //自动获取计算机的IP地址
    socket_receiver_addr.sin_port = htons(atoi(_Zsock->listen_port.c_str())); // atoi (表示ascii to integer)是把字符串转换成整型数的一个函数
    _Zsock->_Zerror->msg(2, _Zsock->listen_port,"","",0);
    if (::bind(socket_receiver, (struct sockaddr *)&socket_receiver_addr, sizeof(socket_receiver_addr)) == -1) //绑定<socket_receiver>监听地址
    {
        _Zsock->_Zerror->error(4,"");
        pthread_exit(0);
    }
    if (listen(socket_receiver, 5) == -1)
    {
        _Zsock->_Zerror->error(5,"");
        pthread_exit(0);
    }
    socket_receiver_service_size = sizeof(socket_receiver_service_addr);
    socket_receiver_service = accept(socket_receiver, (sockaddr *)&socket_receiver_addr, &socket_receiver_service_size);
    if (socket_receiver_service == -1)
    {
        _Zsock->_Zerror->error(6,"");
        pthread_exit(0);
    }
    _Zsock->socket_receiver_id=socket_receiver_service;
//    cout<<"监听连接准备完成"<<endl;
//    fflush(stdout);
    pthread_exit(0);
};





int Init_sock(Zsocket* _Zsock)//多进程创建链接
{
    //不可以用多进程，这样会导致代码都次运行，应该用线程，共享进程资源，释放时不会释放资源
//    int pid;
//    pid=fork();
//
//    if(pid==-1)
//    {
//        cout<<"通信进程创建失败"<<endl;
//    }
//    else if(pid==0)
//    {
//        _sender->Init_Zsocket_sender("test");
//        printf("child returned\n");
//        fflush(stdout);
//        //exit(0);
//    }
//    else
//    {
//        _receiver->Init_Zsocket_receiver();
//        wait(NULL);
//        printf("father returned\n");
//        fflush(stdout);
//    }
//    thread thread_sender(&Zsocket_sender::Init_Zsocket_sender,&_sender,"test");
//    thread_sender.detach();
//    thread thread_recv(&Zsocket_receiver::Init_Zsocket_receiver,&_receiver,NULL);
//    thread_recv.detach();

    pthread_t pt_sender,pt_recv;
    Zsocket_sender_thread_arg* pt_sender_arg=new Zsocket_sender_thread_arg;
    pt_sender_arg->tatgetChainName="test";
    pt_sender_arg->_Zsock=_Zsock;
    pthread_create(&pt_sender, NULL, Init_Zsocket_sender, (void*)pt_sender_arg);
    pthread_create(&pt_recv, NULL, Init_Zsocket_receiver, (void*)_Zsock);
    pthread_join(pt_recv, NULL);//等待
    pthread_join(pt_sender, NULL);//等待
    time_t time_tmp;
    time(&time_tmp);
    _Zsock->_Zerror->msg(0, "接收与发送初始化线程完成", "", "", 0);
    return 1;
}


#endif /* ZportSocketlib_h */
