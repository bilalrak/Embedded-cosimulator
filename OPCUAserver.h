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
#include <thread>
#include <iostream>
#include <mutex>

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

    //for methods in server space
    //UA_Argument inputArguments2;
    //UA_Argument outputArguments2;
    //UA_UInt32 * pInputDimensions;
    //UA_MethodAttributes incAttr;
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
    
    
    
    //bool flag to end server run thread
    

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
    void initializeNode(unsigned int inputs,unsigned int outputs);




    virtual ~OPCUAserver();
};



#endif /* OPCUASERVER_H_ */


