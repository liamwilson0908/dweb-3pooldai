//
//  Zportlib.h
//  Zport
//  负责实现基础的通信类以及业务管理类
//  Created by Zarror Wang on 2022/4/16.
//

#ifndef Zportlib_h
#define Zportlib_h
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
#include <pthread.h>
#include <queue>
#include "ZportPackagelib.h"
#include "ZportErrorlib.h"
#include "ZportSocketlib.h"
#include "ZportSharedMemorylib.h"
#include "ZportAffairHublib.h"
#include "ZportSocketlib.h"
#include "ZportChainKitlib.h"

void * Zport_Package_Analyse(void *arg);

class Zport //处理跨链事务顶层类
{
public:
    //    Zsocket_receiver* Zsock_recv;//消息发送类
    //    Zsocket_sender* Zsock_send;//消息接收类
    Zaffairhub* Zaffair;//管理事务的类
    ZportError* Zerror;//报错类
    Chain_Executor* chain_ex;//链交互类
    string port_name;//本地链（网关）名称
    Zsocket* Zsock;//消息管理类
    queue<ZportPackage*> pak_hub;//消息记录站点
    int process_control;//进程控制退出标志
    vector<string> register_chain;//已经注册的链名称
    map<string,string> register_chain_type;//已注册链的类型
    map<string,vector<string>> chain_contract;//链上合约记录信息
    map<string,vector<string>> contract_function;//合约函数信息
    map<string,int> function_type;//记录链上合约操作类型（读数据或是写数据）
    
public:
    Zport(Zsocket* _Zsock,int test_mode,Zaffairhub* _Zaffair,ZportError* _Zerror,Chain_Executor* _chain_ex):Zsock(_Zsock),Zaffair(_Zaffair),Zerror(_Zerror),chain_ex(_chain_ex)//初始化构造函数
    {
        register_chain.push_back("test");
        register_chain.push_back("this");
        chain_contract["test"].push_back("transfer");
        chain_contract["this"].push_back("transfer");
        contract_function["transfer"].push_back("getBalance");
        contract_function["transfer"].push_back("setBalance");
        contract_function["transfer"].push_back("transfer");
        function_type["getbalance"]=0;
        function_type["setbalance"]=1;
        function_type["transfer"]=1;
        this->process_control=0;
        if(test_mode==1)
        {
            port_name="fabric";
            register_chain_type["test"]="ether";
        }
        else
        {
            port_name="ether";
            register_chain_type["test"]="fabric";
        }
    };
    ~Zport()
    {
        Zerror->msg(0, "释放链执行类", "", "", 0);
    }
    static void* Zport_Server(void* arg);//启动业务服务流程类,服务于外来的数据包
    int Zport_Init(void);//初始化各个进程，创建管道以完成进程通信
    void friend::Copy_Package(ZportPackage *pak1, ZportPackage *pak2);
};


struct pthread_data//用于解包的数据结构
{
    Zport* _Zport;//在后续用于发送数据
    ZportPackage _pak;
};


void* Zport::Zport_Server(void* arg)//启动业务服务流程类,服务于外来的数据包
{
    Zport* _Zport=(Zport*)arg;
    while(1)
    {
        
        if(_Zport->pak_hub.empty())
            continue;
        else
        {
            ZportPackage* pak_tmp=_Zport->pak_hub.front();
            //包处理
            pthread_t pt;
            struct pthread_data* pt_data=new pthread_data;
            Copy_Package(&(pt_data->_pak), pak_tmp);//🌟先添加了友元函数，之后转换地址
            pt_data->_Zport=_Zport;
            pthread_create(&pt, NULL, Zport_Package_Analyse, (void*)pt_data);
            _Zport->pak_hub.pop();
            delete (ZportPackage*)pak_tmp;//释放
        }
        if(_Zport->process_control==1)//设置退出逻辑
        {
            break;
        }
    }
    pthread_exit(0);//退出线程
}


void* Zmessgae_receiver(void * arg) //监听函数功能分发
{
    Zport* _Zport=(Zport*)arg;
    while (1)
    {
        ZportPackage* pak=new ZportPackage;
        pak->package_flag=-1;
        read(_Zport->Zsock->socket_receiver_id, (char*)pak, sizeof(ZportPackage));
        if(pak->package_flag!=-1)
        {
            //_Zport->Zsock->_Zerror->package_info(1, pak);
            _Zport->pak_hub.push(pak);
        }
        if(_Zport->process_control==1)
        {
            break;
        }
    }
    close(_Zport->Zsock->socket_receiver_id);
    pthread_exit(0);
};

int Zport::Zport_Init(void)//初始化各个进程，创建管道以完成进程通信
{
    pthread_t pt_server,pt_recv;//主线程包处理，线程包接受
    pthread_create(&pt_recv, NULL, Zmessgae_receiver, (void*)this);
    pthread_create(&pt_server, NULL, this->Zport_Server, (void*)this);
    pthread_join(pt_recv, NULL);
    pthread_join(pt_server, NULL);
    cout<<"Zport_Init退出了！！！！！"<<endl;
    fflush(stdout);
    exit(0);//退出进程
}


void * Zport_Package_Analyse(void *arg)//对数据包进行分析，以决定下一步行动
{
    Zport* _Zport= ((struct pthread_data*)arg)->_Zport;
    ZportPackage* pak=&((pthread_data*)arg)->_pak;//🌟先添加了友元函数，之后转换地址
    _Zport->Zerror->add_log_msg("创建子线程-->"+to_string(pak->package_flag));
    switch (pak->package_flag)
    {
        case 1://【提议跨链事务】数据包,需要查询链上数据，确定是否设置生成跨链事务
            if(_Zport->chain_ex->Check_data()==1)//确认可以执行跨链事务，跨链事务合法性检查
            {
                //生成事务 makea_affair
                ZportPackage* pak_tmp=new ZportPackage;
                pak_tmp->package_flag=3;//同意对方在共享内存中生成事务
                string affair_pert_tmp=Gen_pert();//对同意的事务生成证书
                strncpy(pak_tmp->pert,affair_pert_tmp.c_str(),16);
                strncpy(pak_tmp->port_name, pak->port_name,16);//记录网关名称
                if(pak->affair_flag==0)//对方提起跨链数据读，则对方链事务不需要进行操作
                {
                    strncpy(pak_tmp->contract_function, "NNNNNNNnnnnnnNNN",16);
                    strncpy(pak_tmp->contract_name, "NNNNNNnnnnnnNNNN",16);
                    pak_tmp->affair_flag=2;
                }
                else
                {
                    pak_tmp->affair_flag=1;
                    strncpy(pak_tmp->contract_function, "transout",16);
                    strncpy(pak_tmp->contract_name,"transfer",16);
                    strncpy(pak_tmp->pro_usr,pak->pro_usr,16);
                    strncpy(pak_tmp->tar_usr,pak->tar_usr,16);
                    pak_tmp->val=pak->val;
                    pak_tmp->op=0;//减少资产
                    pak->op=1;
                }
                _Zport->Zsock->Zmessage_sender(pak_tmp,pak_tmp->port_name);//发出同意消息
                delete pak_tmp;
                _Zport->Zaffair->make_affair(pak->port_name,1, affair_pert_tmp.c_str(), pak->affair_flag, pak->contract_name,pak->contract_function, pak->op,pak->tar_usr,pak->val);//[1]代表对方提起，pak->affair_flag代表跨链事务类型(1.数据查询，2.数据更新)
                _Zport->Zaffair->Lock_affair(affair_pert_tmp);
            }
            else
            {
                //返回失败回执
                ZportPackage* pak_tmp=new ZportPackage;
                strncpy(pak_tmp->pert,pak->pert,16);
                strncpy(pak_tmp->port_name, pak->port_name,16);//记录网关名称
                pak_tmp->package_flag=2;//拒绝对方在共享内存中生成事务
                _Zport->Zsock->Zmessage_sender(pak_tmp,pak_tmp->port_name);//发出拒绝消息
                delete pak_tmp;
            }
            break;
        case 2://收到【拒绝事务提议】数据包，需要向用户返回拒绝原因
            //返回拒绝原因
            _Zport->Zerror->error(-1, "远端拒绝锁定该事物");
            break;
        case 3://收到【生成事务数据包】，需要在本地共享内存中锁定事务，并提议锁定时间
        {
            _Zport->Zaffair->make_affair(pak->port_name,0, pak->pert, pak->affair_flag, pak->contract_name,pak->contract_function,pak->op,pak->pro_usr,pak->val);//[0]代表对方提起，affair_flag代表跨链事务类型
            _Zport->Zaffair->Lock_affair(pak->pert);//锁定事务
            //接下来约定锁定时间
            time_t time_now=0;
            ZportPackage* pak_tmp=new ZportPackage;
            strncpy(pak_tmp->port_name, pak->port_name,16);//记录网关名称
            strncpy(pak_tmp->pert,pak->pert,16);
            pak_tmp->package_flag=4;//提议锁定时间数据包
            time(&time_now);//获取现在时间
            pak_tmp->time_lock=time_now+10;//约定为10s之后
            _Zport->Zsock->Zmessage_sender(pak_tmp,pak_tmp->port_name);//发出锁定时间消息
            _Zport->Zaffair->Lock_affair_time(pak_tmp->time_lock, pak->pert,0);//锁定本地时间
            delete pak_tmp;
            break;
        }
        case 4://收到【提议锁定时间】数据包，检查时间，进行锁定，回执锁定
            if(_Zport->chain_ex->Check_time(pak->time_lock)==1)//时间合法，锁定时间，发回时间锁定成功回执
            {
                ZportPackage* pak_tmp=new ZportPackage;
                pak_tmp->package_flag=6;//接收锁定的时间
                strncpy(pak_tmp->pert,pak->pert,16);
                strncpy(pak_tmp->port_name, pak->port_name,16);//记录网关名称
                _Zport->Zsock->Zmessage_sender(pak_tmp,pak_tmp->port_name);//发出接收消息
                delete pak_tmp;
                _Zport->Zaffair->Lock_affair_time(pak->time_lock, pak->pert,1);//锁定本地时间
            }
            else//时间非法，拒绝锁定时间
            {
                ZportPackage* pak_tmp=new ZportPackage;
                pak_tmp->package_flag=5;//接收锁定的时间
                strncpy(pak_tmp->pert,pak->pert,16);
                strncpy(pak_tmp->port_name, pak->port_name,16);//记录网关名称
                _Zport->Zsock->Zmessage_sender(pak_tmp,pak_tmp->port_name);//发出拒绝消息
                delete pak_tmp;
            }
            break;
        case 5://对方拒绝提议的锁定时间
            //暂不处理
            break;
        case 6://对方接受提议的锁定时间
            //_Zport->Zerror->msg(0, "成功商议并锁定事务", "", "", 0);
            _Zport->Zaffair->Confirm_lock_time(pak->pert);
            break;
        case 7://收到远端事务完成数据包,并解析执行结果
            //记录到事务内存中
            _Zport->Zaffair->Comfirm_remote_affair(pak->pert);
            if(strcmp(pak->affair_msg,"")!=0)
                _Zport->Zerror->affair_recall_msg(pak->affair_msg);
            //_Zport->Zerror->msg(0, pak->affair_msg, "", "", 0);
            break;
        case 8:
            //链接结束数据包，暂不处理
            break;
        default:
            break;
    }
    delete (struct pthread_data*)arg;//清除线程结构体
    pthread_exit(0);
}




#endif /* Zportlib_h */
