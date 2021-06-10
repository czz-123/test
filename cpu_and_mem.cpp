//
// Created by linuxlearn on 2021/4/29.
//

#include "cpu_and_mem.h"

#define VMRSS_LINE 18
#define VMSIZE_LINE 14
#define PROCESS_ITEM 14

//构造函数
cpu_and_mem::cpu_and_mem()
{
    maxpmem = 0.0;
    minpmem = 100.0;
    maxcpu = 0.0;
    mincpu = 100.0;
    cpu_run_time = 0;
    mem_run_time = 0;
    avecpu = 0.0;
    avemem = 0.0;
    allcpu = 0.0;
    allmem = 0.0;
}

//获取第N项开始的指针
const char* get_items(const char*buffer ,unsigned int item){

    const char *p =buffer;

    int len = strlen(buffer);
    int count = 0;

    for (int i=0; i<len;i++){
        if (' ' == *p){
            count ++;
            if(count == item -1){
                p++;
                break;
            }
        }
        p++;
    }

    return p;
}


//获取总的CPU时间
unsigned long cpu_and_mem::get_cpu_total_occupy(){

    FILE *fd;
    char buff[1024]={0};
    Total_Cpu_Occupy_t t;

    fd =fopen("/proc/stat","r");
    if (nullptr == fd){
        return 0;
    }

    fgets(buff,sizeof(buff),fd);
    char name[64]={0};
    sscanf(buff,"%s %ld %ld %ld %ld",name,&t.user,&t.nice,&t.system,&t.idle);
    fclose(fd);

    return (t.user + t.nice + t.system + t.idle);
}


//获取进程的CPU时间
unsigned long cpu_and_mem::get_cpu_proc_occupy(unsigned int pid){

    char file_name[64]={0};
    Proc_Cpu_Occupy_t t;
    FILE *fd;
    char line_buff[1024]={0};
    sprintf(file_name,"/proc/%d/stat",pid);
    fd = fopen(file_name,"r");
    if(nullptr == fd){
        return 0;
    }

    fgets(line_buff,sizeof(line_buff),fd);

    sscanf(line_buff,"%u",&t.pid);
    const char *q =get_items(line_buff,PROCESS_ITEM);
    sscanf(q,"%ld %ld %ld %ld",&t.utime,&t.stime,&t.cutime,&t.cstime);
    fclose(fd);

    return (t.utime + t.stime + t.cutime + t.cstime);
}


//获取CPU占用率
float cpu_and_mem::get_proc_cpu(unsigned int pid){

    totalcputime1=get_cpu_total_occupy();
    procputime1=get_cpu_proc_occupy(pid);

    //usleep(100000);

    //totalcputime2=get_cpu_total_occupy();
    //procputime2=get_cpu_proc_occupy(pid);

    float pcpu = 0.0;
    if(totalcputime2==totalcputime1||procputime1==procputime2 ||totalcputime1 == 0|| procputime1 == 0)
    {
        pcpu = 0.0;
    }
    else
    {
        pcpu=(100.0 * (procputime1-procputime2))/(totalcputime1-totalcputime2);
    }
    totalcputime2 = totalcputime1;
    procputime2 = procputime1;
    if(maxcpu < pcpu)
    {
        maxcpu = pcpu;
    }
    if(mincpu > pcpu && pcpu != 0.00)
    {
        mincpu = pcpu;
    }
    if(pcpu < 0.01)
    {
        return pcpu;
    }
    else
    {
        allcpu += pcpu;
        cpu_run_time++;
        return pcpu;
    }
    //return pcpu;
}



// unsigned int cpu_and_mem::get_proc_mem(unsigned int pid){

//     char file_name[64]={0};
//     FILE *fd;
//     char line_buff[512]={0};
//     sprintf(file_name,"/proc/%d/status",pid);

//     fd =fopen(file_name,"r");
//     if(nullptr == fd){
//         return 0;
//     }

//     char name[64];
//     int vmrss;
//     for (int i=0; i<VMRSS_LINE-1;i++){
//         fgets(line_buff,sizeof(line_buff),fd);
//     }

//     fgets(line_buff,sizeof(line_buff),fd);
//     sscanf(line_buff,"%s %d",name,&vmrss);
//     fclose(fd);

//     return vmrss;
// }
//获取进程占用内存
unsigned int cpu_and_mem::get_proc_mem(unsigned int pid)
{
	long int pmem;
	std::ifstream ifs;
	//int pid = 2752;
	std::string fpid = std::to_string(pid);
	std::string path = "/proc/"+fpid+"/status";
	ifs.open(path,std::ios::in);
	if(!ifs.is_open())
	{
		std::cout << "file open faile!!!" << std::endl;
	}
	std::string buff;
	bool isexit;
	while( ifs >> buff)
	{
		if(buff == "VmRSS:")
		{
			while(1)
			{
				ifs>>buff;
				if(buff == " ")
				{
					continue;
				}
				else
				{
					isexit = false;
					pmem = atoi(buff.c_str());
					//std::cout << pmem  <<std::endl;
					break;
				}
			}
			break;
		}
	}
	ifs.close();
	if(isexit)
	{
		std::cout << "not found target!!" << std::endl;
	}
	return pmem;
}


//获取进程占用虚拟内存
unsigned int cpu_and_mem::get_proc_virtualmem(unsigned int pid){

    char file_name[64]={0};
    FILE *fd;
    char line_buff[512]={0};
    sprintf(file_name,"/proc/%d/status",pid);

    fd =fopen(file_name,"r");
    if(nullptr == fd){
        return 0;
    }

    char name[64];
    int vmsize;
    for (int i=0; i<VMSIZE_LINE-1;i++){
        fgets(line_buff,sizeof(line_buff),fd);
    }

    fgets(line_buff,sizeof(line_buff),fd);
    sscanf(line_buff,"%s %d",name,&vmsize);
    fclose(fd);

    return vmsize;
}

//获取计算机总内存
unsigned int get_total_mem(unsigned int pid){
    char file_name[64]={0};
    FILE *fd;
    char line_buff[512]={0};
    fd =fopen("/proc/meminfo","r");
    if(nullptr == fd){
        return 0;
    }
    char name[64];
    int total_mem;
    fgets(line_buff,sizeof(line_buff),fd);
    sscanf(line_buff,"%s %d",name,&total_mem);
    fclose(fd);
    return total_mem;
}
//获取进程内存占有率
float cpu_and_mem::get_proc_pmem(unsigned int pid){
    unsigned int total_mem,proc_mem;
    total_mem = get_total_mem(pid);
    proc_mem = get_proc_mem(pid);
    float pmem;
    pmem = (float)100*proc_mem/total_mem;
    if(pmem > 100.0)
    {
        return 0.0;
    }
    if(maxpmem < pmem)
    {
        maxpmem = pmem;
    }
    if(minpmem > pmem && pmem != 0.0000)
    {
        minpmem = pmem;
    }
    if(pmem == 0.0)
    {
        return pmem;
    }
    else
    {
        allmem += pmem;
        mem_run_time++;
        return pmem;
    }
    //return pmem;
}
//获取进程内存平均值
float     cpu_and_mem::get_proc_avemem() {
    if(mem_run_time)
    {
        return (float) allmem / mem_run_time;
    }
    else
    {
        return 0.0;
    }

}

//获取CPU平均占有率
float cpu_and_mem::get_proc_avecpu()
{
    if(cpu_run_time)
    {
        return (float) allcpu / cpu_run_time;
    }
    else
    {
        return 0.0;
    }

}
