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
#include "sys/times.h"
#include "sys/vtimes.h"
#include <sys/resource.h>
#include <chrono>
  //for ::getpid() and sysconf(_SC_NPROCESSORS_ONLN);

using namespace std;
using namespace chrono;

SystemResources::SystemResources()
{
    myapplicationPID = to_string(::getpid());
    CPU_count = sysconf(_SC_NPROCESSORS_ONLN);
    sc_CLK_tck = sysconf(_SC_CLK_TCK);
    cout << "number of processor(s):" << CPU_count << " | sc_clk_tck: " << sc_CLK_tck << " | procPID:"<<myapplicationPID<<endl;
}



void SystemResources::getSystem_stat(uint64_t & ovUtil)
{
    
    /*
     * [0]->user
     * [1]->system
     * [2]->idle
     * [3]->iowait
     */
    static uint64_t userLast=0,userniceLast=0,sysLast=0,idleLast=0,iowaitLast=0,irqLast=0,softirqLast=0;
    uint64_t user,usernice,sys,idle,iowait,irq,softirq;
    double percent=0;
    string line;
    string name_proc_stat = "/proc/stat";
    
    fs_proc_stat.open(name_proc_stat.c_str(),ios::in);
    getline(fs_proc_stat,line);
    
        /* 
        1    user – time spent in user mode.
        2    nice – time spent in user mode with low priority.
        3    system – time spent in system mode.
        4    idle – time spent in the idle task.
        5    iowait –  time waiting for I/O to complete.
        6    irq – time servicing hardware interrupts.
        7    softirq – time servicing software interrupts.
        8    steal – time spent in other operating systems when running in a virtualized environment.
        9    guest – time spent running a virtual CPU for guest operating systems.
        10   guest_nice – time spent running a low priority virtual CPU for guest operating systems.
         */
        
            
            std::stringstream stream(line);
            stream.ignore(15,' ');
            stream>>user;               
            stream>>usernice;
            stream>>sys;                
            stream>>idle;                
            stream>>iowait;              
            stream>>irq;
            stream>>softirq;
            
            if(user<userLast || usernice<userniceLast ||sys<sysLast || idle<idleLast || iowait<iowaitLast || irq < irqLast || softirq<softirqLast )
            {
                 //overflow detected..skip this reading
                ovUtil=6000;
                return;
            }
           
            double total;
            total = (user-userLast)+(usernice-userniceLast)+(sys-sysLast)+(irq-irqLast)+(softirq-softirqLast);
            percent = total;
            total +=((idle-idleLast)+(iowait-iowaitLast));
            percent/=total;
            
            percent*=100;
            
            ovUtil=percent;
            
            userLast=user;
            userniceLast=usernice;
            sysLast=sys;
            irqLast=irq;
            softirqLast=softirq;
            idleLast=idle;
            iowaitLast=iowait;
            
            
            fs_proc_stat.close();
            
        
        
    
   
    
    
    
    
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
        string waste;
        std::stringstream stream(line);
        uint64_t val;
        stream >> waste;
        stream >>val;
        memstats.push_back(val);
        //cout<<line<<": "<<val<<endl;
    }
    totalMEM = memstats[0];
    freeMEM = memstats[1];
    avaiMEM = memstats[2];
    fs_proc_meminfo.close();
}

void SystemResources::getsystem_loadavg(double & one_min, double & five_min, double & fifteen_min)
{
    string name_proc_loadavg = "/proc/loadavg";
    fs_proc_loadavg.open(name_proc_loadavg.c_str(),ios::in);
    string line;
    getline(fs_proc_loadavg,line);
    std::stringstream stream(line);
    stream>>one_min;
    stream>>five_min;
    stream>>fifteen_min;
    fs_proc_loadavg.close();
}

void SystemResources::getsystem_procUtilization(uint64_t & U)
{

    static clock_t lastCPU=0,lastCPUuser=0, lastCPUsys=0;
    static chrono::high_resolution_clock::time_point t1 = high_resolution_clock::now();
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
   
    struct tms timeSample;
    double percent;
    clock_t now;
    
    now = times(&timeSample);
   
    
    
    percent = (timeSample.tms_stime+timeSample.tms_cstime-lastCPUsys)+(timeSample.tms_utime+timeSample.tms_cutime-lastCPUuser);
    percent /= (now-lastCPU);
    //percent /= CPU_count;
    percent *= 100;
    lastCPUuser = timeSample.tms_utime+timeSample.tms_cutime;
    lastCPUsys = timeSample.tms_stime+timeSample.tms_cstime;
    lastCPU=now;
    U = percent;
    
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

    fs_proc_meminfo.close();
    fs_proc_stat.close();
}


