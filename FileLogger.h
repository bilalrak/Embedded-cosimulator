/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FileLogger.h
 * Author: bilal
 *
 * Created on April 3, 2017, 10:26 AM
 */

#include <fstream>


#ifndef FILELOGGER_H
#define FILELOGGER_H

class FileLogger {
public:
    FileLogger(std::string path);
    FileLogger(const FileLogger& orig);
    virtual ~FileLogger();
    void printSimOutput(unsigned int N_FMUs, float runTime,float simulatedRunTime,float stepSize, float T_exec);
private:
    std::fstream file;
    std::string fullFileName;
    
    
};

#endif /* FILELOGGER_H */

