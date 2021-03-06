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
#include <unistd.h>
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

    cout << "Starting Embedded Simulator v2.2.5" << endl;
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

#ifdef _WIN32
    
    filelogger = new FileLogger(ExePath());
#else
    filelogger = new FileLogger(getenv("HOME"));
#endif
    
    cout <<"PID from main :  <<"<<::getpid()<<endl;
    while (running)
    {
        if (thisServer->sim_controls.status_simulation == true)
        {
            SimulationEngine();
        } else if (thisServer->sim_controls.start_simulation == true)
        {


            thisServer->sim_controls.start_simulation = false;
            simulation_step = thisServer->sim_controls.simulation_step;
            simulation_step_in_seconds = simulation_step / 1000;
            simulation_end = thisServer->sim_controls.simulation_time;
            simulation_i = 0;
            avgExeTime = 0;
            InitializeFMUs();
            thisServer->sim_controls.status_simulation = true;
            
            t_runstart = high_resolution_clock::now();
           
        }
        else
            std::this_thread::sleep_for(chrono::milliseconds(10)); //reducing idle CPU utilization
        
    }
    //free fmu slave instances

    return 0;
}

void SimulationEngine()
{

    static high_resolution_clock::time_point t1 = high_resolution_clock::now(); //timestamps to calculate step start instance
    static uint32_t i = 0;
    
    t2 = high_resolution_clock::now(); //timestamps to calculate step start instance
    uint32_t delta=duration_cast<microseconds>(t2 - t1).count(); 
    if ( delta>= simulation_step)
    {
        
        //cout<<" duration:"<<duration_cast<microseconds>(t2 - t1).count()<<endl;
        t1 = t2;
        if (simulation_i < simulation_end)
        {   
            //double step = simulation_step / 1000; //converting step in seconds
            SimulationDoStep(simulation_i, simulation_step_in_seconds);
            simulation_i += simulation_step_in_seconds;
            i++;
            t3 = high_resolution_clock::now();
            
            filelogger->printSim_t_exe((duration_cast<microseconds>(t3 - t1).count()),delta);


        } else
        {
            t_runend = high_resolution_clock::now();
            avgExeTime = avgExeTime / i;
            cout << "Runtime:" << duration_cast<milliseconds>(t_runend - t_runstart).count() << "ms" << " || SimSteps:" << i<<endl;
            double temp = duration_cast<milliseconds>(t_runend - t_runstart).count();
            temp /= 1000;
            filelogger->printSimOutputs(myFMUs.size(), simulation_end, temp, simulation_step,i,0);
            thisServer->sim_controls.status_simulation = false;
            simulation_i = 0;
            avgExeTime = 0;
            simulation_step = 0;
            DeinitializeFMUs();
            i = 0;
        }
        
    }
    //if(delta>2)
      //  std::this_thread::sleep_for(chrono::milliseconds(1));
      
}

void SimulationDoStep(float cur_SimTim, float cur_step)
{
    for (unsigned int x = 0; x < myFMUs.size(); x++)
    {
        for (unsigned int y = 0; y < myFMUs[x].get_Invar_size();    y++)
        {
            myFMUs[x].load_FMU_input(y, thisServer->FMUs_sourcebuffer[x].inputs_DataSource[y].buffer);
            thisServer->FMUs_sourcebuffer[x].inputs_DataSource[y].time = cur_SimTim;
        }
        for (unsigned int y = 0; y < myFMUs[x].get_Outvar_size(); y++)
        {
            thisServer->FMUs_sourcebuffer[x].outputs_DataSource[y].buffer = myFMUs[x].get_fmu_output(y);
            thisServer->FMUs_sourcebuffer[x].outputs_DataSource[y].time = cur_SimTim;
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
        string p_xml = getenv("HOME");
        string p_so = getenv("HOME");
        p_xml.append("/modelDescription.xml");
        p_so.append("/linux_fmu_arm2.so");
        fmu.loadFMU_xml(p_xml.c_str());
        fmu.loadFMU_so(p_so.c_str());

#elif defined __i586__
        string p_xml = getenv("HOME");
        string p_so = getenv("HOME");
        p_xml.append("/modelDescription.xml");
        p_so.append("/linux_fmu_quark.so");
        fmu.loadFMU_xml(p_xml.c_str());
        fmu.loadFMU_so(p_so.c_str());
        

#elif defined _WIN32
        string p_xml = ExePath();
        string p_dll =ExePath();
        
        p_xml+="/modelDescription.xml";
        p_dll+="/windows_fmu.dll";
       
        fmu.loadFMU_xml(p_xml.c_str());
        fmu.loadFMU_so(p_dll.c_str());
#else
        string p_xml = getenv("HOME");
        string p_so = getenv("HOME");
        p_xml.append("/xmlfolder/modelDescription.xml");
        p_so.append("/xmlfolder/linux_fmu_x64.so");

        fmu.loadFMU_xml(p_xml.c_str());
        fmu.loadFMU_so(p_so.c_str());
#endif
        thisServer->initialize_FMUbuffer(fmu.get_Invar_size(), fmu.get_Outvar_size());

        for (unsigned int x = 0; x < fmu.get_Invar_size(); x++)
        {
            string s("fmu[");
            s.append(to_string(myFMUs.size()));
            s.append("]_in[");
            s.append(to_string(x));
            s.append("]_");
            s.append(fmu.get_inputTerminal_name(x));
            //fmu.inVars[x].serverNodeName = s;
            thisServer->SetupNode(s, myFMUs.size(), x, true);
            //thisServer->SetupNode(fmu.inVars[x].serverNodeName);
        }

        for (unsigned int x = 0; x < fmu.get_Outvar_size(); x++)
        {
            string s("fmu[");
            s.append(to_string(myFMUs.size()));
            s.append("]_out[");
            s.append(to_string(x));
            s.append("]_");
            s.append(fmu.get_outputTerminal_name(x));
            //fmu.outVars[x].serverNodeName = s;
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


#ifdef _WIN32

string ExePath()
{

    char buffer[50];
    GetModuleFileName(NULL, buffer, 50);
    string::size_type pos = string(buffer).find_last_of("\\/");
    return string(buffer).substr(0, pos);
}
#endif