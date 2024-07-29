//
//  ZportLoglib.h
//  Zport
//
//  Created by Zarror Wang on 2022/4/18.
//

#ifndef ZportLoglib_h
#define ZportLoglib_h
#include <fstream>
#include <stdio.h>
class ZportLog//日志记录类
{
private:
    ofstream log_file;
    ofstream affair_hub_file;
public:
    ZportLog()
    {
        log_file.open("test_log.txt");//打开文件
        affair_hub_file.open("affair_hub.txt");//打开共享内存记录
        if(!log_file.is_open() || !affair_hub_file.is_open())
        {
            cout<<"日志文件创建失败"<<endl;
            fflush(stdout);
        }
    }
    void add_log_msg(string msg)
    {
        log_file<<msg<<endl;
    }
    void add_affair_file(string msg)
    {
        affair_hub_file<<msg<<endl;
    }
    ~ZportLog()
    {
        time_t time_tmp;
        time(&time_tmp);
        string tmp_msg="【";
        tmp_msg+=ctime(&time_tmp);
        tmp_msg.erase(remove(tmp_msg.begin(), tmp_msg.end(), '\n'),tmp_msg.end());
        tmp_msg+=+"】";
        cout<<tmp_msg<<"生成操作日志即事务坞文件"<<endl;
        fflush(stdout);
        log_file.close();
        affair_hub_file.close();
    }
};

#endif /* ZportLoglib_h */
