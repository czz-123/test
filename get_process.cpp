#include<iostream>
#include<thread>
#include<fstream>
#include<ctime>
#include<mutex>
#include <string>
#include <vector>
#ifdef WIN32
#include<iomanip>
#include<windows.h>
#include "w_candm.h"
#else
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "cpu_and_mem.h"
#endif

std::mutex mut;
bool cont = true; //control start and end of monitor process

#ifdef WIN32
void start_process()
{
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;                  
    DWORD returnCode;
    DWORD process_id;


    std::ifstream ifs;
    ifs.open("win\\win_pathfile.txt", std::ios::in);
    if (!ifs.is_open())
    {
        std::cout << "open pathfile failed!!!" << std::endl;
        return;
    }

    std::string ss;
    //std::vector<std::string> comd;
    while (!ifs.eof())
    {
        getline(ifs,ss);
    }
   
    const char* temp = ss.c_str();
    int iSize;
    wchar_t* commandline;
    iSize = MultiByteToWideChar(CP_ACP, 0, temp, -1, NULL, 0); //iSize =wcslen(pwsUnicode)+1=6
    commandline = (wchar_t*)malloc(iSize * sizeof(wchar_t)); 
    MultiByteToWideChar(CP_ACP, 0, temp, -1, commandline, iSize);

    //wchar_t commandLine1[] = L"subapp.exe -l 1 hello 3 www.helllp=.cn";  //���������в���һ
    //const char* temp = "C:\\Users\\czz\\source\\repos\\test_process\\Debug\\test_process.exe";
    //wchar_t commandLine2[] = L"C:\\Users\\czz\\source\\repos\\test_process\\Debug\\test_process.exe";     //���������в�����
    //wchar_t commandLine3[] = L"C:\\Program Files (x86)\\Tencent\\qqmusic\\QQMusic.exe";

    BOOL bRet = CreateProcess(             
        commandline,                       
        NULL,                              
        NULL,                               //_In_opt_    LPSECURITY_ATTRIBUTES lpProcessAttributes,
        NULL,                               //_In_opt_    LPSECURITY_ATTRIBUTES lpThreadAttributes,
        FALSE,                              //_In_        BOOL                  bInheritHandles,
        CREATE_NEW_CONSOLE,                
        NULL,                               //_In_opt_    LPVOID                lpEnvironment,
        NULL,                               //_In_opt_    LPCTSTR               lpCurrentDirectory,
        &si,                                //_In_        LPSTARTUPINFO         lpStartupInfo,
        &pi);                               //_Out_       LPPROCESS_INFORMATION lpProcessInformation
    process_id = pi.dwProcessId;
    std::cout << "child process id " << pi.dwProcessId << std::endl;
    free(commandline);
    commandline = NULL;
    std::ofstream ofs;
    ofs.open("win\\process_id.txt", std::ios::out);
    if (!ofs.is_open())
    {
        std::cout << "child process open id file failed!!!" << std::endl;
        return;
    }
    ofs << process_id << std::endl;
    ofs.close();

    if (bRet)
    {
        std::cout << "child process starting" << std::endl;
        //�ȴ��ӽ��̽���
        //WaitForSingleObject(pi.hProcess, -1);

        //std::cout << "child process ended" << std::endl;
        //��ȡ�ӽ��̵ķ���ֵ
       // GetExitCodeProcess(pi.hProcess, &returnCode);
       // std::cout << "process return code:" << returnCode << std::endl;
    }
    else
    {
        std::cout << "create child process failed!" << std::endl;
        //return 0;
    }
    //    getchar();
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    
}
void monitor_process(int id)
{
    w_candm object;
    std::ofstream ofs;
    ofs.open("win\\cpu_and_mem.txt", std::ios::out);
    if (!ofs.is_open())
    {
        std::cout << "monitor file open failed!!!" << std::endl;
        return;

    }
    while (1)
    {
        if (!cont)
        {
            break;
        }
        ofs << "proc cpu ratio : " << object.GetCpuUsageRatio(id);
        ofs << "  proc mem ratio : " << object.GetMemoryRatio(id) << std::endl;
       
        //std::cout << "proc mem average : " <<  object.GetMemoryAverageRatio() << std::endl;
        //std::cout << "proc mem time : " << object.mem_run_time << std::endl;
        //std::cout << "proc all mem  : " << object.sum_mem << std::endl;
        Sleep(300);
    }
    ofs << "  proc average cpu ratio: " << object.GetCpuAverageRatio() << std::endl;
    ofs << "  proc average mem ratio: " << object.GetMemoryAverageRatio() << std::endl;
    ofs << "proc cpu min ratio:" << object.mincpu << std::endl;
    ofs << "proc cpu max ratio:" << object.maxcpu << std::endl;
    ofs << "proc mem min ratio:" << object.minpmem << std::endl;
    ofs << "proc mem max ratio:" << object.maxpmem << std::endl;
    ofs.close();
}
#else
void test(int pid)
{
    std::ofstream ofs;
    ofs.open("linux/CPU_and_mem_encV.txt", std::ios::out);
    if (!ofs.is_open())
    {
        std::cout << "monitor output file open failed!!!" << std::endl;
        return;
    }
    cpu_and_mem object;
    int i = 0;
    while (1)
    {
        if (!cont)
        {
            break;
        }
        ofs << "proc cpu ratio:" << object.get_proc_cpu(pid);
        ofs << " proc mem ratio:" << object.get_proc_pmem(pid) << std::endl;
        
        //fprintf(fp, " procmem=%d ", object.get_proc_mem(pid));
        //fprintf(fp, " pmem=%lf\n", object.get_proc_pmem(pid));
        //fprintf(fp,"min pcpu = %lf\n",object.mincpu);
        //fprintf(fp," pvirtualmem=%lf\n",object.get_proc_virtualmem(pid));
        std::chrono::milliseconds dura(300);
        std::this_thread::sleep_for(dura);
        //endTime = clock();//��ʱ����
        //fprintf(fp,"The run time is %lf s\n",(float)(endTime - startTime) / CLOCKS_PER_SEC );
        i++;
    }
    float time = 0.3 * i;
    //endTime = clock();//��ʱ����
    //fprintf(fp,"The run time is %lf s\n",(float)(endTime - startTime) / CLOCKS_PER_SEC );
    ofs << "proc cpu min ratio:" << object.mincpu << std::endl;
    ofs << "proc cpu max ratio:" << object.maxcpu << std::endl;
    ofs << "proc mem min ratio:" << object.minpmem << std::endl;
    ofs << "proc mem max ratio:" << object.maxpmem << std::endl;
    ofs << "proc average cpu ratio:" << object.get_proc_avecpu() << std::endl;
    ofs << "proc average mem ratio:" << object.get_proc_avemem() << std::endl;
    ofs << "the run time is : " << time << std::endl;
    /*fprintf(fp, "The run time is %lf s\n", time);
    fprintf(fp, "max pmem = %lf\n", object.maxpmem);
    fprintf(fp, "min pmem = %lf\n", object.minpmem);
    fprintf(fp, "max pcpu = %lf\n", object.maxcpu);
    fprintf(fp, "min pcpu = %lf\n", object.mincpu);
    fprintf(fp, "average pmem = %lf\n", object.get_proc_avemem());
    fprintf(fp, "average pcpu = %lf\n", object.get_proc_avecpu());
    fclose(fp);*/
    ofs.close();
}
#endif
int main(int argc, char* argv[])
{
#ifdef WIN32
    int id;
    std::cout << "main process sucessed create child process !" << std::endl;
    std::thread start_pthread(start_process);
    start_pthread.detach();

    Sleep(1000);
    std::cout << "child process has detach!" << std::endl;

    std::ifstream ifs;
    ifs.open("win\\process_id.txt", std::ios::in);
    if (!ifs.is_open())
    {
        std::cout << "main process failed open chile process id file!!!" << std::endl;
        return 0;
    }
    ifs >> id ;
    ifs.close();
    std::cout << "get child process id: " << id << std::endl;

    std::thread monitor_pthread(monitor_process, id);
    monitor_pthread.detach();

    char temp;
   
    while (1)
    {
        std::cout << "child process is running,whether close (close 0/q): " << std::endl;
        std::cin >> temp;
        if (temp == '0' || temp == 'q')
        {
            mut.lock();
            cont = false;
            mut.unlock();
            HANDLE token = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id); //�ر��ӽ���
            TerminateProcess(token, 0);
            break;
        }
        else
        {
            std::cout << "child process continue......" << std::endl;
            continue;
        }
    }   
    system("pause");
    return 0;
#else
    int begin, end1;
    begin = time(NULL);
    pid_t pid;

    pid = fork();

    if (pid == 0)
    {
        std::ifstream ifs;
        ifs.open("linux/pathfile.txt", std::ios::in);
        if (!ifs.is_open())
        {
            std::cout << "path file open failed!!!" << std::endl;
            return 0;
        }
        std::string ss;

        getline(ifs, ss);
        execlp("/bin/bash", "bash", "-c", (char*)ss.c_str(), NULL);
        ifs.close();

    }

    printf("detact process id is %d\n", pid);

    std::thread my_thread(test, pid);//���߳�������������صĽ���
    my_thread.detach();//���߳��ں�̨����

  
    int temp;
    while (1)
    {
        printf("(1)stop   (2)end  (3)continue  (0)exit");
        scanf("%d", &temp);
        switch (temp) {
        case 1: {
            int state;
            state = kill(pid, SIGSTOP);
            if (state)
            {
                perror("kill error!");
            }
            else
            {
                printf("codetest process is stopping!");
            }
            break;
        }
        case 2:
        {
            int state;
            state = kill(pid, SIGINT);
            if (state)
            {
                perror("kill error!");
            }
            else
            {
                printf("codetest process was ended!");
            }
            mut.lock();
            cont = false;
            mut.unlock();
            end1 = time(NULL);
            printf("time=%d\n", end1 - begin);
            break;
        }
        case 3: {
            int state;
            state = kill(pid, SIGCONT);
            if (state)
            {
                perror("kill error!");
            }
            else
            {
                printf("codetest process  continue!");
            }
            break;
        }
        case 0: {
            //system("pkill codetest");
            exit(0);
            break;
        }
        default:
            printf("input error!!!");
            break;
        }

    }
    //system("pkill codetest");
    return 0;
#endif
}
