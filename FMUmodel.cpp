/*
 * FMUmodel.cpp
 *
 *  Created on: Feb 13, 2017
 *      Author: bilal
 */

#include "FMUmodel.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#ifdef  _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include "pugixml.hpp"

using namespace std;

FMUmodel::FMUmodel()
{
    // TODO Auto-generated constructor stub

}

FMUmodel::~FMUmodel()
{
    // TODO Auto-generated destructor stub
}

void * FMUmodel::getaddr(const char * nameOfFmiFunction)
{

    void * returnadd;
    string s1 = fmuModelidentifier;
    s1.append(nameOfFmiFunction);
    cout << "sym name: " << s1 << endl;
#ifdef  _WIN32
    returnadd = (void*) GetProcAddress((HINSTANCE) soHandle, s1.c_str());
#else
    returnadd = dlsym(soHandle, s1.c_str());
#endif

    return returnadd;
}

bool FMUmodel::bindfunctionaddresses(void)
{
    bool success = true;

    getTypesPlatform = (fGetTypesPlatform) getaddr("_fmiGetTypesPlatform");
    if (!getTypesPlatform)
    {
        success = false;
        cout << "failed sym import of gettypesplatform" << endl;
    }

    getVersion = (fGetVersion) getaddr("_fmiGetVersion");
    if (!getVersion)
    {
        success = false;
        cout << "failed sym import of getVersion" << endl;
    }

    setDebugLogging = (fSetDebugLogging) getaddr("_fmiSetDebugLogging");
    if (!setDebugLogging)
    {
        success = false;
        cout << "failed sym import of setDebugLogging" << endl;
    }

    setReal = (fSetReal) getaddr("_fmiSetReal");
    if (!setReal)
    {
        success = false;
        cout << "failed sym import of setReal" << endl;
    }

    setInteger = (fSetInteger) getaddr("_fmiSetInteger");
    if (!setInteger)
    {
        success = false;
        cout << "failed sym import of setInteger" << endl;
    }

    setBoolean = (fSetBoolean) getaddr("_fmiSetBoolean");
    if (!setBoolean)
    {
        success = false;
        cout << "failed sym import of setBoolean" << endl;
    }

    setString = (fSetString) getaddr("_fmiSetString");
    if (!setString)
    {
        success = false;
        cout << "failed sym import of setString" << endl;
    }

    getReal = (fGetReal) getaddr("_fmiGetReal");
    if (!getReal)
    {
        success = false;
        cout << "failed sym import of getReal" << endl;
    }

    getInteger = (fGetInteger) getaddr("_fmiGetInteger");
    if (!getInteger)
    {
        success = false;
        cout << "failed sym import of getInteger" << endl;
    }

    getBoolean = (fGetBoolean) getaddr("_fmiGetBoolean");
    if (!getBoolean)
    {
        success = false;
        cout << "failed sym import of getBoolean" << endl;
    }

    getString = (fGetString) getaddr("_fmiGetString");
    if (!getString)
    {
        success = false;
        cout << "failed sym import of getString" << endl;
    }

    instantiateSlave = (fInstantiateSlave) getaddr("_fmiInstantiateSlave");
    if (!instantiateSlave)
    {
        success = false;
        cout << "failed sym import of instantiateSlave" << endl;
    }

    initializeSlave = (fInitializeSlave) getaddr("_fmiInitializeSlave");
    if (!initializeSlave)
    {
        success = false;
        cout << "failed sym import of initializeSlave" << endl;
    }

    terminateSlave = (fTerminateSlave) getaddr("_fmiTerminateSlave");
    if (!terminateSlave)
    {
        success = false;
        cout << "failed sym import of terminateSlave" << endl;
    }

    resetSlave = (fResetSlave) getaddr("_fmiResetSlave");
    if (!resetSlave)
    {
        success = false;
        cout << "failed sym import of resetSlave" << endl;
    }

    freeSlaveInstance = (fFreeSlaveInstance) getaddr("_fmiFreeSlaveInstance");
    if (!freeSlaveInstance)
    {
        success = false;
        cout << "failed sym import of freeSlaveInstance" << endl;
    }

    getRealOutputDerivatives = (fGetRealOutputDerivatives) getaddr(
            "_fmiGetRealOutputDerivatives");
    if (!getRealOutputDerivatives)
    {
        success = false;
        cout << "failed sym import of getRealOutputDerivatives" << endl;
    }

    setRealInputDerivatives = (fSetRealInputDerivatives) getaddr(
            "_fmiSetRealInputDerivatives");
    if (!setRealInputDerivatives)
    {
        success = false;
        cout << "failed sym import of setRealInputDerivatives" << endl;
    }

    doStep = (fDoStep) getaddr("_fmiDoStep");
    if (!doStep)
    {
        success = false;
        cout << "failed sym import of doStep" << endl;
    }

    cancelStep = (fCancelStep) getaddr("_fmiCancelStep");
    if (!cancelStep)
    {
        success = false;
        cout << "failed sym import of cancelStep" << endl;
    }

    getStatus = (fGetStatus) getaddr("_fmiGetStatus");
    if (!getStatus)
    {
        success = false;
        cout << "failed sym import of getStatus" << endl;
    }

    getRealStatus = (fGetRealStatus) getaddr("_fmiGetRealStatus");
    if (!getRealStatus)
    {
        success = false;
        cout << "failed sym import of getRealStatus" << endl;
    }

    getIntegerStatus = (fGetIntegerStatus) getaddr("_fmiGetIntegerStatus");
    if (!getIntegerStatus)
    {
        success = false;
        cout << "failed sym import of getIntegerStatus" << endl;
    }

    getBooleanStatus = (fGetBooleanStatus) getaddr("_fmiGetBooleanStatus");
    if (!getBooleanStatus)
    {
        success = false;
        cout << "failed sym import of getBooleanStatus" << endl;
    }

    getStringStatus = (fGetStringStatus) getaddr("_fmiGetStringStatus");
    if (!getStringStatus)
    {
        success = false;
        cout << "failed sym import of getStringStatus" << endl;
    }

    return success;
}

bool FMUmodel::construct_InputOutput_vectors(const char * xmlpath)
{

    pugi::xml_document doc;
    doc.load_file(xmlpath); //"/home/bilal/xmlfolder/modelDescription.xml");

    unsigned int vr;
    outputVar outvar;
    inputvar invar;
    pugi::xml_node start = doc.child("fmiModelDescription").child(
            "ModelVariables");

    for (pugi::xml_node node_scalvar = start.child("ScalarVariable");
            node_scalvar; node_scalvar = node_scalvar.next_sibling())
    {

        string s1(node_scalvar.attribute("causality").value());
        vr = atoi(node_scalvar.attribute("valueReference").value());

        //std::cout << "scalar variable " << node_scalvar.attribute("name").value()<<endl;
        if ("input" == s1)
        {
            invar.name = node_scalvar.attribute("name").value();
            invar.valueReference = vr;
            inVars.push_back(invar);

        } else if ("output" == s1)
        {
            outvar.name = node_scalvar.attribute("name").value();
            outvar.valueReference = vr;
            outVars.push_back(outvar);
        }

    }

    return true;
}

void FMUmodel::load_FMU_input(int inputNumber, fmiReal value)
{
    setReal(c, &inVars[inputNumber].valueReference, 1, &value);
}

fmiReal FMUmodel::get_fmu_output(int outputNumber)
{
    fmiReal myOutput;
    getReal(c, &outVars[outputNumber].valueReference, 1, &myOutput);
    return myOutput;
}

void FMUmodel::init_FMU_inputs()
{
    fmiReal value = 0;
    //{ 1, 0, 3, 0.10, 0.10, 0, 0 };
    for (unsigned int x = 0; x < inVars.size(); x++)
        setReal(c, &inVars[x].valueReference, 1, &value);
}

/*

void FMUmodel::load_FMU_inputs_inputsBuffers(void)
{

    for (unsigned int x = 0; x < inVars.size(); x++)
    {
        setReal(c, &inVars[x].valueReference, 1, &inVars[x].valBuffer);
    }
}
void FMUmodel::load_FMU_input_inputBuffer(int x)
{
    setReal(c,&inVars[x].valueReference,1,&inVars[x].valBuffer);
}
void FMUmodel::load_FMU_outputBuffers_Outputs(void)
{
    for (unsigned int x = 0; x < outVars.size(); x++)
    {
        getReal(c, &outVars[x].valueReference, 1, &outVars[x].valBuffer);

    }
}
 */
bool FMUmodel::load_FMU_Component(float h, float tend)
{
    const char * mimeType = "application/x-fmu-sharedlibrary";
    const char * id = fmuModelidentifier.c_str();
    const char * guid = fmuGUID.c_str();

    fmiReal timeout = 1000;
    fmiBoolean visible = fmiFalse;
    fmiBoolean interactive = fmiFalse;
    char* fmulocation = NULL;
    callbacks.allocateMemory = calloc;
    callbacks.freeMemory = free;
    //callbacks.stepFinished = NULL;
    callbacks.logger = NULL;
    c = instantiateSlave(id, guid, fmulocation, mimeType, timeout,
            visible, interactive, callbacks, false);


    bool status = initializeSlave(c, 0, fmiFalse, 10);
    return status;
}

void FMUmodel::unload_FMU_Component()
{
    freeSlaveInstance(c);
}

bool FMUmodel::loadFMU_so(const char * soPath)
{
    bool outcome = true;
    cout << "loading .so file" << endl;
#ifdef  _WIN32
    HANDLE h = (void*) LoadLibrary(soPath);
    if (!h)
    {
        cout << ".dll file load error: "<< soPath<<":exiting now" << endl;
        outcome = false;
        return outcome;
    } else
    {
        cout << ".dll load successful" << endl;
        soHandle = h;
    }
     if (!bindfunctionaddresses())
    {
        cout << "function binding failed..exiting exiting now" << endl;
        FreeLibrary((HINSTANCE)soHandle);
        outcome = false;
        return outcome;
    }
#else
    HANDLE h = dlopen(soPath, RTLD_NOW);
    if (!h)
    {
        cout << ".so file load error: " << soPath<< " :exiting now" << endl;
        outcome = false;
        return outcome;
    }
    else
    {
        cout << ".so load successful" << endl;
        soHandle = h;
    }
     if (!bindfunctionaddresses())
    {
        cout << "function binding failed..exiting" << dlerror() << " :exiting now" << endl;
        dlclose(soHandle);
        outcome = false;
        return outcome;
    }
#endif

   

   
    cout << "functions loaded successfully" << endl;
    cout << "version of FMI:" << getVersion() << endl;
    return outcome;
}

bool FMUmodel::loadFMU_xml(const char *xmlPath)
{
    cout << "loading xml" << endl;
    bool success = true;
    pugi::xml_document doc;
    pugi::xml_parse_result xmlloadresult = doc.load_file(xmlPath);
    //"/home/bilal/xmlfolder/modelDescription.xml");

    if (!xmlloadresult)
    {
        cout << "XML file load error " << xmlPath<<":"<<xmlloadresult.description() << endl;
        success = false;
        return success;
    }

    cout << "XML Load result: successfull" << endl;
    construct_InputOutput_vectors(xmlPath);

    pugi::xml_node start = doc.child("fmiModelDescription");

    fmuGUID = start.attribute("guid").value();
    fmuModelidentifier = start.attribute("modelIdentifier").value();
    cout << "\nXML scalar input-variable list:" << endl;
    for (unsigned int x = 0; x < inVars.size(); x++)
    {
        cout << inVars[x].name << "  " << inVars[x].valueReference << endl;
    }
    cout << "\nXML scalar output-variable list:" << endl;
    for (unsigned int x = 0; x < outVars.size(); x++)
    {
        cout << outVars[x].name << "  " << outVars[x].valueReference << endl;
    }



    return success;
}

