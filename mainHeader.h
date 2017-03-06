/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mainHeader.h
 * Author: bilal
 *
 * Created on February 16, 2017, 5:42 PM
 */

#ifndef MAINHEADER_H
#define MAINHEADER_H

#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include "FMUmodel.h"
#include "OPCUAserver.h"
#include <signal.h>
#include <thread>
#include <chrono>








//test functions
void testFunction_fmuRead();
void testFunction_makeFMUlist(unsigned int howMuchTOLoad);



//functions
void SimulationEngine();
void SimulationDoStep(float cur_SimTime,float cur_step);
void ConstructHashTable();
void SimulationTimer();
void RunTimer();
void InitializeFMUs();
void DeinitializeFMUs();

//variables and objects
std::vector<FMUmodel> myFMUs;
OPCUAserver * thisServer;
float simulation_i=0;
float simulation_end=0;
float simulation_step=0;
std::chrono::high_resolution_clock::time_point t_runstart;
std::chrono::high_resolution_clock::time_point t_runend;


#endif /* MAINHEADER_H */
