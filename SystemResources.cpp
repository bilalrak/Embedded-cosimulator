/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SystemResources.cpp
 * Author: bilalrak
 * 
 * Created on May 13, 2017, 7:28 PM
 */

#include "SystemResources.h"
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
  //for ::getpid() and sysconf(_SC_NPROCESSORS_ONLN);

using namespace std;

SystemResources::SystemResources()
{
    myapplicationPID = to_string(::getpid());
    //string name_proc_stat = "/proc/stat";
    //fs_proc_stat.open(name_proc_stat.c_str(),ios::in);

    //string name_proc_pid_stat = "/proc/";
    //name_proc_pid_stat += myapplicationPID + "/stat";
    //fs_proc_pid_stat.open(name_proc_pid_stat.c_str(),ios::in);

    //string name_proc_pid_sched = "/proc/";
    //name_proc_pid_sched += myapplicationPID + "/sched";
    //fs_proc_pid_sched.open(name_proc_pid_sched.c_str(), ios::in);

    string name_proc_cpuinfo = "/proc/cpuinfo";
    fs_proc_cpuinfo.open(name_proc_cpuinfo.c_str(), ios::in);

    //string name_proc_meminfo = "/proc/meminfo";
    //fs_proc_meminfo.open(name_proc_meminfo.c_str(),ios::in);


    //number of processors

    CPU_count = sysconf(_SC_NPROCESSORS_ONLN);
    sc_CLK_tck = sysconf(_SC_CLK_TCK);
    cout << "number of processor(s):" << CPU_count << " | sc_clk_tck: " << sc_CLK_tck << " | procPID:"<<myapplicationPID<<endl;
}



bool SystemResources::getSystem_stat(unsigned int cpuNum,uint64_t & user,uint64_t & system,uint64_t &idle, uint64_t&iowait)
{
    
    /*
     * [0]->user
     * [1]->system
     * [2]->idle
     * [3]->iowait
     */
    
    bool found=false;
    string line;
    string str="cpu" +to_string(cpuNum);
    string name_proc_stat = "/proc/stat";
    fs_proc_stat.open(name_proc_stat.c_str(),ios::in);
    while(getline(fs_proc_stat,line))
    {
        if(line.find(str,0)!=-1)
        {
            found=true;
            std::stringstream stream(line);
            stream.ignore(15,' ');
            stream>>user;                //user
            
            stream.ignore(15,' ');
            stream.ignore(15,' ');
            stream>>system;                //system
            
            //stream.ignore(15,' ');
            stream>>idle;                //idle
            
            //stream.ignore(15,' ');
            stream>>iowait;                //iowait
            
            fs_proc_stat.close();
            return found;
        }
        
    }
    fs_proc_stat.close();
    
    
    
    return found;
}

void SystemResources::getSystem_mem(uint64_t & totalMEM,uint64_t & freeMEM, uint64_t & avaiMEM)
{
    /*
     * [0]->memtotal
     * [1]->memFree
     * [2]->memAvailable 
     */
    string name_proc_meminfo = "/proc/meminfo";
    fs_proc_meminfo.open(name_proc_meminfo.c_str(),ios::in);
    string line;
    string str="Buffers";
    std::vector<uint64_t> memstats;
    while(getline(fs_proc_meminfo,line)&&line.find(str,0)==-1)
    {
        std::stringstream stream(line);
        uint64_t val;
        stream.ignore(15,' ');
        stream >>val;
        memstats.push_back(val);
        //cout<<line<<": "<<val<<endl;
    }
    totalMEM = memstats[0];
    freeMEM = memstats[1];
    avaiMEM = memstats[2];
    fs_proc_meminfo.close();
}

void SystemResources::getsysyem_pidstat(uint64_t & utime, uint64_t & stime)
{
    /*
     * [0]->utime
     * [1]->stime
     */
    string name_proc_pid_stat = "/proc/";
    name_proc_pid_stat += myapplicationPID + "/stat";
    fs_proc_pid_stat.open(name_proc_pid_stat.c_str(),ios::in);
    int i=0;
    string line;
    getline(fs_proc_pid_stat,line);
    std::stringstream stream(line);
    while(i!=13)
    {
        stream.ignore(15,' ');
        i++;
    }
    
    stream>>utime;
    
    
    stream>>stime;
    fs_proc_pid_stat.close();
    
}

void SystemResources::getsysyem_pidsched(uint64_t & waitsum)
{
    string name_proc_pid_sched = "/proc/";
    name_proc_pid_sched += myapplicationPID + "/sched";
    fs_proc_pid_sched.open(name_proc_pid_sched.c_str(), ios::in);
    
    
    
    
    fs_proc_pid_sched.close();
}

void SystemResources::getsystem_temperature(uint64_t & temp)
{
    string name_sys_temp = "/sys/class/thermal/thermal_zone0/temp";
    fs_sys_temp.open(name_sys_temp.c_str(),ios::in);
    string line;
    getline(fs_sys_temp,line);
    std::stringstream stream(line);
    stream>>temp;
    fs_sys_temp.close();
}
/*
     
     /proc/stat
     cpu0 60705     883     19312       8034815     10255       0 60 0 0 0
          [user]    [nice]  [system]    [idle]      [iowait]    
     0    1         2       3           4           5     
     
     */



SystemResources::SystemResources(const SystemResources& orig)
{

}

SystemResources::~SystemResources()
{
    fs_proc_cpuinfo.close();
    fs_proc_meminfo.close();
    fs_proc_pid_sched.close();
    fs_proc_pid_stat.close();
    fs_proc_stat.close();
}


