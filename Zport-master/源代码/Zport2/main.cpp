//
//  main.cpp
//  Zport_test
//
//  Created by Zarror Wang on 2022/4/17.
//请注意port2需要使用与port1同样的头文件

#include "../../Zport/Zport/Zportlib.h"
#include "../../Zport/Zport/ZportClient.h""
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>


//struct ts{
//    int a;
//    char p[30];
//    time_t aa;
//};
//void sys_err(const char *str)
//{
//    perror(str);
//    exit(1);
//}

//int main(void)
//{
//    pid_t pid;
//    time_t p;
//    time(&p);
//    struct ts* a=new ts;
//    struct ts* b=new ts;
//    a->a=1;
//    a->aa=p;
//    strcpy(a->p,"pineapple");
//    int fd[2];
//    if (pipe(fd) == -1)
//        sys_err("pipe");
//    pid = fork();
//    if (pid < 0) {
//        sys_err("fork err");
//    } else if (pid == 0) {
//        close(fd[1]);
//        int len = (int)read(fd[0], (char*)b, sizeof(ts));
//        //write(STDOUT_FILENO, buf, len);
//        cout<<b->a<<"--"<<b->p<<"--"<<ctime(&b->aa)<<endl;
//        close(fd[0]);
//    } else {
//        close(fd[0]);
//        write(fd[1], (char*)a, sizeof(ts));
//        //wait(NULL);
//        close(fd[1]);
//    }
//    delete a;
//    delete b;
//    return 0;
//}

//int TestBench_Package_B1()//应用端B，阶段一
//{
//    ZportError* _Zerror=new ZportError();
//    Zsocket_sender* _Zsock_sender=new Zsocket_sender(2,_Zerror);
//    Zsocket_receiver* _Zsock_receiver=new Zsocket_receiver(2,_Zerror);
//    Zport* _Zport=new Zport(_Zsock_receiver,_Zsock_sender,2);//Test-2
//    _Zport->Zsock_recv->Init_Zsocket_receiver();
//    _Zport->Zsock_recv->Zmessgae_receiver();
//    delete _Zerror;
//    delete _Zsock_sender;
//    delete _Zsock_receiver;
//    delete _Zport;
//    return 1;
//};

void* test_fun(void* arg)
{
    Zport* _Zport=(Zport*)arg;
    sleep(20);
    ZportPackage* pak=new ZportPackage;
    pak->package_flag=1;
    pak->val=1;
    pak->op=1;
    strcpy(pak->tar_usr,"zar");
    strcpy(pak->pro_usr,"tom");
    _Zport->Zsock->Zmessage_sender(pak,"test");//测试
    delete pak;
    pthread_exit(0);
};

int TestBench_Package_B2()//应用端B，阶段二
{
    ZportError* _Zerror=new ZportError();
    SharedMemory* _s_m=new SharedMemory(_Zerror);
    
    Zaffairhub* _Zaffair=new Zaffairhub(_Zerror,_s_m);
    Chain_Executor* _Chain_ex=new Chain_Executor(_Zerror);

    Zsocket* _Zsock=new Zsocket(1,_Zerror);
    Init_sock(_Zsock);//创建通信
    
    Zport* _Zport=new Zport(_Zsock,1,_Zaffair,_Zerror,_Chain_ex);//Test-1
    
    pthread_t pt_t;
    pthread_create(&pt_t, NULL, test_fun, (void*)_Zport);
    _Zport->Zport_Init();//初始化
    
    delete _Zerror;
    delete _s_m;
    delete _Zaffair;
    delete _Chain_ex;
    delete _Zsock;
    delete _Zport;
    return 1;
};

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

int TestBench_Package_B3()//应用端A,阶段二
{
    ZportError* _Zerror=new ZportError();//初始化报错类
    SharedMemory* _s_m=new SharedMemory(_Zerror);//初始化共享内存类
    
    Zaffairhub* _Zaffair=new Zaffairhub(_Zerror,_s_m);//初始化事务管理类
    Chain_Executor* _chain_ex=new Chain_Executor(_Zerror);//初始化链上执行类

    Zsocket* _Zsock=new Zsocket(1,_Zerror);//初始化套接字管理类
    Affair_Moniter* _Affair_moniter=new Affair_Moniter(_s_m,_Zerror,_chain_ex,fabric,_Zsock);
    Init_sock(_Zsock);//创建通信
    Zport* _Zport=new Zport(_Zsock,1,_Zaffair,_Zerror,_chain_ex);//初始化网关
    
    pthread_t pt_zport,pt_mointer,pt_test;
    pthread_create(&pt_test, NULL, test_fun, (void*)_Zport);
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
};

void * client_fun(void* arg)
{
    ZportClient* _ZportClient=(ZportClient*) arg;
    _ZportClient->Start_ZportClient();
    pthread_exit(0);//退出线程
}
//int main() {
    //    ZportError* _Zerror=new ZportError();
    //    Zsocket_receiver* _Zsock_receiver=new  Zsocket_receiver(2,_Zerror);
    //    int pid=fork();
    //    if(pid<0)
    //    {
    //        cout<<"多线程创建失败"<<endl;
    //        return -1;
    //    }
    //    else if(pid==0)
    //    {
    //        cout<<"进入父进程"<<endl;
    //        fflush(stdout);
    //        _Zsock_receiver->Zmessgae_receiver();
    //        cout<<"父进程中对象数据实例信息【"<<_Zsock_receiver->time_lock_stamp<<"】"<<endl;
    //        fflush(stdout);
    //    }
    //    else
    //    {
    //        cout<<"进入子进程"<<endl;
    //        fflush(stdout);
    //        time_t time_now;
    //        time(&time_now);
    //        while(_Zsock_receiver->time_lock_stamp!=time_now)
    //        {
    //            cout<<"锁定时间为【"<<_Zsock_receiver->time_lock_stamp<<"】目前时间为【"<<time_now<<"】"<<endl;
    //            time(&time_now);
    //        }
    //        fun();
    //        exit(1);
    //    }
    //    while (wait(0) != -1)
    //            ;
    //    delete _Zerror;
    //    delete _Zsock_receiver;
    //    return 1;
    
    
    
  //  return 1;
//}
int main()
{
    //TestBench_Package_B3();
    ZportError* _Zerror=new ZportError();//初始化报错类
    SharedMemory* _s_m=new SharedMemory(_Zerror);//初始化共享内存类
    
    Zaffairhub* _Zaffair=new Zaffairhub(_Zerror,_s_m);//初始化事务管理类
    Chain_Executor* _chain_ex=new Chain_Executor(_Zerror);//初始化链上执行类
    
    Zsocket* _Zsock=new Zsocket(1,_Zerror);//初始化套接字管理类
    Affair_Moniter* _Affair_Moniter=new Affair_Moniter(_s_m,_Zerror,_chain_ex,fabric,_Zsock);
    
    Init_sock(_Zsock);//创建通信
    Zport* _Zport=new Zport(_Zsock,1,_Zaffair,_Zerror,_chain_ex);//初始化网关
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
    return  1;
}
