#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <string.h>
#include <windows.h>
#include <psapi.h>
//#include <tlhelp32.h>
#include <direct.h>
#include <process.h>
class w_candm
{
public:
	w_candm();
	//时间转换
	static uint64_t convert_time_format(const FILETIME* ftime);

	//获取进程CPU使用率
	float GetCpuUsageRatio(int pid);

	//获取进程内存大小
	float GetMemoryUsage(int pid);

	//获取计算机内存总量
	float GetMemoryall();

	//获取进程内存占用率
	float GetMemoryRatio(int pid);

	//获取CPU平均占有率
	float GetCpuAverageRatio();

	//获取内存平均占有率
	float GetMemoryAverageRatio();


	int mem_run_time;
	int cpu_run_time;
	float maxpmem;
	float minpmem;
	float maxcpu;
	float mincpu;
	float sum_cpu;
	float sum_mem;
	float avecpu;
	float avemem;
};

