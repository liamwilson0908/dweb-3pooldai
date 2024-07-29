//
//  ZportErrorlib.h
//  Zport
//
//  Created by Zarror Wang on 2022/4/18.
//

#ifndef ZportErrorlib_h
#define ZportErrorlib_h
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <unistd.h>
#include <time.h>
#include <string>
#include <string.h>
#include "ZportLoglib.h"
#include "ZportPackagelib.h"
using namespace std;

class ZportError:public ZportLog //处理系统错误汇报&日志文件
{
public:
    int error_index;
    vector<string> error_msg;//错误消息记录
    vector<string> config_msg;//配置信息记录
    vector<string> log_msg;//日志文件记录
    time_t time_now;//用于记录日志输出时间
    vector<string> package_msg;//用于解释一段数据包，输出详细信息到控制台
    
public:
    ZportError()
    {
        error_msg.push_back("error");
        error_msg.push_back("套接字<socket_sender>创建失败");//index=1
        error_msg.push_back("套接字<socket_sender>连接失败");//index=2
        error_msg.push_back("套接字<socket_receiver>创建失败");//index=3
        error_msg.push_back("绑定<socket_receiver>监听地址失败");//index=4
        error_msg.push_back("监听<socket_receiver>失败");//index=5
        error_msg.push_back("套接字<socket_receiver_service>accept失败");//index=6
        error_msg.push_back("线程<zsocket_sender>发送失败");//index=7
        error_msg.push_back("ZportAffairHub申请共享内存失败");//index=8
        error_msg.push_back("ZportAffairHub事务数量已满");//index=9,mode=9
        error_msg.push_back("进程通信管道<pipe>创建失败");//index=10,mode=10
        error_msg.push_back("ZportAffairHub锁定远端事务失败，事务标识为【");//index=11,mode=11
        error_msg.push_back("】原因【未找到该事物】");//index=12,mode=11
        error_msg.push_back("ZportAffairHub锁定事务时间失败，事务标识为【");//index=13,mode=12
        error_msg.push_back("】原因【未找到该事务】");//index=14,mode=12
        error_msg.push_back("ZportAffairHub锁定事务时间失败，事务标识为【");//index=15,mode=13
        error_msg.push_back("】原因【锁定时间已经失效或该事务未锁定】");//index=16,mode=13
        error_msg.push_back("ZportAffairHub更新远端事务finished失败，事务标识为【");//index=17,mode=14
        error_msg.push_back("】原因【未找到该事物】");//index=18,mode=14
        error_msg.push_back("子进程创建失败");//index=19,mode=15
        // error_msg.push_back("");
        config_msg.push_back("config_msg");
        config_msg.push_back("套接字<socket_sender>连接端口=======>");//mode==1,index=1
        config_msg.push_back("套接字<socket_receiver>监听端口=====>");//mode==2.index=2
        config_msg.push_back("套接字<socket_sender>收到终止命令，发送结束");//mode==3,index=3
        config_msg.push_back("套接字<socket_receiver>发送方结束发送，关闭连接");//mode==4,index=4
        //config_msg.push_back("");
        log_msg.push_back("log_msg");
        log_msg.push_back("套接字<socket_sender>向地址【");//mode==5,index=1
        log_msg.push_back("】发出信息【");//mode==5,index=2
        log_msg.push_back("】");//mode==5,index=3
        log_msg.push_back("套接字<socket_receiver>从端口【");//mode==6,index=4
        log_msg.push_back("】接收到普通信息【");//mode==6,index=5
        log_msg.push_back("】");//mode==6,index=6
        log_msg.push_back("套接字<socket_receiver>从端口【");//mode==7,index=7
        log_msg.push_back("】接收到时间锁定，锁定时间为【");//mode==7,index=8
        log_msg.push_back("】");//mode==7,index=9
        log_msg.push_back("远端事务阶段一提议事务成功，事务标识为【");//mode==8,index=10
        log_msg.push_back("】");//mode==8,index=11
        log_msg.push_back("远端事务阶段二锁定事务成功，事务标识为【");//mode==9,index=12
        log_msg.push_back("】");//mode==9,index=13
        log_msg.push_back("远端事务阶段三锁定时间成功，事务标识为【");//mode==10,index=14
        log_msg.push_back("】,锁定目标时间为【");//mode==10,index=15
        log_msg.push_back("】");//mode==10,index=16
        log_msg.push_back("远端事务阶段四更新远端完成状态成功，事务标识为【");//mode==11,index=17
        log_msg.push_back("】");//mode==11,index=18
        //log_msg.push_back("");
        package_msg.push_back("package_msg");
        package_msg.push_back("【跨链事务标识为：");//index=1
        package_msg.push_back("】");//index=2
        package_msg.push_back("【Propose_Affair_Pak】此数据包为【提议跨链事务】数据包，属于【阶段一：提议】");//index=3
        package_msg.push_back("【Refuse_Propose_Affair_Pak】此数据包为【拒绝跨链事务提议】数据包，属于【阶段一：拒绝提议】");//index=4
        package_msg.push_back("【Generate_Affair_Pak】此数据包为【生成双方跨链事务】数据包，属于【阶段二：锁定事务】");//index=5
        package_msg.push_back("【Propose_Lock_Time_Pak】此数据包为【提议跨链事务的锁定时间】数据包，属于【阶段三：锁定时间】");//index=6
        package_msg.push_back("【Refuse_Lock_Time_Pak】此数据包为【拒绝跨链事务的锁定时间】数据包，属于【阶段三：拒绝锁定的时间】");//index=7
        package_msg.push_back("【Accept_Lock_Time_Pak】此数据包为【接收跨链事务的锁定时间】数据包，属于【阶段三：接收锁定的时间】");//index=8
        package_msg.push_back("【Affair_Complete_Pak】此数据包为【跨链事务执行完成回执】数据包，属于【阶段四：执行后交互】");//index=9
        package_msg.push_back("【Close_Sock_Pak】此数据包为【关闭套接字】数据包，属于【阶段五：关闭阶段】");//index=10
        //package_msg.push_back("");
        
    }
    void error(int mode,string messgae)//错误提示
    {
        time(&this->time_now);
        string tmp_msg="【";
        tmp_msg+=ctime(&this->time_now);
        tmp_msg.erase(remove(tmp_msg.begin(), tmp_msg.end(), '\n'),tmp_msg.end());
        tmp_msg+=+"】";
        switch (mode) {
            case 11://锁定事务失败
                tmp_msg+=this->error_msg[11]+messgae+this->error_msg[12];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 12://锁定时间失败，未找到事务
                tmp_msg+=this->error_msg[13]+messgae+this->error_msg[14];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 13://锁定时间失败，时间失效
                tmp_msg+=this->error_msg[15]+messgae+this->error_msg[16];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 14://更新远端事务执行状态失败，未找到该事务
                tmp_msg+=this->error_msg[17]+messgae+this->error_msg[18];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 15://子进程创建失败
                cout << this->error_msg[15] << endl;
                fflush(stdout);
                this->add_log_msg(this->error_msg[15]);
                break;
            default:
                cout << this->error_msg[mode] << endl;
                fflush(stdout);
                this->add_log_msg(this->error_msg[mode]);
                break;
        }
    }
    void msg(int mode,string message,string config_ip,string config_port,time_t time_stamp)//配置信息提示,消息类型，消息信息，IP信息，端口信息，时间戳
    {
        time(&this->time_now);
        string tmp_msg="【";
        tmp_msg+=ctime(&this->time_now);
        tmp_msg.erase(remove(tmp_msg.begin(), tmp_msg.end(), '\n'),tmp_msg.end());
        tmp_msg+=+"】";
        switch (mode) {
            case 0://直接打印message，仅用于测试
                tmp_msg+=message;
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                break;
            case 1://打印连接端口配置信息
                tmp_msg+=this->config_msg[1]+message;
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 2://打印连接端口配置信息
                tmp_msg+=this->config_msg[2]+message;
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 3://打印发送端关闭连接信息
                cout<<this->config_msg[3]<<endl;
                fflush(stdout);
                this->add_log_msg(this->config_msg[3]);
                break;
            case 4://打印接收端关闭连接信息
                cout<<this->config_msg[4]<<endl;
                fflush(stdout);
                this->add_log_msg(this->config_msg[4]);
                break;
            case 5://打印发送端发出信息内容
                tmp_msg+=log_msg[1]+config_ip+":"+config_port+log_msg[2]+message+log_msg[3];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 6://打印接收端接收到信息内容
                tmp_msg+=log_msg[4]+config_port+log_msg[5]+message+log_msg[6];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 7://测试，时间锁定
                tmp_msg+=log_msg[7]+config_port+log_msg[8]+to_string(time_stamp)+log_msg[9];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 8://远端事务锁定
                tmp_msg+=log_msg[10]+message+log_msg[11];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 9://远端事务锁定
                tmp_msg+=log_msg[12]+message+log_msg[13];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 10://远端事务时间锁定
                tmp_msg+=log_msg[14]+message+log_msg[15]+to_string(time_stamp)+log_msg[16];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 11://远端事务完成更新
                tmp_msg+=log_msg[17]+message+log_msg[18];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            default:
                break;
        }
    }
    void package_info(int mode,ZportPackage* zmsg)//输出一个包内容,mode标识接受还是发送
    {
        time(&this->time_now);
        string tmp_msg="";
        if(mode==1)
        {
            tmp_msg+="【接收到】【";
        }
        else
        {
            tmp_msg+="【发送出】【";
        }
        tmp_msg+=ctime(&this->time_now);
        tmp_msg.erase(remove(tmp_msg.begin(), tmp_msg.end(), '\n'),tmp_msg.end());
        tmp_msg+=+"】";
        switch (zmsg->package_flag) {
            case 1://提议事务包
                tmp_msg+=package_msg[3];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 2:
                tmp_msg+=package_msg[4];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 3:
                tmp_msg+=package_msg[5]+package_msg[1]+zmsg->pert+package_msg[2];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 4:
                tmp_msg+=package_msg[6]+package_msg[1]+zmsg->pert+package_msg[2];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 5:
                tmp_msg+=package_msg[7]+package_msg[1]+zmsg->pert+package_msg[2];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 6:
                tmp_msg+=package_msg[8]+package_msg[1]+zmsg->pert+package_msg[2];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 7:
                tmp_msg+=package_msg[9]+package_msg[1]+zmsg->pert+package_msg[2];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            case 8:
                tmp_msg+=package_msg[9];
                cout<<tmp_msg<<endl;
                fflush(stdout);
                this->add_log_msg(tmp_msg);
                tmp_msg.clear();
                break;
            default:
                break;
        }
        
    }
    void usr_client_info(string chain_t,string peer_chain_type)//打印用户控制台信息
    {
        cout<<"*********************************Zport区块链跨链网关系统****************************"<<endl;
        cout<<"本地网关的业务链类型"<<chain_t<<"                                                         ***"<<endl;
        cout<<"已注册友链类型：1."<<peer_chain_type<<"                                                       ***"<<endl;
        cout<<"已注册友链名称：1.test                                                           ***"<<endl;
        cout<<"已注册友链链上合约：transfer                                                      ***"<<endl;
        cout<<"本地链上合约：transfer                                                           ***"<<endl;
        cout<<"*********************************************************************************"<<endl;
        cout<<"网关合约调用方法                                                                 ***"<<endl;
        cout<<"1.本地链：Zport1 【调用链上合约名】【调用链上合约方法】【调用链上合约方法参数】             ***"<<endl;
        cout<<"2.注册友链：Zport2 【友链名称】【调用链上合约名】【调用链上合约方法】 【调用链上合约方法参数】 ***"<<endl;
        cout<<"*********************************************************************************"<<endl;
        fflush(stdout);
    }
    void affair_recall_msg(string msg)
    {
        cout<<"***********************跨链事务执行成功，执行结果如下***********************"<<endl;
        cout<<msg<<endl;
        fflush(stdout);
        cout<<"***********************************************************************"<<endl;
    }
    void local_affair_msg(string msg)
    {
        cout<<"***********************本地链事务执行成功，执行结果如下***********************"<<endl;
        cout<<msg<<endl;
        fflush(stdout);
        cout<<"***********************************************************************"<<endl;
    }
    ~ZportError()
    {
        time(&this->time_now);
        string tmp_msg="【";
        tmp_msg+=ctime(&this->time_now);
        tmp_msg.erase(remove(tmp_msg.begin(), tmp_msg.end(), '\n'),tmp_msg.end());
        tmp_msg+=+"】";
        cout<<tmp_msg<<"释放错误与日志管理类"<<endl;
        fflush(stdout);
    }

};


#endif /* ZportErrorlib_h */
