//
// Created by linuxlearn on 2021/4/29.
//

#ifndef TWO_PROCESS3_CPU_AND_MEM_H
#define TWO_PROCESS3_CPU_AND_MEM_H

#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>

class cpu_and_mem {
public:
    cpu_and_mem();
    
    //获取总的CPU时间
    unsigned long get_cpu_total_occupy();

    //获取进程的CPU时间
    unsigned long get_cpu_proc_occupy(unsigned int pid);

    //获取CPU占用率
    float get_proc_cpu(unsigned int pid);

    //获取进程占用内存
    unsigned int get_proc_mem(unsigned int pid);

    //获取进程占用虚拟内存
    unsigned int get_proc_virtualmem(unsigned int pid);

    //进程本身
    //int get_pid(const char* process_name, const char* user = nullptr);

    //获取进程内存占有率
    float get_proc_pmem(unsigned int pid);

    //获取CPU平均占有率
    float get_proc_avecpu();

    //获取内存平均占有率
    float get_proc_avemem();

    unsigned long totalcputime1,totalcputime2;
    unsigned long procputime1,procputime2;
    float maxpmem;
    float minpmem;
    float maxcpu;
    float mincpu;
    int mem_run_time;
    int cpu_run_time;
    float allcpu;
    float allmem;
    float avecpu;
    float avemem;
};

typedef struct {
    unsigned long user;
    unsigned long nice;
    unsigned long system;
    unsigned long idle;
}Total_Cpu_Occupy_t;


typedef struct {
    unsigned int pid;
    unsigned long utime;  //user time
    unsigned long stime;  //kernel time
    unsigned long cutime; //all user time
    unsigned long cstime; //all dead time
}Proc_Cpu_Occupy_t;
#endif //TWO_PROCESS3_CPU_AND_MEM_H
