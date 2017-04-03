/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: bilal
 *
 * Created on February 16, 2017, 3:42 PM
 */

#include <cstdlib>
#include "mainHeader.h"
#include "fileReader.h"
using namespace std;
using namespace chrono;







UA_Boolean running = true;

static void stopHandler(int sign)
{
    thisServer->stopServer(); //Handler to receive ctrl-c
    running = false;
}

int main(int argc, char** argv)
{
    
    cout << "Starting Embedded Simulator" << endl;
    signal(SIGINT, stopHandler);
    thisServer = new OPCUAserver();
    thisServer->loadInstanceAdd(thisServer);
    thisServer->InitializeServer(); //initialize OPCUA server 
    cout << "\n\nenter the number of FMUs to load> ";
    string s;
    cin>>s;



    testFunction_makeFMUlist(atoi(s.c_str())); //load FMU(s) into vector for simulation 
    cout << "loaded " << myFMUs.size() << " FMU(s)" << endl;
    thisServer->startServer(); //start OPCUA server process
    
    filelogger =new FileLogger();


    while (running)
    {
        if (thisServer->sim_controls.status_simulation == true)
        {
            SimulationEngine();
        } else if (thisServer->sim_controls.start_simulation == true)
        {


            thisServer->sim_controls.start_simulation = false;
            simulation_step = thisServer->sim_controls.simulation_step;
            simulation_end = thisServer->sim_controls.simulation_time;
            simulation_i = 0;
            avgExeTime=0;
            InitializeFMUs();
            thisServer->sim_controls.status_simulation = true;
            t_runstart = high_resolution_clock::now();
            

        }






    }
    //free fmu slave instances

    return 0;
}



void SimulationEngine()
{

    static high_resolution_clock::time_point t1 = high_resolution_clock::now(); //timestamps to calculate step start instance
    static unsigned int i = 0;
     t2 = high_resolution_clock::now();//timestamps to calculate step start instance
    if (duration_cast<milliseconds>(t2 - t1).count() >= simulation_step)
    {
        //cout<<"st:"<<simulation_step<<" duration:"<<duration_cast<milliseconds>(t2 - t1).count()<<endl;
        t1 = t2;
        if (simulation_i < simulation_end)
        {
            float step = simulation_step / 1000;
            SimulationDoStep(simulation_i, step);
            simulation_i += step;
            i++;
            t3 = high_resolution_clock::now();
            
            avgExeTime+= (duration_cast<microseconds>(t3 - t1).count());
            //cout<<ttt<<endl;
        } else
        {
            t_runend = high_resolution_clock::now();
            avgExeTime=avgExeTime/i;
            cout << "Runtime:" << duration_cast<milliseconds>(t_runend - t_runstart).count() << "ms" << " || SimSteps:" << i << " || avg Exe Time:"<<(avgExeTime)<<"us"<<endl;
            filelogger->printSimOutput(myFMUs.size(),simulation_end,(duration_cast<milliseconds>(t_runend - t_runstart).count())/1000,simulation_step,avgExeTime/1000);
            thisServer->sim_controls.status_simulation = false;
            simulation_i = 0;
            avgExeTime=0;
            simulation_step = 0;
            DeinitializeFMUs();
            
            i = 0;

        }
    }





}

void SimulationDoStep(float cur_SimTim, float cur_step)
{
    for (unsigned int x = 0; x < myFMUs.size(); x++)
    {
        for (unsigned int y = 0; y < myFMUs[x].inVars.size(); y++)
        {
            myFMUs[x].load_FMU_input(y, thisServer->parentFMUs[x].inputs_SourceBuffer[y].buffer);
            thisServer->parentFMUs[x].inputs_SourceBuffer[y].time = cur_SimTim;
        }
        for (unsigned int y = 0; y < myFMUs[x].outVars.size(); y++)
        {
            thisServer->parentFMUs[x].outputs_SourceBuffer[y].buffer = myFMUs[x].get_fmu_output(y);
            thisServer->parentFMUs[x].outputs_SourceBuffer[y].time = cur_SimTim;
        }
        myFMUs[x].doStep(myFMUs[x].c, cur_SimTim, cur_step, fmiTrue);

    }
}

void testFunction_makeFMUlist(unsigned int howMuchTOLoad)
{
    for (unsigned int count = 0; count < howMuchTOLoad; count++)
    {
        FMUmodel fmu;

#ifdef __arm__
        //fmu.loadFMU_xml("/home/orangepi/modelDescription.xml");
        //fmu.loadFMU_so("/home/orangepi/linux_fmu_arm2.so");
        string p_xml = getenv("HOME");
        string p_so = getenv("HOME");
        p_xml.append("/modelDescription.xml");
        p_so.append("/linux_fmu_arm2.so");
        fmu.loadFMU_xml(p_xml.c_str());
        fmu.loadFMU_so(p_so.c_str());
#else
        string p_xml = getenv("HOME");
        string p_so = getenv("HOME");
        p_xml.append("/xmlfolder/modelDescription.xml");
        p_so.append("/xmlfolder/linux_fmu.so");

        fmu.loadFMU_xml(p_xml.c_str());
        fmu.loadFMU_so(p_so.c_str());
#endif
        thisServer->initializeNode(fmu.inVars.size(), fmu.outVars.size());

        for (unsigned int x = 0; x < fmu.inVars.size(); x++)
        {
            string s("fmu[");
            s.append(to_string(myFMUs.size()));
            s.append("]_in[");
            s.append(to_string(x));
            s.append("]_");
            s.append(fmu.inVars[x].name);
            fmu.inVars[x].serverNodeName = s;
            thisServer->SetupNode(s, myFMUs.size(), x, true);
            //thisServer->SetupNode(fmu.inVars[x].serverNodeName);
        }

        for (unsigned int x = 0; x < fmu.outVars.size(); x++)
        {
            string s("fmu[");
            s.append(to_string(myFMUs.size()));
            s.append("]_out[");
            s.append(to_string(x));
            s.append("]_");
            s.append(fmu.outVars[x].name);
            fmu.outVars[x].serverNodeName = s;
            thisServer->SetupNode(s, myFMUs.size(), x, false);
            //thisServer->SetupNode(fmu.outVars[x].serverNodeName);
        }

        myFMUs.push_back(fmu);

    }
}

void InitializeFMUs()
{
    for (unsigned int x = 0; x < myFMUs.size(); x++)
    {
        float step = simulation_step / 1000;
        myFMUs[x].init_FMU_inputs();
        myFMUs[x].load_FMU_Component(step, simulation_end);

    }
}

void DeinitializeFMUs()
{
    for (unsigned int x = 0; x < myFMUs.size(); x++)
        myFMUs[x].unload_FMU_Component();
}