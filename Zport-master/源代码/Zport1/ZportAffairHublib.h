//
//  ZportAffairHublib.h
//  Zport
//  用于实现事坞的数据结构定义与操作
//  Created by Zarror Wang on 2022/4/18.
//

#ifndef ZportAffairHublib_h
#define ZportAffairHublib_h
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
#include "ZportErrorlib.h"
#include "ZportSharedMemorylib.h"
#include "ZportSocketlib.h"
using namespace std;


string Gen_pert()//随机英文字母序列生成函数，用于生成每件事物的唯一标识，共十个字符长度
{
    string pert(16,'a');
    vector<char> type_char={'1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    sleep(3);
    srand((int)time(NULL));
    for(int i=0;i<15;i++)
    {
        int index=rand()%61;
        pert[i]=type_char[index];
    }
    pert[15]='\0';
    return pert;
};


class Zaffairhub//数据坞类
{
private:
    SharedMemory* SM;//共享内存管理类
    ZportError* Zerror;//报错类
    Zsocket* Zsock;//消息管理类
    
public:
    Zaffairhub(ZportError* _Zerror,SharedMemory* _SM):Zerror(_Zerror),SM(_SM){}
    int make_affair(string port_name,int affair_mode,string pert, int affair_flag,string contract_name,string contract_function,int op,string tar_user,int val)//后三字段为测试字段,mode字段确认该事物是自身提起or对方提起的，若是自己提起的，remote_lock在第三阶段，若是对方提起的，直接在此remote_lock
    {
        time_t time_now;//用于记录事务时间
        if(SM->s_m->affair_cnt>=100)//事务坞中已满，不可再添加
        {
            Zerror->error(9,"");
            return -1;
        }
        else//向事务坞中加入事务
        {
            time(&time_now);
            SM->s_m->affair_seg[SM->s_m->affair_cnt].affair_init_time=time_now;//事务初始化时间
            strncpy(SM->s_m->affair_seg[SM->s_m->affair_cnt].pert,pert.c_str(),16);//现在在想添加一个hash函数能够唯一的确认一笔交易。用pert
            SM->s_m->affair_seg[SM->s_m->affair_cnt].op=op;
            SM->s_m->affair_seg[SM->s_m->affair_cnt].val=val;
            SM->s_m->affair_seg[SM->s_m->affair_cnt].moniter_flag=0;//链上执行状态
            strncpy(SM->s_m->affair_seg[SM->s_m->affair_cnt].tar_user,tar_user.c_str(),16);//执行用户信息
            strncpy(SM->s_m->affair_seg[SM->s_m->affair_cnt].port_name, port_name.c_str(),16);//发起事务的网关名
            strncpy(SM->s_m->affair_seg[SM->s_m->affair_cnt].contract_function,contract_function.c_str(),16);//合约函数
            strncpy(SM->s_m->affair_seg[SM->s_m->affair_cnt].contract_name, contract_name.c_str(),16);//合约名称
            SM->s_m->affair_seg[SM->s_m->affair_cnt].affair_flag=affair_flag;
        }
        if(affair_mode==1)//0代表该事务是自己提起的，1代表对方提起
        {
            SM->s_m->affair_seg[SM->s_m->affair_cnt].remote_lock_symbol=0;//若是对方提起的，表示对方事物等待在lock_time阶段锁定
        }
        else
        {
            SM->s_m->affair_seg[SM->s_m->affair_cnt].remote_lock_symbol=1;//若是自己提起的，则当可以生成事务的时候便锁定，应为远端已经锁定了
        }
        SM->s_m->affair_cnt++;//事务数量增加
        this->Zerror->msg(8, pert, "", "", 0);//输出成功信息
        return 1;
    }
    int Lock_affair(string pert)//锁定双方的事务，保证同一件事物在双方的共享内存区都已经存在
    {
        time_t time_now;//用于记录事务时间
        int flag=0;//标志位，用于确定该步骤是否找到了事务
        for(int i=0;i<SM->s_m->affair_cnt;i++)
        {
            if(strcmp(SM->s_m->affair_seg[i].pert,pert.c_str())==0)//匹配到了该事物单
            {
                SM->s_m->affair_seg[i].remote_lock_symbol=1;//锁定对方
                flag=1;
                break;
            }
        }
        if(flag==0)//没有找到该事物，没有锁定
        {
            this->Zerror->error(11, pert);
            return -1;
        }
        else//输出锁定成功信息
        {
            time(&time_now);
            this->Zerror->msg(9, pert, "", "", 0);
        }
        return 1;
    }
    int Lock_affair_time(time_t lock_time,string pert,int mode)//锁定双方事务执行时间,按照约定时间便会执行,mode标识时间是自己提的还是对方提的，自己提的[0]等待远端时间锁定，对方提的[1]直接远端锁定
    {
        time_t time_now;//用于获得实时时间
        int flag=0;//标志位，用于确定该步骤是否找到了事务
        for(int i=0;i<SM->s_m->affair_cnt;i++)
        {
            if(strcmp(SM->s_m->affair_seg[i].pert,pert.c_str())==0)//匹配到了该事物
            {
                //锁定时间之前，先测试时间是否合法
                time(&time_now);//获取实时时间
                if( time_now >= lock_time || SM->s_m->affair_seg[i].remote_lock_symbol!=1)//当前时间已经超过了约定的时间，失效,或该事务还未锁定，不予锁定时间
                {
                    if(SM->s_m->affair_seg[i].remote_lock_symbol!=1)
                    this->Zerror->error(13, pert);
                    else
                    {
                        
                        //this->Zerror->msg(0, "时间失效", "", "", 0);
                        cout<<"当前时间"<<time_now<<"--"<<"约定时间"<<lock_time<<endl;
                        fflush(stdout);
                    }
                    return -1;
                }
                SM->s_m->affair_seg[i].affair_lock_time=lock_time;//锁定时间
                if(mode==1)
                {
                    SM->s_m->affair_seg[i].remote_time_lock_symbol=1;
                }
                flag=1;
                break;
            }
        }
        if(flag==0)//没有找到该事物，没有锁定时间
        {
            this->Zerror->error(12, pert);
            return -1;
        }
        else//输出锁定成功信息
        {
            this->Zerror->msg(10, pert, "", "", lock_time);
        }

        return 1;
    }
    int Confirm_lock_time(string pert)//锁定双方事务执行时间,按照约定时间便会执行
    {
        int flag=0;//标志位，用于确定该步骤是否找到了事务
        for(int i=0;i<SM->s_m->affair_cnt;i++)
        {
            if(strcmp(SM->s_m->affair_seg[i].pert,pert.c_str())==0)//匹配到了该事物
            {
                SM->s_m->affair_seg[i].remote_time_lock_symbol=1;//锁定时间
                flag=1;
                break;
            }
        }
        if(flag==0)//没有找到该事物
        {
            this->Zerror->error(12, pert);
            return -1;
        }
        else//输出锁定成功信息
        {
            //this->Zerror->msg(0, "目标链事务时间已锁定", "", "",0);
            this->Zerror->add_log_msg("目标链事务时间已锁定");
        }
        return 1;
    }
    
    int Comfirm_remote_affair(string pert)//确认双方事务的执行结果
    {
        time_t time_now;//用于记录事务时间
        int flag=0;//标志位，用于确定该步骤是否找到了事务
        for(int i=0;i<SM->s_m->affair_cnt;i++)
        {
            if(strcmp(SM->s_m->affair_seg[i].pert,pert.c_str())==0)//匹配到了该事物单
            {
                SM->s_m->affair_seg[i].remote_complete_symbol=1;//更新对方已经执行
                flag=1;
            }
        }
        if(flag==0)//没有找到该事物，没有更新
        {
            this->Zerror->error(14, pert);
            return -1;
        }
        else//输出更新成功信息
        {
            time(&time_now);
            this->Zerror->msg(11, pert, "", "", 0);
        }
        return 1;
    }
    
    void Dis_AffairHub(){
        for(int i=0;i<this->SM->s_m->affair_cnt;i++)
        {
            cout<<"************跨链事务***************"<<endl;
            cout<<"*****事务ID:"<<this->SM->s_m->affair_seg[i].pert<<"******"<<endl;
            cout<<"*****事务类型:"<<this->SM->s_m->affair_seg[i].affair_flag<<"*****"<<endl;
            cout<<"*****事务的初始化时间:"<<this->SM->s_m->affair_seg[i].affair_init_time<<"*****"<<endl;
            cout<<"*****事务的锁定时间:"<<this->SM->s_m->affair_seg[i].affair_lock_time<<"*****"<<endl;
            cout<<"*****事务远端锁定标记:"<<this->SM->s_m->affair_seg[i].remote_lock_symbol<<"*****"<<endl;
            cout<<"*****事务远端时间锁定标记:"<<this->SM->s_m->affair_seg[i].remote_time_lock_symbol<<"*****"<<endl;
            cout<<"*****事务本链执行标记:"<<this->SM->s_m->affair_seg[i].local_complete_symbol<<"*****"<<endl;
            cout<<"*****事务远端链执行标记:"<<this->SM->s_m->affair_seg[i].remote_complete_symbol<<"*****"<<endl;
            cout<<"*****moniter中线程执行标记:"<<this->SM->s_m->affair_seg[i].moniter_flag<<"*****"<<endl;
            cout<<"**********************************"<<endl;
        }
        fflush(stdout);
    }
    ~Zaffairhub()
    {
        Zerror->msg(0, "释放事务坞", "", "", 0);
    }
    
};

#endif /* ZportAffairHublib_h */
