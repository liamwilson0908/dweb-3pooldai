//
//  ZportClient.h
//  Zport
//
//  Created by Zarror Wang on 2022/5/20.
//

#ifndef ZportClient_h
#define ZportClient_h
#include "Zportlib.h"

class ZportClient
{
public:
    Zport* _Zport;
    ZportError* _Zerror;
    Affair_Moniter* _Affair_Mointer;
    
public:
    int cmd_segmentation_zp1(string &cmd_line,string &contract_name,string &contract_function,string &function_para)
    {
        string t[4]={""};
        int flag=0;
        for(int i=0;i<cmd_line.length();i++)
        {
            if(cmd_line[i]!=' ' && cmd_line[i]!='\0')
            {
                t[flag]+=cmd_line[i];
            }
            else
            {
                flag++;
            }
        }
        if(flag!=3)
            return -1;
        contract_name=t[1];
        contract_function=t[2];
        function_para=t[3];
        return 1;
    }
    int cmd_segmentation_zp2(string &cmd_line,string &chain_name,string &contract_name,string &contract_function,string &function_para)
    {
        string t[5]={""};
        int flag=0;
        for(int i=0;i<cmd_line.length();i++)
        {
            if(cmd_line[i]!=' ' && cmd_line[i]!='\0')
            {
                t[flag]+=cmd_line[i];
            }
            else
            {
                flag++;
            }
        }
        if(flag!=4)
            return -1;
        chain_name=t[1];
        contract_name=t[2];
        contract_function=t[3];
        function_para=t[4];
        return 1;
    }
    int para_segmentation_zp1(string &para_line,string &usr,int &val)
    {
        string t[2]={""};
        int flag=0;
        for(int i=0;i<para_line.length();i++)
        {
            if(para_line[i]!=',' && para_line[i]!='\0')
            {
                t[flag]+=para_line[i];
            }
            else
            {
                //t[flag]+='\0';
                flag++;
            }
        }
        if(flag!=1)
            return -1;
        usr=t[0];
        val=atoi(t[1].c_str());
        return 1;
    }
    int para_segmentation_zp2(string &para_line,string &pro_usr,string &tar_usr,int &val)
    {
        string t[3]={""};
        int flag=0;
        for(int i=0;i<para_line.length();i++)
        {
            if(para_line[i]!=',' && para_line[i]!='\0')
            {
                t[flag]+=para_line[i];
            }
            else
            {
                t[flag]+='\0';
                flag++;
            }
        }
        if(flag!=2)
            return -1;
        pro_usr=t[0];
        tar_usr=t[1];
        val=atoi(t[2].c_str());
        return 1;
    }
    int Start_ZportClient()
    {
        _Zerror->usr_client_info(_Zport->port_name,_Zport->register_chain_type["test"]);
        string usr_cmd="";
        
        while(getline(cin,usr_cmd))
        {
            if(usr_cmd=="quit")//退出网关
            {
                _Zport->process_control=1;
                _Affair_Mointer->process_control=1;
                break;
            }
            if(usr_cmd=="show hub")//展示事务坞
            {
                _Zport->Zaffair->Dis_AffairHub();
                fflush(stdout);
                continue;
            }
            //cout<<usr_cmd<<endl;
            if(strncmp("Zport1", usr_cmd.c_str(),6)==0)
            {
                _Zerror->msg(0, "本地链事务", "", "", 0);
                string contract_name="";//链上合约名
                string contract_function="";//链上合约方法
                string function_para="";//合约参数
                if(cmd_segmentation_zp1(usr_cmd, contract_name, contract_function, function_para)!=1)
                {
                    _Zerror->msg(0, "命令非法，请重新输入", "", "", 0);
                    continue;
                }
                if(find(_Zport->chain_contract["this"].begin(),_Zport->chain_contract["this"].end(),contract_name)==_Zport->chain_contract["this"].end())//不存在这个合约
                {
                    _Zerror->msg(0, "命令非法，当前访问链上不存在该合约", "", "", 0);
                    continue;
                }
                if(find(_Zport->contract_function[contract_name].begin(),_Zport->contract_function[contract_name].end(),contract_function)==_Zport->contract_function[contract_name].end())//不存在这个方法
                {
                    _Zerror->msg(0, "命令非法，当前访问链上合约不存在该方法", "", "", 0);
                    continue;
                }
                //                cout<<"contract_name----"<<contract_name<<endl;
                //                cout<<"contract_function----"<<contract_function<<endl;
                //                cout<<"function_para----"<<function_para<<endl;
                //                fflush(stdout);
                if(_Affair_Mointer->c_t==fabric)
                {
                    string chain_line="source /etc/profile;goduck fabric contract invoke transferCC ";
                    int val=0;
                    string usr="";
                    if(contract_function=="getBalance")
                        chain_line+=contract_function+" "+function_para;
                    else if(contract_function=="setBalance")
                    {
                        if(para_segmentation_zp1(function_para,usr,val)!=1)
                        {
                            _Zerror->msg(0, "参数非法，请重新输入", "", "", 0);
                            continue;
                        }
                        chain_line+=contract_function;
                        chain_line+=" ";
                        chain_line+=usr;
                        chain_line+=",";
                        chain_line+=to_string(val);
                    }
                    FILE* fp=popen(chain_line.c_str(),"r");
                    char* buf=(char*)malloc(2048*sizeof(char));
                    int buf_size=(int)fread(buf, 1, 2048, fp);
                    pclose(fp);
                    _Zport->Zerror->local_affair_msg(buf);
                    free(buf);
                }
                else if(_Affair_Mointer->c_t==ether)
                {
                    string chain_line="source /etc/profile;cd ~/go/src/pier-client-ethereum/example;";//此为以太坊私链上的执行指令
                    int val=0;
                    string usr="";
                    if(contract_function=="getBalance")
                        chain_line+=contract_function+" "+function_para;
                    else if(contract_function=="setBalance")
                    {
                        if(para_segmentation_zp1(function_para,usr,val)!=1)
                        {
                            _Zerror->msg(0, "参数非法，请重新输入", "", "", 0);
                            continue;
                        }
                        chain_line+=contract_function;
                        chain_line+=" ";
                        chain_line+=usr;
                        chain_line+=",";
                        chain_line+=to_string(val);
                    }
                    FILE* fp=popen(chain_line.c_str(),"r");
                    char* buf=(char*)malloc(2048*sizeof(char));
                    int buf_size=(int)fread(buf, 1, 2048, fp);
                    pclose(fp);
                    _Zport->Zerror->local_affair_msg(buf);
                    free(buf);
                }
            }
            else if(strncmp("Zport2", usr_cmd.c_str(),6)==0)
            {
                _Zerror->msg(0, "跨链事务", "", "", 0);
                string chain_name="";//业务链名称
                string contract_name="";//链上合约名
                string contract_function="";//链上合约方法
                string function_para="";//合约参数
                if(cmd_segmentation_zp2(usr_cmd, chain_name, contract_name, contract_function, function_para)!=1)
                {
                    _Zerror->msg(0, "命令非法，请重新输入", "", "", 0);
                    continue;
                }
                else
                {
                    //                    cout<<"调用参数为"<<endl;
                    //                    cout<<"chain_name----"<<chain_name<<endl;
                    //                    cout<<"contract_name----"<<contract_name<<endl;
                    //                    cout<<"contract_function----"<<contract_function<<endl;
                    //                    cout<<"function_para----"<<function_para<<endl;
                    //                    fflush(stdout);
                    if(find(_Zport->register_chain.begin(),_Zport->register_chain.end(),chain_name)==_Zport->register_chain.end())//不存在这条链
                    {
                        _Zerror->msg(0, "命令非法，当前链未注册", "", "", 0);
                        continue;
                    }
                    if(find(_Zport->chain_contract[chain_name].begin(),_Zport->chain_contract[chain_name].end(),contract_name)==_Zport->chain_contract[chain_name].end())//不存在这个合约
                    {
                        _Zerror->msg(0, "命令非法，当前访问链上不存在该合约", "", "", 0);
                        continue;
                    }
                    if(find(_Zport->contract_function[contract_name].begin(),_Zport->contract_function[contract_name].end(),contract_function)==_Zport->contract_function[contract_name].end())//不存在这个方法
                    {
                        _Zerror->msg(0, "命令非法，当前访问链上合约不存在该方法", "", "", 0);
                        continue;
                    }
                    
                    if(_Zport->function_type[contract_function]==0)//读数据跨链操作
                    {
                        ZportPackage* pak=new ZportPackage;
                        pak->package_flag=1;
                        pak->affair_flag=0;
                        strncpy(pak->contract_name,contract_name.c_str(),16);
                        strncpy(pak->contract_function,contract_function.c_str(),16);
                        strncpy(pak->tar_usr,function_para.c_str(),16);
                        strncpy(pak->port_name, chain_name.c_str(),16);
                        _Zport->Zsock->Zmessage_sender(pak,chain_name);
                        delete pak;
                    }
                    else if(_Zport->function_type[contract_function]==1)//写数据跨链操作
                    {
                        ZportPackage* pak=new ZportPackage;
                        pak->package_flag=1;
                        pak->affair_flag=1;
                        strncpy(pak->contract_name,contract_name.c_str(),16);
                        strncpy(pak->contract_function,contract_function.c_str(),16);
                        strncpy(pak->port_name, chain_name.c_str(),16);
                        string pro_usr,tar_usr;
                        int val;
                        if(para_segmentation_zp2(function_para, pro_usr, tar_usr, val)!=1)
                        {
                            _Zerror->msg(0, "参数非法，请重新输入", "", "", 0);
                            continue;
                        }
                        strncpy(pak->pro_usr,pro_usr.c_str(),16);
                        strncpy(pak->tar_usr,tar_usr.c_str(),16);
                        pak->val=val;
                        _Zport->Zsock->Zmessage_sender(pak,chain_name);
                        delete pak;
                        
                    }
                }
            }
            else
            {
                _Zerror->msg(0, "命令非法，请重新输入", "", "", 0);
                continue;
            }
            
        }
        return 1;
    }
    ZportClient(Zport* _zport,ZportError* _zerror,Affair_Moniter* _affair_mointer):_Zport(_zport),_Zerror(_zerror),_Affair_Mointer(_affair_mointer)
    {
    }
    ~ZportClient()
    {
        _Zerror->msg(0, "释放客户端管理类", "", "", 0);
    }
};

#endif /* ZportClient_h */
