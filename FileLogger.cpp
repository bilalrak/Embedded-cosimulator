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
    file<<"# FMUs,Runtime (s),simulated Runtime (s),StepSize (ms),T-exec (ms)"<<endl;
    file.close();
    file.open(fullFileName,ios::out|ios::app);
}

FileLogger::FileLogger(const FileLogger& orig)
{
}

void FileLogger::printSimOutput(unsigned int N_FMUs, float runTime,float simulatedRunTime,float stepSize, float T_exec)
{
    file<<N_FMUs<<","<<runTime<<","<<simulatedRunTime<<","<<stepSize<<","<<T_exec<<endl;
    
}


FileLogger::~FileLogger()
{
    file.close();
}

