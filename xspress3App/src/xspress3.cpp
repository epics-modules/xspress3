#include <iostream>
#include <string>
#include <stdexcept>

//Epics headers
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsExport.h>
#include <epicsString.h>
#include <iocsh.h>
#include <drvSup.h>
#include <registryFunction.h>


//Xspress3 SDK header
extern "C" {
  //#include "xspress3sdk.h"
}

#include "xspress3.h"

using std::cout;
using std::endl;

//Definitions of static class data members
const epicsUInt32 Xspress3::logFlow_ = 1;
const epicsUInt32 Xspress3::logError_ = 2;

//C Function prototypes to tie in with EPICS
static void xsp3StatusTaskC(void *drvPvt);
static void xsp3DataTaskC(void *drvPvt);


/**
 * Constructor for Xspress3::Xspress3. 
 * This must be called in the Epics IOC startup file.
 * @param portName The Asyn port name to use
 * @param maxChannels The number of channels to use (eg. 8)
 */
Xspress3::Xspress3(const char *portName, int numChannels, int maxBuffers, size_t maxMemory)
  : asynNDArrayDriver(portName,
		      numChannels, /* maxAddr - channels use different param lists*/ 
		      NUM_DRIVER_PARAMS,
		      maxBuffers,
		      maxMemory,
		      asynInt32Mask | asynInt32ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynFloat64ArrayMask | asynDrvUserMask | asynOctetMask, /* Interface mask */
		      asynInt32Mask | asynInt32ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynFloat64ArrayMask | asynOctetMask,  /* Interrupt mask */
		      ASYN_CANBLOCK | ASYN_MULTIDEVICE, /* asynFlags.*/
		      1, /* Autoconnect */
		      0, /* Default priority */
		      0), /* Default stack size*/
  numChannels_(numChannels)
{
  int status = asynSuccess;
  const char *functionName = "Xspress3::Xspress3";

  debug_ = 1;

  log(logFlow_, "Start of constructor", functionName); 

  //Create the epicsEvent for signaling to the status task when parameters should have changed.
  //This will cause it to do a poll immediately, rather than wait for the poll time period.
  statusEvent_ = epicsEventMustCreate(epicsEventEmpty);
  if (!statusEvent_) {
    printf("%s:%s epicsEventCreate failure for status event\n", driverName, functionName);
    return;
  }
  startEvent_ = epicsEventMustCreate(epicsEventEmpty);
  if (!startEvent_) {
    printf("%s:%s epicsEventCreate failure for start event\n", driverName, functionName);
    return;
  }
  stopEvent_ = epicsEventMustCreate(epicsEventEmpty);
  if (!stopEvent_) {
    printf("%s:%s epicsEventCreate failure for stop event\n", driverName, functionName);
    return;
  }

  //Add the params to the paramLib 
  //createParam adds the parameters to all param lists automatically (using maxAddr).
  createParam(xsp3ResetParamString,         asynParamInt32,       &xsp3ResetParam);
  createParam(xsp3EraseParamString,         asynParamInt32,       &xsp3EraseParam);
  createParam(xsp3StartParamString,         asynParamInt32,       &xsp3StartParam);
  createParam(xsp3StopParamString,          asynParamInt32,       &xsp3StopParam);
  createParam(xsp3StatusParamString,        asynParamOctet,       &xsp3StatusParam);
  createParam(xsp3NumChannelsParamString,   asynParamInt32,       &xsp3NumChannelsParam);
  createParam(xsp3MaxNumChannelsParamString,asynParamInt32,       &xsp3MaxNumChannelsParam);
  createParam(xsp3TriggerModeParamString,   asynParamInt32,       &xsp3TriggerModeParam);
  createParam(xsp3NumFramesParamString,     asynParamInt32,       &xsp3NumFramesParam);
  //These params will use different param lists based on asyn address
  createParam(xsp3ChanMcaParamString,       asynParamInt32Array,  &xsp3ChanMcaParam);
  createParam(xsp3ChanMcaCorrParamString,   asynParamFloat64Array,&xsp3ChanMcaCorrParam);
  createParam(xsp3ChanSca1ParamString,      asynParamInt32,       &xsp3ChanSca1Param);
  createParam(xsp3ChanSca2ParamString,      asynParamInt32,       &xsp3ChanSca2Param);
  createParam(xsp3ChanSca3ParamString,      asynParamInt32,       &xsp3ChanSca3Param);
  createParam(xsp3ChanSca4ParamString,      asynParamInt32,       &xsp3ChanSca4Param);
  createParam(xsp3ChanSca1CorrParamString,  asynParamFloat64,     &xsp3ChanSca1CorrParam);
  createParam(xsp3ChanSca2CorrParamString,  asynParamFloat64,     &xsp3ChanSca2CorrParam);
  createParam(xsp3ChanSca3CorrParamString,  asynParamFloat64,     &xsp3ChanSca3CorrParam);
  createParam(xsp3ChanSca4CorrParamString,  asynParamFloat64,     &xsp3ChanSca4CorrParam);
  createParam(xsp3ChanSca1ArrayParamString,asynParamInt32Array,  &xsp3ChanSca1ArrayParam);
  createParam(xsp3ChanSca2ArrayParamString,asynParamInt32Array,  &xsp3ChanSca2ArrayParam);
  createParam(xsp3ChanSca3ArrayParamString,asynParamInt32Array,  &xsp3ChanSca3ArrayParam);
  createParam(xsp3ChanSca4ArrayParamString,asynParamInt32Array,  &xsp3ChanSca4ArrayParam);
  createParam(xsp3ChanSca1CorrArrayParamString,asynParamFloat64Array,  &xsp3ChanSca1CorrArrayParam);
  createParam(xsp3ChanSca2CorrArrayParamString,asynParamFloat64Array,  &xsp3ChanSca2CorrArrayParam);
  createParam(xsp3ChanSca3CorrArrayParamString,asynParamFloat64Array,  &xsp3ChanSca3CorrArrayParam);
  createParam(xsp3ChanSca4CorrArrayParamString,asynParamFloat64Array,  &xsp3ChanSca4CorrArrayParam);
  createParam(xsp3ChanSca1HlmParamString,   asynParamInt32,       &xsp3ChanSca1HlmParam);
  createParam(xsp3ChanSca2HlmParamString,   asynParamInt32,       &xsp3ChanSca2HlmParam);
  createParam(xsp3ChanSca3HlmParamString,   asynParamInt32,       &xsp3ChanSca3HlmParam);
  createParam(xsp3ChanSca4HlmParamString,   asynParamInt32,       &xsp3ChanSca4HlmParam);
  createParam(xsp3ChanSca1LlmParamString,   asynParamInt32,       &xsp3ChanSca1LlmParam);
  createParam(xsp3ChanSca2LlmParamString,   asynParamInt32,       &xsp3ChanSca2LlmParam);
  createParam(xsp3ChanSca3LlmParamString,   asynParamInt32,       &xsp3ChanSca3LlmParam);
  createParam(xsp3ChanSca4LlmParamString,   asynParamInt32,       &xsp3ChanSca4LlmParam);
  createParam(xsp3ChanTotalParamString,     asynParamInt32,       &xsp3ChanTotalParam);
  createParam(xsp3ChanTotalCorrParamString, asynParamFloat64,     &xsp3ChanTotalCorrParam);
  createParam(xsp3ChanTotalArrayParamString,asynParamInt32Array,  &xsp3ChanTotalArrayParam);
  createParam(xsp3ChanTotalCorrArrayParamString,asynParamFloat64Array,  &xsp3ChanTotalCorrArrayParam);
  
  

 //Initialize non static data members
  acquiring_ = 0;

  pollingPeriod_ = 0.5; //seconds
  fastPollingPeriod_ = 0.1; //seconds

  /* Create the thread that updates the Xspress3 status */
  status = (epicsThreadCreate("Xsp3StatusTask",
                              epicsThreadPriorityMedium,
                              epicsThreadGetStackSize(epicsThreadStackMedium),
                              (EPICSTHREADFUNC)xsp3StatusTaskC,
                              this) == NULL);
  if (status) {
    printf("%s:%s epicsThreadCreate failure for status task\n",driverName, functionName);    
    return;
  }


    /* Create the thread that readouts the data */
  status = (epicsThreadCreate("GeDataTask",
                              epicsThreadPriorityMedium,
                              epicsThreadGetStackSize(epicsThreadStackMedium),
                              (EPICSTHREADFUNC)xsp3DataTaskC,
                              this) == NULL);
  if (status) {
    printf("%s:%s epicsThreadCreate failure for data task\n",
           driverName, functionName);
    return;
  }

  running_ = 1;

  //Read the max number of channels supported by the system here
  

  //Set any paramLib parameters that need passing up to device support
  setIntegerParam(xsp3NumChannelsParam, numChannels_);
  setIntegerParam(xsp3MaxNumChannelsParam, numChannels_); //Set this to what the system reports, not what we pass in
  setIntegerParam(xsp3TriggerModeParam, 0);
  setIntegerParam(xsp3NumFramesParam, 0);
  for (int chan=0; chan<numChannels_; chan++) {
    setIntegerParam(chan, xsp3ChanSca1Param, 0);
    setIntegerParam(chan, xsp3ChanSca2Param, 0);
    setIntegerParam(chan, xsp3ChanSca3Param, 0);
    setIntegerParam(chan, xsp3ChanSca4Param, 0);
    setDoubleParam(chan,  xsp3ChanSca1CorrParam, 0);
    setDoubleParam(chan,  xsp3ChanSca2CorrParam, 0);
    setDoubleParam(chan,  xsp3ChanSca3CorrParam, 0);
    setDoubleParam(chan,  xsp3ChanSca4CorrParam, 0);
    setIntegerParam(chan, xsp3ChanSca1HlmParam, 0);
    setIntegerParam(chan, xsp3ChanSca2HlmParam, 0);
    setIntegerParam(chan, xsp3ChanSca3HlmParam, 0);
    setIntegerParam(chan, xsp3ChanSca4HlmParam, 0);
    setIntegerParam(chan, xsp3ChanSca1LlmParam, 0);
    setIntegerParam(chan, xsp3ChanSca2LlmParam, 0);
    setIntegerParam(chan, xsp3ChanSca3LlmParam, 0);
    setIntegerParam(chan, xsp3ChanSca4LlmParam, 0);
    setIntegerParam(chan, xsp3ChanTotalParam, 0);
    setDoubleParam(chan,  xsp3ChanTotalCorrParam, 0);
  }

  //Do we need to do a status read now?
  //statusRead();

  setStringParam(xsp3StatusParam, "System Init Complete");

  callParamCallbacks();

  log(logFlow_, "End of constructor", functionName); 

}

Xspress3::~Xspress3() 
{
  cout << "Destructor called." << endl;
}


/**
 * Wrapper for asynPrint and local debug prints. If the debug_ data member
 * is on, then it simply prints the message. Otherwise if uses asynPrint statements.
 * @param mask Use either logFlow_ or logError_
 * @param msg The message to print
 * @param function The name of the calling function
 */
void Xspress3::log(epicsUInt32 mask, const char *msg, const char *function)
{
  if (debug_ == 1) {
    cout << function << " " << msg << endl;
  } else {
    if (mask == logFlow_) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s %s.\n", function, msg);
    } else {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s %s.\n", function, msg);
    }
  }
}



/**
 * Reimplementing this function from asynNDArrayDriver to deal with integer values.
 */ 
asynStatus Xspress3::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
  int function = pasynUser->reason;
  int addr = 0;
  asynStatus status = asynSuccess;
  const char *functionName = "Xspress3::writeInt32";
  
  log(logFlow_, "Calling writeInt32", functionName);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s asynUser->reason: &d.\n", functionName, function);

  //Read address (channel number).
  status = getAddress(pasynUser, &addr); 
  if (status!=asynSuccess) {
    return(status);
  }

  cout << functionName << " asyn address: " << addr << endl;
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Asyn addr: &d.\n", functionName, addr);

  if (function == xsp3ResetParam) {
    log(logFlow_, "System reset", functionName);
  } 
  else if (function == xsp3EraseParam) {
    log(logFlow_, "Erase MCA data, clear SCAs", functionName);
  } 
  else if (function == xsp3StartParam) {
    log(logFlow_, "Start collecting data", functionName);
    epicsEventSignal(this->startEvent_);
  } 
  else if (function == xsp3StopParam) {
    log(logFlow_, "Stop collecting data", functionName);
    epicsEventSignal(this->stopEvent_);
  } 
  else if (function == xsp3TriggerModeParam) {
   log(logFlow_, "Set trigger mode", functionName);
  } 
  else if (function == xsp3NumFramesParam) {
    log(logFlow_, "Set the number of frames", functionName);
  } 
  else if (function == xsp3ChanSca1HlmParam) {
    log(logFlow_, "Setting SCA1 high limit.", functionName);
  } 
  else if (function == xsp3ChanSca2HlmParam) {
    log(logFlow_, "Setting SCA2 high limit", functionName);
  } 
  else if (function == xsp3ChanSca3HlmParam) {
    log(logFlow_, "Setting SCA3 high limit", functionName);
  } 
  else if (function == xsp3ChanSca4HlmParam) {
    log(logFlow_, "Setting SCA4 high limit", functionName);
  } 
  else if (function == xsp3ChanSca1LlmParam) {
    log(logFlow_, "Setting SCA1 low limit", functionName);
  } 
  else if (function == xsp3ChanSca2LlmParam) {
    log(logFlow_, "Setting SCA2 low limit", functionName); 
  } 
  else if (function == xsp3ChanSca3LlmParam) {
    log(logFlow_, "Setting SCA3 low limit", functionName);
  }
  else if (function == xsp3ChanSca4LlmParam) {
    log(logFlow_, "Setting SCA4 low limit", functionName);
  } else {
    log(logError_, "No matching parameter.", functionName);
  }

  //Set in param lib so the user sees a readback straight away. We might overwrite this in the 
  //status task, depending on the parameter.
  status = (asynStatus) setIntegerParam(addr, function, value);
  if (status!=asynSuccess) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, 
	      "%s Error setting parameter. Asyn addr: &d, asynUser->reason: &d.\n", 
	      functionName, addr, function);
    return(status);
  }

  //Do callbacks so higher layers see any changes 
  callParamCallbacks(addr);

  return status;
}

/**
 * Reimplementing this function from asynNDArrayDriver to deal with floating point values.
 */ 
asynStatus Xspress3::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
  int function = pasynUser->reason;
  int addr = 0;
  asynStatus status = asynSuccess;
  const char *functionName = "Xspress3::writeFloat64";
  
  log(logFlow_, "Calling writeFloat64", functionName);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s asynUser->reason: &d.\n", functionName, function);

  //Read address (channel number).
  status = getAddress(pasynUser, &addr); 
  if (status!=asynSuccess) {
    return(status);
  }

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Asyn addr: &d.\n", functionName, addr);


  //Set in param lib so the user sees a readback straight away. We might overwrite this in the 
  //status task, depending on the parameter.
  status = (asynStatus) setDoubleParam(function, value);

  //Do callbacks so higher layers see any changes 
  callParamCallbacks();

  return status;
}

/**
 * Status poling task
 */
void Xspress3::statusTask(void)
{
  asynStatus status = asynSuccess;
  epicsEventWaitStatus eventStatus;
  float timeout = pollingPeriod_;

  const char* functionName = "Xspress3::statusTask";

  log(logFlow_, "Started.", functionName);

   while (1) {
     eventStatus = epicsEventWaitWithTimeout(statusEvent_, timeout);          
     if (eventStatus == (epicsEventWaitOK || epicsEventWaitTimeout)) {
       if (debug_) {
	 //log(logFlow_, "Got status event.", functionName);
       }
       
     }
   }
     
 

}

/**
 * Data readout task.
 * Calculate statistics and post waveforms.
 */
void Xspress3::dataTask(void)
{
  asynStatus status = asynSuccess;
  epicsEventWaitStatus eventStatus;
  epicsFloat64 timeout = 0.0;
  const char* functionName = "Xspress3::dataTask";

  log(logFlow_, "Started.", functionName);

   while (1) {

     eventStatus = epicsEventWait(startEvent_);          
     if (eventStatus == epicsEventWaitOK) {
        log(logFlow_, "Got start event.", functionName);
      }


      eventStatus = epicsEventWaitWithTimeout(stopEvent_, timeout);          
      if (eventStatus == epicsEventWaitOK) {
        log(logFlow_, "Got stop event.", functionName);
      }

   }

}


//Global C utility functions to tie in with EPICS

static void xsp3StatusTaskC(void *drvPvt)
{
  Xspress3 *pPvt = (Xspress3 *)drvPvt;

  pPvt->statusTask();
}


static void xsp3DataTaskC(void *drvPvt)
{
  Xspress3 *pPvt = (Xspress3 *)drvPvt;
  
  pPvt->dataTask();
}

/*************************************************************************************/
/** The following functions have C linkage, and can be called directly or from iocsh */


/**
 * Config function for IOC shell. It instantiates an instance of the driver.
 * @param portName The Asyn port name to use
 * @param numChannels The number of channels (eg. 8)
 */
extern "C" {

  int xspress3Config(const char *portName, int numChannels, int maxBuffers, size_t maxMemory)
  {
    asynStatus status = asynSuccess;
    
    /*Instantiate class.*/
    try {
      new Xspress3(portName, numChannels, maxBuffers, maxMemory);
    } catch (...) {
      cout << "Unknown exception caught when trying to construct Xspress3." << endl;
      status = asynError;
    }
    
    return(status);
  }
  
  /* Code for iocsh registration */
  
  /* xspress3Config */
  static const iocshArg xspress3ConfigArg0 = {"Port name", iocshArgString};
  static const iocshArg xspress3ConfigArg1 = {"Num Channels", iocshArgInt};
  static const iocshArg xspress3ConfigArg2 = {"Max Buffers", iocshArgInt};
  static const iocshArg xspress3ConfigArg3 = {"Max Memory", iocshArgInt};
  static const iocshArg * const xspress3ConfigArgs[] =  {&xspress3ConfigArg0,
							 &xspress3ConfigArg1,
							 &xspress3ConfigArg2,
							 &xspress3ConfigArg3};
  
  static const iocshFuncDef configXspress3 = {"xspress3Config", 4, xspress3ConfigArgs};
  static void configXspress3CallFunc(const iocshArgBuf *args)
  {
    xspress3Config(args[0].sval, args[1].ival, args[2].ival, args[3].ival);
  }
  
  static void xspress3Register(void)
  {
    iocshRegister(&configXspress3, configXspress3CallFunc);
  }
  
  epicsExportRegistrar(xspress3Register);

} // extern "C"


/****************************************************************************************/
