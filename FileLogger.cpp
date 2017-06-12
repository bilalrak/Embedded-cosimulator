/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FileLogger.cpp
 * Author: bilal
 * 
 * Created on April 3, 2017, 10:26 AM
 */

#include "FileLogger.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;


FileLogger::FileLogger(string path)
{
   
    //string myhome = getenv("HOME");
    fullFileName = path.append("/simulation_result.csv");
    
 
    file.open(fullFileName.c_str(),ios::out|ios::trunc);
#ifdef fullLoging 
    /*fullLoging logs the texe in every iteration of simulation
     * else only the Worst Case Execution time is logged 
     */
    file<<"t-exe (us),# FMUs,Runtime (s),simulated Runtime (s),StepSize (ms),iterations"<<endl;
#else
    file<<"WCET (us),# FMUs,Runtime (s),simulated Runtime (s),StepSize (ms),iterations"<<endl;
#endif
    file.close();
    file.open(fullFileName,ios::out|ios::app);
}

FileLogger::FileLogger(const FileLogger& orig)
{
}

void FileLogger::printSimOutputs(unsigned int N_FMUs, double runTime,double simulatedRunTime,double stepSize,uint32_t iterations,double WCET)
{
#ifdef fullLoging
    file<<"--"<<","<<N_FMUs<<","<<runTime<<","<<simulatedRunTime<<","<<stepSize<<","<<iterations<<endl;
#else
    file<<WCET<<","<<N_FMUs<<","<<runTime<<","<<simulatedRunTime<<","<<stepSize<<","<<iterations<<endl;
#endif
    file<<"\n\n\n\n\n"<<endl;
}
void FileLogger::printSim_t_exe(double t_exe)
{
    file<<t_exe<<endl;
}





FileLogger::~FileLogger()
{
    file.close();
}

