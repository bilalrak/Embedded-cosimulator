/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fileReader.cpp
 * Author: bilal
 * 
 * Created on February 22, 2017, 11:36 AM
 */

#include "fileReader.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

fileReader::fileReader()
{
}

unsigned int fileReader::readNumber_FMUstoLoad()
{
    unsigned int count = 0;

    file.open("/home/bilal/simProperties.txt", ios::in);
    if (file.failbit)
        cout << "properties file not found" << endl;







    return count;
}

fileReader::fileReader(const fileReader& orig)
{
}

fileReader::~fileReader()
{
}

