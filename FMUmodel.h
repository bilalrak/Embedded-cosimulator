/*
 * FMUmodel.h
 *
 *  Created on: Feb 13, 2017
 *      Author: bilal
 */

#ifndef FMUMODEL_H_
#define FMUMODEL_H_

#include <vector>
#include "myfmi_cs.h"
#include "fmiPlatformTypes.h"
#include <string>


struct IOterminal
{
public:
    std::string name;

    unsigned int valueReference;
    //fmiReal valBuffer;
    //std::string serverNodeName;
};
/*struct IOterminal
{
public:
    std::string name;
    unsigned int valueReference;
    //fmiReal valBuffer;
    //std::string serverNodeName;
};*/


typedef void*  HANDLE;






class FMUmodel
{
public:
	FMUmodel();
	virtual ~FMUmodel();
    

    fmiComponent c;
    fmiCallbackFunctions callbacks;
    std::string  fmuGUID;
    std::string fmuModelidentifier;

    //<functions>
    fGetTypesPlatform getTypesPlatform;
    fGetVersion getVersion;
    fSetDebugLogging setDebugLogging;
    fSetReal setReal;
    fSetInteger setInteger;
    fSetBoolean setBoolean;
    fSetString setString;
    fGetReal getReal;
    fGetInteger getInteger;
    fGetBoolean getBoolean;
    fGetString getString;
    fInstantiateSlave instantiateSlave;
    fInitializeSlave initializeSlave;
    fTerminateSlave terminateSlave;
    fResetSlave resetSlave;
    fFreeSlaveInstance freeSlaveInstance;
    fGetRealOutputDerivatives getRealOutputDerivatives;
    fSetRealInputDerivatives setRealInputDerivatives;
    fDoStep doStep;
    fCancelStep cancelStep;
    fGetStatus getStatus;
    fGetRealStatus getRealStatus;
    fGetIntegerStatus getIntegerStatus;
    fGetBooleanStatus getBooleanStatus;
    fGetStringStatus getStringStatus;
    //<functions/>

    
    void init_FMU_inputs();
    void load_FMU_input(int inputNumber, fmiReal value);
    fmiReal get_fmu_output(int outputNumber);
    //void load_FMU_inputs_inputsBuffers(void);
    //void load_FMU_input_inputBuffer(int x);
    //void load_FMU_outputBuffers_Outputs(void);
    bool loadFMU_xml(const char *xmlPath);
    bool loadFMU_so(const char * soPath);
    bool load_FMU_Component(float h,float tend);
    void unload_FMU_Component();
    unsigned int get_Invar_size();
    unsigned int get_Outvar_size();
    
    std::string get_inputTerminal_name(unsigned int terminalNumber);
    std::string get_outputTerminal_name(unsigned int terminalNumber);


private:
    void * getaddr(const char * nameOfFmiFunction);
    bool bindfunctionaddresses(void); //import symbols from shared object and store them in function pointers of class
    HANDLE soHandle;
    static void fmuLogger(fmiComponent c, fmiString instanceName, fmiStatus status,
               fmiString category, fmiString message, ...);
    bool construct_InputOutput_vectors(const char * xmlpath);

    std::vector<IOterminal> inVars;
    std::vector<IOterminal> outVars;


};

#endif /* FMUMODEL_H_ */
