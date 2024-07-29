//
//  ZportSharedMemorylib.h
//  Zport
//  用于统一的内存管理
//  Created by Zarror Wang on 2022/4/19.
//


#ifndef ZportSharedMemorylib_h
#define ZportSharedMemorylib_h
#include "ZportErrorlib.h"

typedef struct AFFAIR//一条事务的数据结构，目前只针对链上数据交换场景
{
    time_t affair_init_time;//记录事务的初始化时间
    time_t affair_lock_time;//记录事务的锁定执行时间
    int affair_flag;//记录事务类型  0->数据查询。1->数据更新,2->不需要执行，仅对方链上执行
    int remote_lock_symbol;//远端事务锁定标记
    int remote_time_lock_symbol;//远端事务时间锁定标记
    int local_complete_symbol;//本链已经执行标记
    int remote_complete_symbol;//远端链执行标记
    int op;//1代表增加，2代表减少
    int val;
    int moniter_flag;//记录monitor中是否以及建立线程执行 0->未创建执行线程。1->已创建执行线程 2->已创建线程，执行完毕，并回执
    char port_name[16];//发起事务的网关名称
    char contract_name[16];//合约名层
    char contract_function[16];//合约函数
    char tar_user[16];
    char pert[16];//事务的唯一标识，随机生成
    char affair_msg[512];//事务执行结果信息
}affair;

struct Shared_Memory//定义共享内存区的数据结构
{
    int affair_cnt;//当前事务坞内事务数量
    affair affair_seg[100];
};

class SharedMemory//用于内存管理的类
{
public:
    Shared_Memory* s_m;//用于访问共享内存的指针
private:
    int shm_id;//共享内存ID
    ZportError* Zerror;//报错类
public:
    int Init_Shared_Memory()//创建共享内存，并把this->af指向共享区的首地址
    {
        this->shm_id = shmget(IPC_PRIVATE, sizeof(Shared_Memory), 0600);//事务坞允许存储100件近期事务
        if(this->shm_id==-1)//创建共享内存失败
        {
            Zerror->error(8,"");
            return -1;
        }
        this->s_m=(Shared_Memory*)shmat(this->shm_id, 0, 0);//打开共享内存区
        this->s_m->affair_cnt=0;
        return 1;
    }
    int Free_Shared_Memory()//释放共享内存区
    {
        Zerror->msg(0, "释放共享内存", "", "", 0);
        shmdt(this->s_m);
        shmctl(this->shm_id, IPC_RMID, 0); //当cmd为IPC_RMID时，删除该共享段
        return 1;
    }
    int Get_Shared_Memory_ID()//返回共享内存ID
    {
        return this->shm_id;
    }
    SharedMemory(ZportError* _Zerror):Zerror(_Zerror)
    {
        Init_Shared_Memory();//初始化共享内存
    }
    ~SharedMemory()
    {
        for(int i=0;i<this->s_m->affair_cnt;i++)
        {
            string tmp0="*************************************************";
            string tmp1="事务初始化时间:";
            tmp1+=to_string(this->s_m->affair_seg[i].affair_init_time);
            string tmp2="事务的锁定执行时间:";
            tmp2+=to_string(this->s_m->affair_seg[i].affair_lock_time);
            string tmp3="事务类型:";
            tmp3+=to_string(this->s_m->affair_seg[i].affair_flag);
            string tmp4="远端事务锁定标记:";
            tmp4+=to_string(this->s_m->affair_seg[i].remote_lock_symbol);
            string tmp5="远端事务时间锁定标记:";
            tmp5+=to_string(this->s_m->affair_seg[i].remote_time_lock_symbol);
            string tmp6="本链已经执行标记:";
            tmp6+=to_string(this->s_m->affair_seg[i].local_complete_symbol);
            string tmp7="远端链执行标记:";
            tmp7+=to_string(this->s_m->affair_seg[i].remote_complete_symbol);
            string tmp8="操作符:";
            tmp8+=to_string(this->s_m->affair_seg[i].op);
            string tmp9="操作量:";
            tmp9+=to_string(this->s_m->affair_seg[i].val);
            string tmp10="mointer监视标记:";
            tmp10+=to_string(this->s_m->affair_seg[i].moniter_flag);
            string tmp11="发起事务的网关名称:";
            tmp11+=this->s_m->affair_seg[i].port_name;
            string tmp12="合约名层:";
            tmp12+=this->s_m->affair_seg[i].contract_name;
            string tmp13="合约函数:";
            tmp13+=this->s_m->affair_seg[i].contract_function;
            string tmp14="目标账户:";
            tmp14+=this->s_m->affair_seg[i].tar_user;
            string tmp15="事务标识:";
            tmp15+=this->s_m->affair_seg[i].pert;
            string tmp16="回执信息:";
            tmp16+=this->s_m->affair_seg[i].affair_msg;
            Zerror->add_affair_file(tmp0);
            Zerror->add_affair_file(tmp15);
            Zerror->add_affair_file(tmp3);
            Zerror->add_affair_file(tmp1);
            Zerror->add_affair_file(tmp2);
            Zerror->add_affair_file(tmp11);
            Zerror->add_affair_file(tmp12);
            Zerror->add_affair_file(tmp13);
            Zerror->add_affair_file(tmp14);
            Zerror->add_affair_file(tmp4);
            Zerror->add_affair_file(tmp5);
            Zerror->add_affair_file(tmp6);
            Zerror->add_affair_file(tmp7);
            Zerror->add_affair_file(tmp8);
            Zerror->add_affair_file(tmp9);
            Zerror->add_affair_file(tmp10);
            Zerror->add_affair_file(tmp16);
            Zerror->add_affair_file(tmp0);
        }
        Free_Shared_Memory();
    }
    
    
};




#endif /* ZportSharedMemorylib_h */
