/*
 * OPCUAserver.cpp
 *
 *  Created on: Feb 13, 2017
 *      Author: bilal
 */

#include "OPCUAserver.h"
#include "open62541.h"
#include <errno.h>


using namespace std;
//mutex m;
OPCUAserver * OPCUAserver::OngoingInstance = 0;

void OPCUAserver::loadInstanceAdd(OPCUAserver*add)
{
    OPCUAserver::OngoingInstance = add;
}

/*
 * callback:
 * the callback is registered to a method in address space. the method is used by client to start simulation
 * 
 * function:
 * the method takes 1 input argument as an array of type Uint32
 * [0] stepTime in millisecond
 * [1] runTime in second
 * 
 * these values are loaded into 
 * OPCUAserver::sim_controls.simulation_step
 * OPCUAserver::sim_controls.simulation_time
 * 
 * and OPCUAserver::sim_controls.start_simulation is set to true to notify MAIN to start simulation
 * 
 * Condition:
 * condition to start simulation is OPCUAserver::sim_controls.status_simulation == false
 * 
 * Node Details:
 * UA_NODEID_NUMERIC(1,62541)
 * located in objects folder
 * NodeClass = Method
 * Executable & UserExecutable
 */
UA_StatusCode OPCUAserver::startSimMethod_callback(
        void *methodHandle,
        const UA_NodeId objectId,
        size_t inputSize, const UA_Variant *input,
        size_t outputSize, UA_Variant *output)
{
    UA_String tmp;
    if (OngoingInstance->sim_controls.status_simulation == false)
    {
        tmp = UA_STRING("simulation started");
        OngoingInstance->sim_controls.simulation_step = ((UA_UInt32*) input->data)[0];
        OngoingInstance->sim_controls.simulation_time = ((UA_UInt32*) input->data)[1];
        OngoingInstance->sim_controls.start_simulation = true;
    } else
    {
        tmp = UA_STRING("simulation already running");

    }
    UA_Variant_setScalarCopy(output, &tmp, &UA_TYPES[UA_TYPES_STRING]);
    return UA_STATUSCODE_GOOD;
}

/*
 * callback:
 * This callback is called when client sends a read request for simulation status node. the
 * callback insert the value from data source into the response message
 * 
 * DataSource:
 * OPCUAserver::sim_controls.status_simulation (type=boolean)
 * 
 * Node Details:
 * located in objects folder with UA_NodeId=UA_NODEID_NUMERIC(1,13000)
 * NodeClass = Variable
 * Scalar
 */
UA_StatusCode OPCUAserver::Client_readSimulationStatus_callback(void *handle, const UA_NodeId nodeId, UA_Boolean sourceTimeStamp, const UA_NumericRange *range, UA_DataValue *value)
{
    value->hasValue = true;
    UA_StatusCode retval = UA_Variant_setScalarCopy(&value->value, &OngoingInstance->sim_controls.status_simulation,
            &UA_TYPES[UA_TYPES_BOOLEAN]);
    value->sourceTimestamp = UA_DateTime_now();
    value->hasSourceTimestamp = true;
    return UA_STATUSCODE_GOOD;

}

/*
 * callback:
 * This callback is called when client sends a read request for input/output node. the
 * callback insert the value from data source into the response message
 * 
 * handle:
 * the handle is registered at the time of adding the node into the address space (only with input or output nodes).
 * the input/output nodes are added into the address space by SetupNode function which also registers associated handles.
 * the handle contains following information
 * [0]-FMU number       (type-uint)
 * [1]-terminal number  (type-uint)
 * [2]-inInput          (type-uint)
 * 
 * 
 * DataSource:
 * 
 * for input        OPCUAserver::parentFMUs[FMUnum].inputs_SourceBuffer[terminalNum].buffer  (type double)
 *                  OPCUAserver::parentFMUs[FMUnum].inputs_SourceBuffer[terminalNum].time    (type double)
 * 
 * for output
 *                  OPCUAserver::parentFMUS[<FMUnum>].outputs_SourceBuffer[<terminalNum>].buffer (type double)
 *                  OPCUAserver::parentFMUS[<FMUnum>].outputs_SourceBuffer[<terminalNum>].time   (type double)
 * Node Details:
 * located in objects folder with UA_NodeId=UA_NODEID_STRING(1,"fmu[<FMUnum>]_in/out[<terminalNum>]_PN::...")
 * NodeClass = Variable
 * array
 */
UA_StatusCode OPCUAserver::Client_readDataSource_callback(void *handle, const UA_NodeId nodeId, UA_Boolean sourceTimeStamp, const UA_NumericRange *range, UA_DataValue *value)
{
    cout<<"test -reached here"<<endl;
    value->hasValue = true;
    unsigned int *fmuNum = ((unsigned int*) handle);
    unsigned int *terminalNum = ((unsigned int*) (handle + sizeof (unsigned int)));
    unsigned int *IsInput =((unsigned int*) (handle + sizeof (unsigned int)*2));
    UA_Double mydouble[2];

    if (*IsInput)
    {
        mydouble[0] = OngoingInstance->FMUs_sourcebuffer[*fmuNum].inputs_DataSource[*terminalNum].buffer;
        mydouble[1] = OngoingInstance->FMUs_sourcebuffer[*fmuNum].inputs_DataSource[*terminalNum].time;
    } else
    {
        mydouble[0] = OngoingInstance->FMUs_sourcebuffer[*fmuNum].outputs_DataSource[*terminalNum].buffer;
        mydouble[1] = OngoingInstance->FMUs_sourcebuffer[*fmuNum].outputs_DataSource[*terminalNum].time;
    }
    //UA_StatusCode retval = UA_Variant_setScalarCopy(&value->value, &mydouble,&UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Variant_setArrayCopy(&value->value, &mydouble, 2, &UA_TYPES[UA_TYPES_DOUBLE]);
    value->sourceTimestamp = UA_DateTime_now();
    value->hasSourceTimestamp = true;
    cout<<"test -exiting callback"<<endl;
    return UA_STATUSCODE_GOOD;
    
}

/*
 * callback:
 * This callback is called when client sends a write request for input node. the
 * callback insert the value from request into the data source. this call back is only
 * registered with input nodes as client is only allowed to change inputs and outputs are generated 
 * through the simulation.
 * 
 * handle:
 * the handle is registered at the time of adding the node into the address space (only with input or output nodes).
 * the input/output nodes are added into the address space by SetupNode function which also registers associated handles.
 * the handle contains following information
 * [0]-FMU number       (type-uint)
 * [1]-terminal number  (type-uint)
 * [2]-inInput          (type-uint)
 * 
 * 
 * DataSource:
 * 
 * for input        OPCUAserver::FMUs_sourcebuffer[FMUnum].inputs_SourceBuffer[terminalNum].buffer  (type double)
 *                  OPCUAserver::FMUs_sourcebuffer[FMUnum].inputs_SourceBuffer[terminalNum].time    (type double)
 * 
 * Node Details:
 * located in objects folder with UA_NodeId=UA_NODEID_STRING(1,"fmu[<FMUnum>]_in[<terminalNum>]_PN::...")
 * NodeClass = Variable
 * array
 */
UA_StatusCode OPCUAserver::Client_writeDataSource_callback(void *handle, const UA_NodeId nodeid, const UA_Variant *data, const UA_NumericRange *range)
{
    //if (range)
    //    return UA_STATUSCODE_BADINDEXRANGEINVALID;
    unsigned int *fmuNum = ((unsigned int*) handle);
    unsigned int *inputNum = ((unsigned int*) (handle + sizeof (unsigned int)));
    OngoingInstance->FMUs_sourcebuffer[*fmuNum].inputs_DataSource[*inputNum].buffer = *(UA_Double*) data->data;
    return UA_STATUSCODE_GOOD;
}

OPCUAserver::OPCUAserver()
{
    // TODO Auto-generated constructor stub

}

void OPCUAserver::InitializeServer()
{

    config = UA_ServerConfig_standard;
    nl = UA_ServerNetworkLayerTCP(UA_ConnectionConfig_standard, 16664);
    config.networkLayers = &nl;
    config.networkLayersSize = 1;
    server = UA_Server_new(config);
    run_server = true;
    sim_controls.simulation_step = 0;
    sim_controls.simulation_time = 0;
    sim_controls.start_simulation = false;
    sim_controls.status_simulation = false;
    //SetupNode("Step_size");
    //SetupNode("Run_time");


    //add method calls in the server space
    UA_Argument inputArguments;
    UA_Argument_init(&inputArguments);
    //inputArguments.arrayDimensionsSize = 0;
    inputArguments.arrayDimensionsSize = 1;
    UA_UInt32 * pOutputDimensions = UA_UInt32_new();
    pOutputDimensions[0] = 2;
    inputArguments.arrayDimensions = pOutputDimensions;
    //inputArguments.arrayDimensions = NULL;
    inputArguments.dataType = UA_TYPES[UA_TYPES_UINT32].typeId;
    inputArguments.description = UA_LOCALIZEDTEXT("en_US", "[0]-steptime (ms)\n[1]-runtime (s)");
    inputArguments.name = UA_STRING("Simulation parameters");
    inputArguments.valueRank = 1;

    UA_Argument outputArguments;
    UA_Argument_init(&outputArguments);
    outputArguments.arrayDimensionsSize = 0;
    outputArguments.arrayDimensions = NULL;
    outputArguments.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    outputArguments.description = UA_LOCALIZEDTEXT("en_US", "reply message from server");
    outputArguments.name = UA_STRING("reply");
    outputArguments.valueRank = -1;

    UA_MethodAttributes addmethodattributes;
    UA_MethodAttributes_init(&addmethodattributes);
    addmethodattributes.description = UA_LOCALIZEDTEXT("en_US", "Starts FMI simulation on the server");
    addmethodattributes.displayName = UA_LOCALIZEDTEXT("en_US", "StartSim");
    addmethodattributes.executable = true;
    addmethodattributes.userExecutable = true;
    UA_Server_addMethodNode(server, UA_NODEID_NUMERIC(1, 62541),
            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
            UA_QUALIFIEDNAME(1, "StartSim"), addmethodattributes,
            &startSimMethod_callback, // Call this method
            (void *) server, // Pass our server pointer as a handle to the method
            1, &inputArguments, 1, &outputArguments, NULL);


    UA_DataSource DataSource;
    DataSource = (UA_DataSource){.handle = NULL, .read = Client_readSimulationStatus_callback, .write = NULL};
    string nodeName = "simulationStatus";
    UA_VariableAttributes v_attr;
    UA_VariableAttributes_init(&v_attr);
    v_attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", nodeName.c_str());
    v_attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", nodeName.c_str());
    UA_Double myFloatnode = 0;
    UA_Variant_setScalarCopy(&v_attr.value, &myFloatnode, &UA_TYPES[UA_TYPES_DOUBLE]);
    v_attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    const UA_QualifiedName qualifiedn = UA_QUALIFIEDNAME(1, "simulationStatus");

    UA_Server_addDataSourceVariableNode(server,
            UA_NODEID_NUMERIC(1, 13000), //1
            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), //2
            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), //3
            qualifiedn, //4
            UA_NODEID_NULL, //5
            v_attr, //6
            DataSource, //7
            NULL); //8


}

UA_ByteString OPCUAserver::loadCertificate(void)
{

}

void OPCUAserver::SetupNode(std::string nodeName, unsigned int fmuNumber, unsigned int terminalNumber, bool IsInput)
{
    UA_DataSource DataSource;


    if (IsInput)
    {
        this->FMUs_sourcebuffer[fmuNumber].inputs_DataSource[terminalNumber].NodeName = nodeName;
        DataSource = (UA_DataSource){.handle = NULL, .read = Client_readDataSource_callback, .write = Client_writeDataSource_callback};

    } else
    {
        this->FMUs_sourcebuffer[fmuNumber].outputs_DataSource[terminalNumber].NodeName = nodeName;
        DataSource = (UA_DataSource){.handle = NULL, .read = Client_readDataSource_callback, .write = NULL};
    }
    unsigned int *h = new unsigned int[3];
    h[0] = fmuNumber;
    h[1] = terminalNumber;
    h[2] = (unsigned int) IsInput;
    DataSource.handle = (void*) h;

    UA_VariableAttributes v_attr;
    UA_VariableAttributes_init(&v_attr);
    v_attr.description = UA_LOCALIZEDTEXT_ALLOC("en_US", nodeName.c_str());
    v_attr.displayName = UA_LOCALIZEDTEXT_ALLOC("en_US", nodeName.c_str());
    UA_Double myFloatnode[2];
    myFloatnode[1] = 2;
    myFloatnode[2] = 3;
    //UA_Variant_setScalarCopy(&v_attr.value, &myFloatnode, &UA_TYPES[UA_TYPES_DOUBLE]);
    //A_Variant_setArray(&v_attr.value, UA_Array_new(2, &UA_TYPES[UA_TYPES_DOUBLE]),2, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Variant_setArrayCopy(&v_attr.value, &myFloatnode, 2, &UA_TYPES[UA_TYPES_DOUBLE]);

    v_attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    const UA_QualifiedName qualifiedn = UA_QUALIFIEDNAME(1, "datasourceNode");

    UA_Server_addDataSourceVariableNode(server,
            UA_NODEID_STRING_ALLOC(1, nodeName.c_str()), //1
            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), //2
            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES), //3
            qualifiedn, //4
            UA_NODEID_NULL, //5
            v_attr, //6
            DataSource, //7
            NULL); //8

    UA_Variant_deleteMembers(&v_attr.value);

}

void OPCUAserver::runServer()
{
    UA_StatusCode retval = UA_Server_run(server, &run_server);
}

void OPCUAserver::startServer()
{
    thread mythread(&OPCUAserver::runServer, this);
    //runServer();
    mythread.detach();
}

void OPCUAserver::stopServer()
{
    run_server = false;
}

void OPCUAserver::initialize_FMUbuffer(unsigned int inputs, unsigned int outputs)
{
    FMU_sourcebuffer newholder;

    for (unsigned int x = 0; x < inputs; x++)
    {
        DataSource_terminal inputbuff;
        inputbuff.buffer = 0;
        inputbuff.time = 0;
        newholder.inputs_DataSource.push_back(inputbuff);
    }
    for (unsigned int x = 0; x < outputs; x++)
    {
        DataSource_terminal outputbuff;
        outputbuff.buffer = 0;
        outputbuff.time = 0;
        newholder.outputs_DataSource.push_back(outputbuff);
    }
    FMUs_sourcebuffer.push_back(newholder);

}

/*void OPCUAserver::setNodeValue(std::string nodeName, fmiReal value)
{
    string s = nodeName;
    char * nodeid = new char[s.size() + 1];
    copy(s.begin(), s.end(), nodeid);
    nodeid[s.size()] = '\0';
    UA_Variant v;
    UA_Variant_init(&v);
    UA_Variant_setScalar(&v, &value, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Server_writeValue(server, UA_NODEID_STRING(1, nodeid), v);
    cout << "setnodevalue: " << UA_NODEID_STRING(1, nodeid).identifier.string.data << " | value: " << value << endl;
    //delete[] nodeid;

}*/

OPCUAserver::~OPCUAserver()
{
    // TODO Auto-generated destructor stub
}

