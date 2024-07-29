//
//  main.cpp
//  Zport
//  基于时间锁定和操作中台的异构联盟链的跨链数据交互方案。
//  Created by Zarror Wang on 2022/4/16.
//


#include "Zportlib.h"
#include "ZportClient.h"
using namespace std;

//int TestBench_Package_A1()//应用端A,阶段一
//{
//    ZportError* _Zerror=new ZportError();
//    SharedMemory* _s_m=new SharedMemory(_Zerror);
//    Zaffairhub* _Zaffair=new Zaffairhub(_Zerror,_s_m);
//    Chain_Executor* _Chain_ex=new Chain_Executor();
//    Zsocket_sender* _Zsock_sender=new Zsocket_sender(1,_Zerror);
//    Zsocket_receiver* _Zsock_receiver=new Zsocket_receiver(1,_Zerror);
//    Zport* _Zport=new Zport(_Zsock_receiver,_Zsock_sender,1,_Zaffair,_Zerror,_Chain_ex);//Test-1
//    ZportPackage* pak=new ZportPackage;
//    _Zport->Zsock_send->Init_Zsocket_sender("test");
//    string test_pert="";
//    test_pert=Gen_pert();
//    sleep(3);
//    cout<<"<发出测试包1>"<<endl;
//    Gen_Package(pak, 1, 0, _Zport->port_name,test_pert, 0, 1, 1, "Zar", "Tom");
//    _Zport->Zsock_send->Zmessage_sender(pak);
//    Clear_Package(pak);
//    sleep(3);
//    cout<<"<发出测试包2>"<<endl;
//    Gen_Package(pak, 2, 1, _Zport->port_name,test_pert, 0, 1, 1, "Zar", "Tom");
//    _Zport->Zsock_send->Zmessage_sender(pak);
//    Clear_Package(pak);
//    sleep(3);
//    cout<<"<发出测试包3>"<<endl;
//    Gen_Package(pak, 3, 0, _Zport->port_name,test_pert, 0, 1, 1, "Zar", "Tom");
//    _Zport->Zsock_send->Zmessage_sender(pak);
//    Clear_Package(pak);
//    sleep(3);
//    cout<<"<发出测试包4>"<<endl;
//    time_t time_now;
//    time(&time_now);
//    Gen_Package(pak, 4, 0, _Zport->port_name,test_pert, time_now, 1, 1, "Zar", "Tom");
//    _Zport->Zsock_send->Zmessage_sender(pak);
//    Clear_Package(pak);
//    sleep(3);
//    cout<<"<发出测试包5>"<<endl;
//    Gen_Package(pak, 5, 1, _Zport->port_name,test_pert, time_now, 1, 1, "Zar", "Tom");
//    _Zport->Zsock_send->Zmessage_sender(pak);
//    Clear_Package(pak);
//    sleep(3);
//    cout<<"<发出测试包6>"<<endl;
//    Gen_Package(pak, 6, 0, _Zport->port_name,test_pert, time_now, 1, 1 ,"Zar", "Tom");
//    _Zport->Zsock_send->Zmessage_sender(pak);
//    Clear_Package(pak);
//    sleep(3);
//    cout<<"<发出测试包7>"<<endl;
//    Gen_Package(pak, 7, 0, _Zport->port_name,test_pert, 0, 1, 1, "Zar", "Tom");
//    _Zport->Zsock_send->Zmessage_sender(pak);
//    Clear_Package(pak);
//    sleep(3);
//    cout<<"<发出测试包8>"<<endl;
//    Gen_Package(pak, 8, 0, _Zport->port_name,"", 0, 1, 1, "Zar", "Tom");
//    _Zport->Zsock_send->Zmessage_sender(pak);
//    Clear_Package(pak);
//    delete _Zerror;
//    delete _Zsock_sender;
//    delete _Zsock_receiver;
//    delete _Zport;
//    return 1;
//};

void* Zport_thread(void* arg)
{
    Zport* zport=(Zport*) arg;
    zport->Zport_Init();//启动网关，进行事务监听
    pthread_exit(0);
}
void* Moniter_thread(void* arg)
{
    Affair_Moniter* affair_mointer=(Affair_Moniter*) arg;
    affair_mointer->Start_AffairMointer();//启动事务监听
    pthread_exit(0);
}

int TestBench_Package_A3()//应用端A,阶段二
{
    ZportError* _Zerror=new ZportError();//初始化报错类
    SharedMemory* _s_m=new SharedMemory(_Zerror);//初始化共享内存类
    
    Zaffairhub* _Zaffair=new Zaffairhub(_Zerror,_s_m);//初始化事务管理类
    Chain_Executor* _chain_ex=new Chain_Executor(_Zerror);//初始化链上执行类

    Zsocket* _Zsock=new Zsocket(0,_Zerror);//初始化套接字管理类
    Affair_Moniter* _Affair_moniter=new Affair_Moniter(_s_m,_Zerror,_chain_ex,ether,_Zsock);
    
    Init_sock(_Zsock);//创建通信
    Zport* _Zport=new Zport(_Zsock,0,_Zaffair,_Zerror,_chain_ex);//Test-1初始化网关
    //_Zport->Zport_Init();//启动网关，进行事务监听
    
    pthread_t pt_zport,pt_mointer;
    pthread_create(&pt_zport, NULL, Zport_thread, (void*)_Zport);
    pthread_create(&pt_mointer, NULL, Moniter_thread, (void*)_Affair_moniter);
    pthread_join(pt_zport, NULL);
    pthread_join(pt_mointer, NULL);
 
    delete _Zerror;
    delete _s_m;
    delete _Zaffair;
    delete _chain_ex;
    delete _Zsock;
    delete _Affair_moniter;
    delete _Zport;
    return 1;
};

struct server_fun_arg//线程传输结构体
{
    Zport* _Zport;
    Affair_Moniter* _Affair_Moniter;
};

void * server_fun(void* arg)
{
    server_fun_arg* s_f_arg=(server_fun_arg*)arg;
    Zport* _Zport=s_f_arg->_Zport;
    Affair_Moniter* _Affair_Moniter=s_f_arg->_Affair_Moniter;
    pthread_t pt_zport,pt_mointer;
    pthread_create(&pt_zport, NULL, Zport_thread, (void*)_Zport);
    pthread_create(&pt_mointer, NULL, Moniter_thread, (void*)_Affair_Moniter);
    pthread_join(pt_zport, NULL);
    pthread_join(pt_mointer, NULL);
    pthread_exit(0);//退出线程
}

void * client_fun(void* arg)
{
    ZportClient* _ZportClient=(ZportClient*) arg;
    _ZportClient->Start_ZportClient();
    pthread_exit(0);//退出线程
}

int main() {
    //    ZportError* _Zerror=new ZportError();
    //    Zsocket_sender* _Zsock_sender=new Zsocket_sender(1,_Zerror);
    //    Zsocket_receiver* _Zsock_receiver=new Zsocket_receiver(1,_Zerror);
    //    Zport* _Zport=new Zport(_Zsock_receiver,_Zsock_sender);
    //    int pid=fork();
    //    if (pid < 0)
    //    {
    //        cout << "多线程创建失败" << endl;
    //        fflush(stdout);
    //    }
    //    else if(pid==0)//发送进程
    //    {
    //        _Zport->Zport_test_listen();
    //    }
    //    else//接收进程
    //    {
    //        _Zport->Zport_test_send();
    //        return 1;
    //    }
    //    while (wait(0) != -1){}
    //    cout<<"完成"<<endl;
    //    fflush(stdout);
    //    _Zsock_sender->Zmessage_sender("halo");
    //    sleep(7);
    //    _Zsock_sender->Zmessage_sender("quit");
    //    delete _Zerror;
    //    delete _Zsock_sender;
    //TestBench_Package_A2();
    //新一阶段测试
//    ZportError* _Zerror=new ZportError();
//    Zport* _Zport;
//    ZportClient* Z_C=new ZportClient(_Zport,_Zerror);
//    Z_C->Start_ZportClient();
    //TestBench_Package_A3();
    
    ZportError* _Zerror=new ZportError();//初始化报错类
    SharedMemory* _s_m=new SharedMemory(_Zerror);//初始化共享内存类
    
    Zaffairhub* _Zaffair=new Zaffairhub(_Zerror,_s_m);//初始化事务管理类
    Chain_Executor* _chain_ex=new Chain_Executor(_Zerror);//初始化链上执行类
    
    Zsocket* _Zsock=new Zsocket(0,_Zerror);//初始化套接字管理类
    Affair_Moniter* _Affair_Moniter=new Affair_Moniter(_s_m,_Zerror,_chain_ex,ether,_Zsock);
    
    Init_sock(_Zsock);//创建通信
    Zport* _Zport=new Zport(_Zsock,0,_Zaffair,_Zerror,_chain_ex);//初始化网关
    ZportClient* _ZportClient=new ZportClient(_Zport,_Zerror,_Affair_Moniter);//客户端管理类
    
    server_fun_arg* s_f_arg=new server_fun_arg;
    s_f_arg->_Zport=_Zport;
    s_f_arg->_Affair_Moniter=_Affair_Moniter;
    
    pthread_t pt_server_fun,pt_client_fun;
    pthread_create(&pt_client_fun, NULL, server_fun, (void*)s_f_arg);
    pthread_create(&pt_client_fun, NULL, client_fun, (void*)_ZportClient);
    pthread_join(pt_server_fun, NULL);
    pthread_join(pt_client_fun, NULL);
    

    delete _s_m;
    delete _Zaffair;
    delete _chain_ex;
    delete _Zsock;
    delete _Affair_Moniter;
    delete _Zport;
    delete _ZportClient;
    delete _Zerror;
    return 1;
}
