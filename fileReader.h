/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fileReader.h
 * Author: bilal
 *
 * Created on February 22, 2017, 11:36 AM
 */

#ifndef FILEREADER_H
#define FILEREADER_H
#include <string>
#include <iostream>
#include <fstream>

#define outputFileName "/home/bilal/xmlfolder/Simulation_Result.csv"

class fileReader {
public:
    fileReader();
    unsigned int readNumber_FMUstoLoad();
    
    
    
    
    
    
    
    fileReader(const fileReader& orig);
    virtual ~fileReader();
private:
    std::fstream file;

};

#endif /* FILEREADER_H */

