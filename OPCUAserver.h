/*
 * OPCUAserver.h
 *
 *  Created on: Feb 13, 2017
 *      Author: bilal
 */

#ifndef OPCUASERVER_H_
#define OPCUASERVER_H_
#include "open62541.h"
#include "FMUmodel.h"
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#ifdef _WIN32
#include "mingw.thread_1.h"
#include "mingw.mutex_1.h"
#else
#include <thread>
#endif

#include "SystemResources.h"

struct simulation_controls {
public:
    bool start_simulation;
    float simulation_time;
    float simulation_step;
    bool status_simulation;
    bool newFMU_loadCount;
    uint8_t FMUs_count;
};

struct DataSource_terminal {
public:
    std::string NodeName;
    double buffer;
    double time;
};
struct FMU_sourcebuffer{
    std::vector<DataSource_terminal> inputs_DataSource;
    std::vector<DataSource_terminal> outputs_DataSource;
};



class OPCUAserver {
private:



    UA_ByteString loadCertificate(void);
    void runServer();
    
    
    
    //for initializing server
    UA_ServerConfig config;
    UA_ServerNetworkLayer nl;
    UA_Server *server;
    UA_Boolean run_server;
    SystemResources * mysystem = new SystemResources();
    
    
    
    //for methods in server space
    static UA_StatusCode startSimMethod_callback(
            void *methodHandle,
            const UA_NodeId objectId,
            size_t inputSize, const UA_Variant *input,
            size_t outputSize, UA_Variant *output);
    

    
    //for data source nodes in server space
    static UA_StatusCode Client_readDataSource_callback(
            void *handle,
            const UA_NodeId nodeId,
            UA_Boolean sourceTimeStamp,
            const UA_NumericRange *range,
            UA_DataValue *value);
    
    

    static UA_StatusCode Client_writeDataSource_callback(
            void *handle,
            const UA_NodeId nodeid,
            const UA_Variant *data,
            const UA_NumericRange *range);

    static UA_StatusCode Client_readSimulationStatus_callback(
            void *handle,
            const UA_NodeId nodeId,
            UA_Boolean sourceTimeStamp,
            const UA_NumericRange *range,
            UA_DataValue *value);
    
    /**************************************************
     * proc fs data callbacks
     **************************************************
     */
    
    static UA_StatusCode Client_read_sysResourc_CPUutilization_callback( //overall utilization
            void *handle,
            const UA_NodeId nodeId,
            UA_Boolean sourceTimeStamp,
            const UA_NumericRange *range,
            UA_DataValue *value);
    
    
    static UA_StatusCode Client_read_systemResource_meminfo_callback(
            void *handle,
            const UA_NodeId nodeId,
            UA_Boolean sourceTimeStamp,
            const UA_NumericRange *range,
            UA_DataValue *value);
    
    static UA_StatusCode Client_read_systemResource_cpuInfo_callback(
            void *handle,
            const UA_NodeId nodeId,
            UA_Boolean sourceTimeStamp,
            const UA_NumericRange *range,
            UA_DataValue *value);

    static UA_StatusCode Client_read_systemResource_APPutiliation_callback( //app utilization
            void *handle,
            const UA_NodeId nodeId,
            UA_Boolean sourceTimeStamp,
            const UA_NumericRange *range,
            UA_DataValue *value);
    

    static UA_StatusCode Client_read_systemResource_CPUavgload_callback( //app utilization
            void *handle,
            const UA_NodeId nodeId,
            UA_Boolean sourceTimeStamp,
            const UA_NumericRange *range,
            UA_DataValue *value);
    
    
    unsigned int i;
public:
    //public class methods and variables
    static OPCUAserver * OngoingInstance;
    simulation_controls sim_controls;
    std::vector<FMU_sourcebuffer> FMUs_sourcebuffer;
    
    void loadInstanceAdd(OPCUAserver*add);
    OPCUAserver();
    void InitializeServer();
    void SetupNode(std::string nodeName,unsigned int fmuNumber, unsigned int terminalNumber, bool IsInput);
    void startServer();
    void stopServer();
    //void setNodeValue(std::string nodeName, fmiReal value);
    void initialize_FMUbuffer(unsigned int inputs,unsigned int outputs);




    virtual ~OPCUAserver();
};



#endif /* OPCUASERVER_H_ */


