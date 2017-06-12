/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SystemResources.h
 * Author: bilalrak
 *
 * Created on May 13, 2017, 7:28 PM
 */
//from /proc/stat

#include <vector>
#include <fstream>


#define systemResourseCode_user     0   //total time spent in user mode
#define systemResourseCode_system   1   //total time spent in Kernel mode
#define systemResourseCode_iowait   2   //time waiting for IO to complete
#define systemResourseCode_idle     3   //time spent in idle task

#define systemResourseCode_utime    4   //amount of time the process has been scheduled in user mode
#define systemResourseCode_stime    5   //amount of time the process has been scheduled in kernel mode
#define systemResourceCode_waitSum  6   //amount of time the process was ready but had to wait for CPU

#define systemResource_CPUfreq      7   //cpu frequency

#define systemResourseCode_memtotal 8   //total usable physical ram (long unsigned int - %lu: man page of proc file sys)
#define systemResourseCode_memfree  9   //sum of lowfree+highfree   (%lu)   
#define systemResourseCode_memAvail 10  //estimate of available memory for starting new application (%lu)
                                        //instead i will be using uint32_t type to store every resource parameter.

#define systemStatCode_CPUs         0   //time:user, kernel, iowait,idle
#define systemStatCode_Mem          1   //quantity: total, free, available
#define systemStatCode_pid          2   //time: proc_usermode, proc_kernelmode, proc_waitforCPU


#ifndef SYSTEMRESOURCES_H
#define SYSTEMRESOURCES_H

struct SystemStats
{
public:
    std::string PID;                                            //the process ID of this application
    /* process statistics */
    
    std::vector<long unsigned int> userModeTime_u;              
    std::vector<long unsigned int> kernelModeTime_s;            
    std::vector<long unsigned int> ioWaitTime_io;               
    std::vector<long unsigned int> idleTime_i;                  
    
    //from /proc/[pid]/stat
    std::vector<long unsigned int> procSchedUserMode_pu;        
    std::vector<long unsigned int> procSchedKernelMode_ps;      
    
    //from /proc/[pid]/sched
    std::vector<long unsigned int> procReadyWaitTime_w;         
    
    
    /* CPU statistics */
    //from /proc/cpuinfo (source "book-advanced linux programming")
    std::vector<float> CPU_MHz;
    std::vector<float> temp;
    
    
    
    /* memory statistics */
    //from /proc/meminfo (source "book-advanced linux programming")
    uint32_t memTotal;                                          
    uint32_t memFree;                                           
    uint32_t memAvail;                                          
};


class SystemResources {
    
    
public:
    uint CPU_count;
    uint16_t sc_CLK_tck;
    
    
    SystemResources();
    SystemResources(const SystemResources& orig);
    //void getSystemResource(uint16_t resourceCode,std::vector<uint32_t> & param);
    bool getSystem_stat(unsigned int cpuNum,uint64_t & user,uint64_t & system,uint64_t &idle, uint64_t&iowait);
    void getSystem_mem(uint64_t & totalMEM,uint64_t & freeMEM, uint64_t & avaiMEM);
    void getsysyem_pidstat(uint64_t & utime, uint64_t & stime);
    void getsysyem_pidsched(uint64_t & waitsum);
    void getsystem_temperature(uint64_t & temp);
    
    
    virtual ~SystemResources();
private:
     std::string myapplicationPID;
     //FILE * fs_proc_stat;
     //FILE * fs_proc_pid_stat;
     //FILE * fs_proc_pid_sched;
     //FILE * fs_proc_cpuinfo;
     //FILE * fs_proc_meminfo;
     std::fstream fs_proc_stat;
     std::fstream fs_proc_pid_stat;
     std::fstream fs_proc_pid_sched;
     std::fstream fs_proc_cpuinfo;
     std::fstream fs_proc_meminfo;
     std::fstream fs_sys_temp;
   

};

#endif /* SYSTEMRESOURCES_H */

