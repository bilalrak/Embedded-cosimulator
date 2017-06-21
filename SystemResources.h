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




#ifndef SYSTEMRESOURCES_H
#define SYSTEMRESOURCES_H

#include <vector>
#include <fstream>


class SystemResources {
    
    
public:
    uint CPU_count;
    uint16_t sc_CLK_tck;
    
    
    SystemResources();
    SystemResources(const SystemResources& orig);
    //void getSystemResource(uint16_t resourceCode,std::vector<uint32_t> & param);
    void getSystem_stat(uint64_t & ovUtil);
    void getSystem_mem(uint64_t & totalMEM,uint64_t & freeMEM, uint64_t & avaiMEM);
    void getsystem_temperature(uint64_t & temp);
    void getsystem_procUtilization(uint64_t & U);
    void getsystem_loadavg(double & one_min, double & five_min, double & fifteen_min);
    
    
    virtual ~SystemResources();
private:
     std::string myapplicationPID;
     //FILE * fs_proc_stat;
     //FILE * fs_proc_pid_stat;
     //FILE * fs_proc_pid_sched;
     //FILE * fs_proc_cpuinfo;
     //FILE * fs_proc_meminfo;
     std::fstream fs_proc_stat;
     std::fstream fs_proc_meminfo;
     std::fstream fs_sys_temp;
     std::fstream fs_proc_loadavg;
     
   

};

#endif /* SYSTEMRESOURCES_H */

