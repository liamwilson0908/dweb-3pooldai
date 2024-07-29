//
//  Zportmessagelib.h
//  Zport
//  主要负责各种数据交换的数据包格式
//  Created by Zarror Wang on 2022/4/18.
//

#ifndef ZportPackagelib_h
#define ZportPackagelib_h
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
using namespace std;

struct ZportPackage//定义统一传输的消息格式
{
    time_t time_lock;//阶段二时间锁定字段
    int package_flag;//定义此数据包格式
    int affair_flag;//跨链事务类型【0】跨链数据读 【1】跨链数据写
    int val;//资产增减量
    int op;//运算符，特定对应于资产增减 0减1增
    int Refuse_Re;//若为拒绝包，此字段标识拒绝信息
    char contract_name[16];//调用的合约名
    char contract_function[16];//调用链上合约函数
    char port_name[16];//目标网关名称
    char pert[16];//阶段一事务标识
    //测试阶段字段
    char pro_usr[16];//事务提起方
    char tar_usr[16];//目标链上用户
    //解决原因字段
    char affair_msg[512];//事务执行结果字段
};

//void Gen_Package(ZportPackage* pak,int mode,int Re_flag,string port_name,string pert,time_t time_lock,int op,int val,string tar_usr,string pro_usr)//mode标识package类型,R2_flag表示拒绝的原因标识
//{
//    switch (mode) {
//        case 1://提议事务包
//            pak->package_flag=1;
//            strcpy(pak->pert, pert.c_str());//事务标识
//            strcpy(pak->port_name, port_name.c_str());//网关名
//            strcpy(pak->pro_usr,pro_usr.c_str());//提起用户名
//            strcpy(pak->tar_usr,tar_usr.c_str());//目标用户名
//            pak->val=val;//运算指令量
//            pak->op=op;//运算符
//            break;
//        case 2://拒绝提议事务包,1表示余额原因拒绝，2表示账户不存在
//            pak->package_flag=2;
//            pak->Refuse_Re=Re_flag;
//            strcpy(pak->pert, pert.c_str());//事务标识
//            strcpy(pak->port_name, port_name.c_str());//网关名
//            strcpy(pak->pro_usr,pro_usr.c_str());//提起用户名
//            strcpy(pak->tar_usr,tar_usr.c_str());//目标用户名
//            pak->val=val;//运算指令量
//            pak->op=op;//运算符
//            break;
//        case 3://生成事务包
//            pak->package_flag=3;
//            strcpy(pak->pert, pert.c_str());//事务标识
//            strcpy(pak->port_name, port_name.c_str());//网关名
//            strcpy(pak->pro_usr,pro_usr.c_str());//提起用户名
//            strcpy(pak->tar_usr,tar_usr.c_str());//目标用户名
//            pak->val=val;//运算指令量
//            pak->op=op;//运算符
//            break;
//        case 4://提议时间锁定包
//            pak->package_flag=4;
//            strcpy(pak->pert, pert.c_str());//事务标识
//            strcpy(pak->port_name, port_name.c_str());//网关名
//            pak->time_lock=time_lock;//锁定的时间
//            strcpy(pak->pro_usr,pro_usr.c_str());//提起用户名
//            strcpy(pak->tar_usr,tar_usr.c_str());//目标用户名
//            pak->val=val;//运算指令量
//            pak->op=op;//运算符
//            break;
//        case 5://拒绝时间锁定包
//            pak->package_flag=5;
//            pak->Refuse_Re=Re_flag;
//            strcpy(pak->pert, pert.c_str());//事务标识
//            pak->time_lock=time_lock;//锁定的时间
//            strcpy(pak->port_name, port_name.c_str());//网关名
//            strcpy(pak->pro_usr,pro_usr.c_str());//提起用户名
//            strcpy(pak->tar_usr,tar_usr.c_str());//目标用户名
//            pak->val=val;//运算指令量
//            pak->op=op;//运算符
//            break;
//        case 6://接收时间锁定包
//            pak->package_flag=6;
//            strcpy(pak->pert, pert.c_str());//事务标识
//            pak->time_lock=time_lock;//锁定的时间
//            strcpy(pak->port_name, port_name.c_str());//网关名
//            strcpy(pak->pro_usr,pro_usr.c_str());//提起用户名
//            strcpy(pak->tar_usr,tar_usr.c_str());//目标用户名
//            pak->val=val;//运算指令量
//            pak->op=op;//运算符
//            break;
//        case 7:
//            pak->package_flag=7;
//            strcpy(pak->pert, pert.c_str());//事务标识
//            pak->time_lock=time_lock;//锁定的时间
//            strcpy(pak->port_name, port_name.c_str());//网关名
//            strcpy(pak->pro_usr,pro_usr.c_str());//提起用户名
//            strcpy(pak->tar_usr,tar_usr.c_str());//目标用户名
//            pak->val=val;//运算指令量
//            pak->op=op;//运算符
//            break;
//        case 8:
//            pak->package_flag=8;
//            strcpy(pak->pert, pert.c_str());//事务标识
//            pak->time_lock=time_lock;//锁定的时间
//            strcpy(pak->port_name, port_name.c_str());//网关名
//            break;
//        default:
//            break;
//    }
//
//}
void Clear_Package(ZportPackage* pak)
{
    pak->package_flag=0;
    pak->affair_flag=-1;
    strncpy(pak->affair_msg,"",512);
    pak->val=0;
    pak->op=-1;
    strncpy(pak->pert,"",16);
    strncpy(pak->tar_usr,"",16);
    strncpy(pak->pro_usr,"",16);
    strncpy(pak->port_name,"",16);
    strncpy(pak->contract_name,"",16);
    strncpy(pak->contract_function, "",16);
    pak->time_lock=0;
    pak->Refuse_Re=0;
}

void Copy_Package(ZportPackage* pak1,ZportPackage* pak2)
{
    pak1->val=pak2->val;
    pak1->op=pak2->op;
    pak1->package_flag=pak2->package_flag;
    pak1->time_lock=pak2->time_lock;
    pak1->affair_flag=pak2->affair_flag;
    strncpy(pak1->contract_name,pak2->contract_name,16);
    strncpy(pak1->contract_function,pak2->contract_function,16);
    strncpy(pak1->affair_msg,pak2->affair_msg,512);
    strncpy(pak1->pert,pak2->pert,16);
    strncpy(pak1->tar_usr,pak2->tar_usr,16);
    strncpy(pak1->pro_usr,pak2->pro_usr,16);
    strncpy(pak1->port_name,pak2->port_name,16);
    pak1->Refuse_Re=pak2->Refuse_Re;
}




#endif /* ZportPackagelib_h */
