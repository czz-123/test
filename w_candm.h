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
	//ʱ��ת��
	static uint64_t convert_time_format(const FILETIME* ftime);

	//��ȡ����CPUʹ����
	float GetCpuUsageRatio(int pid);

	//��ȡ�����ڴ��С
	float GetMemoryUsage(int pid);

	//��ȡ������ڴ�����
	float GetMemoryall();

	//��ȡ�����ڴ�ռ����
	float GetMemoryRatio(int pid);

	//��ȡCPUƽ��ռ����
	float GetCpuAverageRatio();

	//��ȡ�ڴ�ƽ��ռ����
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

