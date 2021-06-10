#include "w_candm.h"

static const int kMaxInfoBuffer = 256;
#define  GBYTES  1073741824    
#define  MBYTES  1048576    
#define  KBYTES  1024    
#define  DKBYTES 1024.0 

w_candm::w_candm()
{
    maxpmem = 0.0;
    minpmem = 100.0;
    maxcpu = 0.0;
    mincpu = 100.0;
    cpu_run_time = 0;
    mem_run_time = 0;
    avecpu = 0.0;
    avemem = 0.0;
    sum_cpu = 0.0;
    sum_mem = 0.0;
}
uint64_t w_candm::convert_time_format(const FILETIME* ftime)
{
    LARGE_INTEGER li;

    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

float w_candm::GetCpuUsageRatio(int pid)
{
    static int64_t last_time = 0;
    static int64_t last_system_time = 0;

    FILETIME now;
    FILETIME creation_time;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;
    int64_t system_time;
    int64_t time;
    int64_t system_time_delta;
    int64_t time_delta;

    // get cpu num
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    int cpu_num = info.dwNumberOfProcessors;

    float cpu_ratio = 0.0;

    // get process hanlde by pid
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    // use GetCurrentProcess() can get current process and no need to close handle

    // get now time
    GetSystemTimeAsFileTime(&now);

    if (!GetProcessTimes(process, &creation_time, &exit_time, &kernel_time, &user_time))
    {
        // We don't assert here because in some cases (such as in the Task Manager)
        // we may call this function on a process that has just exited but we have
        // not yet received the notification.
        printf("GetCpuUsageRatio GetProcessTimes failed\n");
        return 0.0;
    }

    // should handle the multiple cpu num
    system_time = (convert_time_format(&kernel_time) + convert_time_format(&user_time)) / cpu_num;
    time = convert_time_format(&now);

    if ((last_system_time == 0) || (last_time == 0))
    {
        // First call, just set the last values.
        last_system_time = system_time;
        last_time = time;
        return 0.0;
    }

    system_time_delta = system_time - last_system_time;
    time_delta = time - last_time;

    CloseHandle(process);

    if (time_delta == 0)
    {
        printf("GetCpuUsageRatio time_delta is 0, error\n");
        return 0.0;
    }

    // We add time_delta / 2 so the result is rounded.
    // cpu_ratio = (int)((system_time_delta * 100 + time_delta / 2) / time_delta); // the % unit
    cpu_ratio = (float)(system_time_delta * 100 + time_delta / 2) / time_delta; // the % unit
    last_system_time = system_time;
    last_time = time;

    //cpu_ratio /= 100.0; // convert to float number

    if (maxcpu < cpu_ratio)
    {
        maxcpu = cpu_ratio;
    }
    if (mincpu > cpu_ratio && cpu_ratio != 0.00)
    {
        mincpu = cpu_ratio;
    }
    if (cpu_ratio < 0.00001)
    {
        return cpu_ratio;
    }
    else
    {
        sum_cpu += cpu_ratio;
        cpu_run_time++;
        return cpu_ratio;
    }

    //return cpu_ratio;
}

float w_candm::GetMemoryall()
{
    std::string memory_info;
    MEMORYSTATUSEX statusex;
    statusex.dwLength = sizeof(statusex);
    float decimal_total = 0;// decimal_avl = 0;
    if (GlobalMemoryStatusEx(&statusex))
    {
        unsigned long long total = 0, remain_total = 0;// avl = 0, remain_avl = 0;
       
        remain_total = statusex.ullTotalPhys ;
        total = statusex.ullTotalPhys;
        decimal_total += total;
        //std::cout << decimal_total << std::endl; 
    }
  
    return decimal_total;
}


float w_candm::GetMemoryUsage(int pid)
{
    uint64_t pmem = 0, vmem = 0;
    PROCESS_MEMORY_COUNTERS pmc;

    // get process hanlde by pid
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (GetProcessMemoryInfo(process, &pmc, sizeof(pmc)))
    {
        pmem = pmc.WorkingSetSize;
        vmem = pmc.PagefileUsage;
    }
    CloseHandle(process);

    return pmem;
}

//获取进程内存占用率
float w_candm::GetMemoryRatio(int pid)
{
    float pmem;
    float allmem = GetMemoryall();
    float promem = GetMemoryUsage(pid);
    pmem = (float)100 * promem / allmem;
    if (maxpmem < pmem)
    {
        maxpmem = pmem;
    }
    if (minpmem > pmem && pmem != 0.0000)
    {
        minpmem = pmem;
    }
    if (pmem == 0.0)
    {
        return pmem;
    }
    else
    {
        sum_mem += pmem;
        mem_run_time++;
        return pmem;
    }
}
//获取进程内存平均值
float    w_candm::GetMemoryAverageRatio() {
    if (mem_run_time)
    {
        return (float)sum_mem / mem_run_time;
    }
    else
    {
        return 0.0;
    }

}

//获取CPU平均占有率
float w_candm::GetCpuAverageRatio()
{
    if (cpu_run_time)
    {
        return (float)sum_cpu / cpu_run_time;
    }
    else
    {
        return 0.0;
    }

}