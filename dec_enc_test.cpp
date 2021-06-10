#include <iostream>
#include <thread>
#include <fstream>
#include <ctime>
#include <mutex>
#include <string>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "cpu_and_mem.h"

//H265
//./mpi_enc_test -i /root/sources/CPUandMEM/518test/testfile/h265720p.yuv -w 1280 -h 720 -o /root/sources/CPUandMEM/vedio2.h265
//./mpi_dec_test -i /root/sources/CPUandMEM/testfile/db720p.h265 -t 16777220 -o /root/sources/CPUandMEM/testfile/h265720p.yuv -n 120

//H264
//./mpi_enc_test -i /root/sources/CPUandMEM/518test/testfile/h264720p.yuv -w 1280 -h 720 -o /root/sources/CPUandMEM/vedio2.h264
//./mpi_dec_test -i /root/sources/CPUandMEM/testfile/db720p.h264 -t 7 -o /root/sources/CPUandMEM/testfile/h264720p.yuv -n 300


//H265
//./mpi_enc_test -i /root/sources/CPUandMEM/518test/testfile/h265720p.yuv -w 1280 -h 720 -o /root/sources/CPUandMEM/vedio2.h265
//./mpi_dec_test -i /root/sources/CPUandMEM/testfile/music720p.h265 -t 16777220 -o /root/sources/CPUandMEM/testfile/h265720p.yuv -n 120

//H264
//./mpi_enc_test -i /root/sources/CPUandMEM/518test/testfile/h264720p.yuv -w 1280 -h 720 -o /root/sources/CPUandMEM/vedio2.h264
//./mpi_dec_test -i /root/sources/CPUandMEM/testfile/h264720p.h264 -t 7 -o /root/sources/CPUandMEM/testfile/h264720p.yuv -n 300


#define num_child 19  //解码路数
std::mutex mut;      //线程锁
bool cont = true;    //控制监测进程的结束
int *child_id = new int[num_child+1]; // 子进程id数组(1个编码和num_child个解码)

void test(int pid,int i)  // 监测子进程CPU和内存使用情况的函数
{
    std::ofstream ofs;
    std::string path = "CPU_and_mem"+std::to_string(i)+".txt";
    ofs.open(path, std::ios::out);
    if (!ofs.is_open())
    {
        std::cout << "monitor output file open failed!!!" << std::endl;
        return;
    }
    cpu_and_mem object;
    
    while (1)
    {
        if(!cont)
        {
            break;
        }
        ofs << "proc cpu ratio:" << object.get_proc_cpu(pid);
        ofs << " proc mem ratio:" << object.get_proc_pmem(pid) << std::endl;
      
        std::chrono::milliseconds dura(300);
        std::this_thread::sleep_for(dura);
    }
    std::cout << "cout monitor sum !" << std::endl;
    ofs << "proc cpu min ratio:" << object.mincpu << std::endl;
    ofs << "proc cpu max ratio:" << object.maxcpu << std::endl;
    ofs << "proc mem min ratio:" << object.minpmem << std::endl;
    ofs << "proc mem max ratio:" << object.maxpmem << std::endl;
    ofs << "proc average cpu ratio:" << object.get_proc_avecpu() << std::endl;
    ofs << "proc average mem ratio:" << object.get_proc_avemem() << std::endl;
    
    ofs.close();
}



int main()
{
    
   pid_t pid;
   int i;
   for (i = 0; i < num_child; ++i)
   {
       pid=fork();
       child_id[i] = pid;
       // 这是异常情况
       if (pid==-1)
       {
           perror("fork失败!");
           exit(1);
       }
       //循环中，fork函数调用五次，子进程返回0,父进程返回子进程的pid，
       //为了避免子进程也fork，需要判断并break
       if (pid==0)
       {
           break;
       }
   }

   //返回大于0的进程就是父进程
   if(pid>0)  //父进程
   {
        printf("父进程: pid= %d , ppid=%d,子进程: %d \n", getpid(),getppid(),pid);
        pid=fork();
        child_id[num_child] = pid;
        if(pid == 0)
        {
            //printf("i的变量是: %d 子进程: pid= %d , ppid=%d \n", i,getpid(),getppid());
                execlp("/bin/bash", "bash", "-c","/root/sources/CPUandMEM/mpp/build/linux/aarch64/test/mpi_enc_test -i /root/sources/CPUandMEM/518test/testfile/h264720p.yuv -w 1280 -h 720 -t 7",NULL);
        } 
        else if(pid > 0) // 以下为主进程内容
        {
            //std::vector<std::thread>mythreads;
            // 创建num_child个线程，线程入口函数统一使用test.
            // for (int i = 0; i < num_child+1; i++)
            // {
            //     mythreads.push_back(std::thread(test, child_id[i],i)); // 创建了num_child个线程，同时这num_child个已经开始执行了
            // }

            // for (auto iter = mythreads.begin(); iter != mythreads.end(); ++iter)
            // {
            //     iter->detach(); // 不用等待着num_child个线程都返回
            // }

            //sleep(1); //这里延迟父进程程序，等子进程先执行完。
            pid_t wpid;
            int status;
            int k = 0;
            while(1)  //子进程结束后，回收子进程PCB中的内容
            {
                if((wpid = waitpid(-1,&status,WNOHANG)) != -1 && wpid != 0)
                {
                    if(WIFEXITED(status))  //如果子进程正常退出，则返回子进程的返回值
                    {
                        std::cout<<"---catch pid "<< wpid <<" return value "<<WEXITSTATUS(status)<<std::endl; //打印子进程PID和子进程返回值
                        k++;
                        mut.lock();        //回收子进程后利用布尔值设定结束监测线程
                        cont = false;
                        mut.unlock();
                    }
                    if(WIFSIGNALED(status)) //如果子进程非正常退出，则返使子进程结束的信号值
                    {
                        std::cout << "child process was break by signal:" << WTERMSIG(status) << std::endl;
                        k++;
                        mut.lock();
                        cont = false;
                        mut.unlock();
                    }
                }
                if(k >= num_child+1) // 所有的子进程回收完毕，退出while循环
                {
                    break;
                }
                
            }
            sleep(2);    
            std::cout << k << std::endl;
            delete[] child_id;
            return 0;
        }

    }
    else if(pid==0)  //子进程
    {  
        sleep(2); //等待解码运行起来
        execlp("/bin/bash", "bash", "-c","/root/sources/CPUandMEM/mpp/build/linux/aarch64/test/mpi_dec_test -i /root/sources/CPUandMEM/testfile/db720p.h264 -t 7 -w 1280 -h 720  -n 300",NULL);  
    }
}
