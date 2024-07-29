//
//  ZportChainKitlib.h
//  Zport
//  用于封装一些直接和联盟链进行交互的操作类
//  Created by Zarror Wang on 2022/4/18.
//

#ifndef ZportChainKitlib_h
#define ZportChainKitlib_h
#include "ZportSharedMemorylib.h"
#include "ZportErrorlib.h"
#include "ZportSocketlib.h"

/*example
 goduck fabric contract invoke transfer getBalance tom
 goduck ether contract invoke --abi-path transfer.abi 0x857133c5C69e6Ce66F7AD46F200B9B3573e77582 getBalance zar
 */
enum chain_type{fabric,ether};//链类型
struct chain_ex_arg{ //线程的传入参数
    chain_type c_t;//链类型
    int affair_flag;//事务类型
    char tar_usr[16];//目标用户
    int op;//操作符
    int val;//操作量
    SharedMemory* SM;//回执执行信息
    char pert[16];//事务标识
    int affair_index;//事务序号
    char contract_name[16];//合约名称
    char contract_function[16];//合约函数
};

class Chain_Executor//直接同链进行交互的类
{
public:
    map<string,string> account_file_path;//证书文件路径，键为链类型，值为账户证书路径
    static map<string,string> contract_address;//链上合约地址
    static map<string,string> contract_name;//链上合约名称
    static map<int,string> cmdline_config;//链上操作执行字段
    ZportError* _Zerror;//报错类
public:
    
    int Check_data()//检查目标链是否可以合法执行事务
    {
        return 1;
    }
    int Check_time(time_t time_lock)//检查提议锁定的时间是否合法
    {
        return 1;
    }
    static string Gen_Cmd(chain_type c_t,int affair_flag,string contract_name,string contract_function,string tar_usr,int op,int val)//mode:操作类型，tar_usr目标用户，op,val:增减，资产量//生成链上指令模块
    {
        string cmd_line="";
        if(c_t==fabric)
            switch (affair_flag) {
                case 0://链上数据查询
                    cmd_line+="source /etc/profile;goduck fabric contract invoke transferCC "+contract_function+" "+tar_usr;
                    break;
                case 1://链上数据操作
                    //cmd_line+=cmdline_config[1]+cmdline_config[2]+contract_name+" getBalance "+tar_usr;
                    if(op==0)//资产减事件
                    {
                        cmd_line+="source /etc/profile;goduck fabric contract invoke transferCC transout "+tar_usr+","+to_string(val);
                    }
                    else
                    {
                        cmd_line+="source /etc/profile;goduck fabric contract invoke transferCC transin "+tar_usr+","+to_string(val);
                    }
                    break;
                default:
                    break;
            }
        else if(c_t==ether)
            switch (affair_flag) {
                case 0://链上数据查询
                    cmd_line+="source /etc/profile;cd ~/go/src/pier-client-ethereum/example;goduck ether contract invoke --abi-path transfer.abi 0x857133c5C69e6Ce66F7AD46F200B9B3573e77582 "+contract_function+" "+tar_usr;
                    break;
                case 1://链上数据操作
                    if(op==0)//资产减事件
                    {
                        cmd_line+="source /etc/profile;cd ~/go/src/pier-client-ethereum/example;goduck ether contract invoke --abi-path transfer.abi 0x857133c5C69e6Ce66F7AD46F200B9B3573e77582 transout "+tar_usr+","+to_string(val);
                    }
                    else
                    {
                        cmd_line+="source /etc/profile;cd ~/go/src/pier-client-ethereum/example;goduck ether contract invoke --abi-path transfer.abi 0x857133c5C69e6Ce66F7AD46F200B9B3573e77582 transin "+tar_usr+","+to_string(val);
                    }
                    break;
                default:
                    break;
            }
        return cmd_line;
        
    }
    
    static void* Chain_Ex(void* arg)//mode:操作类型，tar_usr目标用户，op,val:增减，资产量
    {
        chain_ex_arg* ex_arg=(chain_ex_arg*)arg;
        chain_type c_t=ex_arg->c_t;
        int affair_flag=ex_arg->affair_flag;
        string tar_usr=ex_arg->tar_usr;
        int op=ex_arg->op;
        int val=ex_arg->val;
        SharedMemory* SM=ex_arg->SM;
        int affair_index=ex_arg->affair_index;
        string contract_name=ex_arg->contract_name;
        string contract_function=ex_arg->contract_function;
        string chain_cmd="";
        //        cout<<"合约名"<<contract_name<<endl;
        //        cout<<"函数名"<<contract_function<<endl;
        //        cout<<"参数"<<tar_usr<<endl;
        //        fflush(stdout);
        //        string chain_cmd="";
        //        chain_cmd=Gen_Cmd(c_t,mode, "transfer",tar_usr, op, val);
        //        //FILE* fp=popen("source /etc/profile;goduck fabric contract invoke transfer getBalance tom", "r");
        //        //FILE* fp1=popen("source /etc/profile;cd ~/go/src/pier-client-ethereum/example;goduck ether contract invoke --abi-path transfer.abi 0x857133c5C69e6Ce66F7AD46F200B9B3573e77582 getBalance zar","r");
        //        FILE* fp=popen(chain_cmd.c_str(),"r");
        //        char* buf=(char*)malloc(2048*sizeof(char));
        //        int buf_size=(int)fread(buf, 1, 2048, fp);
        //        if(buf_size!=-1)
        //            cout<<"这是管道读出的反值"<<endl<<buf<<endl;
        //        fclose(fp);
        chain_cmd = Gen_Cmd(c_t, affair_flag, contract_name,contract_function,tar_usr, op, val);
        FILE* fp=popen(chain_cmd.c_str(),"r");
        char* buf=(char*)malloc(2048*sizeof(char));
        int buf_size=(int)fread(buf, 1, 2048, fp);
        pclose(fp);
  
        if(buf_size!=-1)//根据执行结果，返回回执
        {
            SM->s_m->affair_seg[affair_index].local_complete_symbol=1;
            //            if(c_t==fabric)
            //                strcpy(SM->s_m->affair_seg[affair_index].affair_msg,buf);
            //            else
            //                strcpy(SM->s_m->affair_seg[affair_index].affair_msg,buf);
            strncpy(SM->s_m->affair_seg[affair_index].affair_msg,buf,512);
        }
        free (buf);
        pthread_exit(0);
    }
    Chain_Executor(ZportError* _zerror):_Zerror(_zerror)
    {
        
    }
    ~Chain_Executor()
    {
        _Zerror->msg(0, "释放链执行类", "", "", 0);
    }
    
};

struct Affair_Res_arg//用于事务处理完成后返回执行结果的回执线程函数传数结构体
{
    Zsocket* Zsock;//用于信息发送的结构体
    char pert[16];//事务标识
    char target_port_name[16];//目标网关名称
    char affair_msg[512];//链上事务执行结果，回执信息
};
void * Affair_Res_sender(void* arg)//用于事务处理完成后返回执行结果的回执线程函数
{
    Affair_Res_arg* res_arg=(Affair_Res_arg*) arg;
    ZportPackage* pak_tmp=new ZportPackage;
    strncpy(pak_tmp->pert,res_arg->pert,16);
    pak_tmp->package_flag=7;//提议锁定时间数据包
    strncpy(pak_tmp->affair_msg, res_arg->affair_msg,512);//事务执行结果
    strncpy(pak_tmp->port_name, res_arg->target_port_name,16);
    res_arg->Zsock->Zmessage_sender(pak_tmp,pak_tmp->port_name);//发出消息
    delete pak_tmp;
    delete (Affair_Res_arg*) arg;//清除线程结构体
    pthread_exit(0);
}

class Affair_Moniter//用于监视共享内存事务的控制类，由该类负责控制【待定类】执行链上行为
{
    
private:
    SharedMemory* SM;//共享内存空间
    ZportError* Zerror;//报错提示类
    Chain_Executor* chain_exector;//链操作执行类
    Zsocket* Zsock;//网关类，用于套接字传输，用于事务执行后传输信息
public:
    int process_control;
    chain_type c_t;//管理的链类型
    
public:
    Affair_Moniter(SharedMemory* _SM,ZportError* _Zerror,Chain_Executor* _chain_exector,chain_type _c_t,Zsocket* _Zsock):Zerror(_Zerror),SM(_SM),chain_exector(_chain_exector),c_t(_c_t),Zsock(_Zsock)//构造函数
    {
        this->process_control=0;
    }
    int Get_Total_AffairCout()//返回事务数量
    {
        return this->SM->s_m->affair_cnt;
    }
    int Start_AffairMointer()//启动监视类，随时监视事务坞中是否有合法事务可以执行
    {
        while (1)
        {
            sleep(0.5);//休眠监听
            if(this->process_control==1)
                break;
            for(int i=0;i<SM->s_m->affair_cnt;i++)
            {
                if(SM->s_m->affair_seg[i].moniter_flag==1)//本地链已经创建执行线程
                {
                    if(SM->s_m->affair_seg[i].local_complete_symbol==1)
                    {
                        SM->s_m->affair_seg[i].moniter_flag=2;
                    }
                }
                else if(SM->s_m->affair_seg[i].moniter_flag==0)
                {
                    time_t time_now;
                    time(&time_now);
                    if(SM->s_m->affair_seg[i].remote_time_lock_symbol==1 && SM->s_m->affair_seg[i].affair_lock_time==time_now)//只有远端时间锁定，本地时间锁定，到达锁定时间，才执行
                    {
                        string tmp="在时间【";
                        tmp+=to_string(time_now);
                        tmp+="】创建事务处理线程，事务标识为";
                        tmp+=SM->s_m->affair_seg[i].pert;
                        //Zerror->msg(0, tmp, "", "", 0);
                        Zerror->add_log_msg(tmp);
                        if(SM->s_m->affair_seg[i].affair_flag==2)
                        {
                            //Zerror->msg(0, "跨链数据读事务，仅在对方链上执行", "", "", 0);
                            Zerror->add_log_msg("跨链数据读事务，仅在对方链上执行");
                            SM->s_m->affair_seg[i].moniter_flag=1;
                            SM->s_m->affair_seg[i].local_complete_symbol=1;
                        }
                        else
                        {
                            pthread_t pt;
                            chain_ex_arg* ex_arg=new chain_ex_arg;
                            ex_arg->val=SM->s_m->affair_seg[i].val;
                            ex_arg->affair_flag=SM->s_m->affair_seg[i].affair_flag;//事务类型
                            ex_arg->op=SM->s_m->affair_seg[i].op;
                            strncpy(ex_arg->pert,SM->s_m->affair_seg[i].pert,16);
                            strncpy(ex_arg->tar_usr,SM->s_m->affair_seg[i].tar_user,16);
                            strncpy(ex_arg->contract_name, SM->s_m->affair_seg[i].contract_name, 16);
                            strncpy(ex_arg->contract_function, SM->s_m->affair_seg[i].contract_function, 16);
                            ex_arg->c_t=this->c_t;
                            ex_arg->affair_index=i;
                            ex_arg->SM=this->SM;
                            SM->s_m->affair_seg[i].moniter_flag=1;
                            pthread_create(&pt, NULL, this->chain_exector->Chain_Ex, (void*)ex_arg);
                        }
                    }
                    
                }
                else if(SM->s_m->affair_seg[i].moniter_flag==2)//对于已经执行的事务，向互信网关返回执行结果
                {
                    pthread_t pt_res;//回执线程符号
                    struct Affair_Res_arg* res_arg=new Affair_Res_arg;
                    strncpy(res_arg->pert, SM->s_m->affair_seg[i].pert,16);
                    strncpy(res_arg->target_port_name, SM->s_m->affair_seg[i].port_name,16);//回执事务的网关名称
                    res_arg->Zsock=Zsock;
                    strncpy(res_arg->affair_msg, SM->s_m->affair_seg[i].affair_msg,512);//保存回执信息
                    pthread_create(&pt_res, NULL, Affair_Res_sender, (void*)res_arg);
                    SM->s_m->affair_seg[i].moniter_flag=3;
                }
            }
        }
        return 1;
    }
    ~Affair_Moniter()
    {
        Zerror->msg(0, "释放事务监视模块", "", "", 0);
    }
};





#endif /* ZportChainKitlib_h */
