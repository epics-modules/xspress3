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

#include "xspress3Epics.h"

using std::cout;
using std::endl;

//Definitions of static class data members
const epicsUInt32 Xspress3::logFlow_ = 1;
const epicsUInt32 Xspress3::logError_ = 2;
const epicsInt32 Xspress3::ctrlDisable_ = 0;
const epicsInt32 Xspress3::ctrlEnable_ = 1;


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
  createParam(xsp3CtrlDataParamString,         asynParamInt32,       &xsp3CtrlDataParam);
  createParam(xsp3CtrlMcaParamString,         asynParamInt32,       &xsp3CtrlMcaParam);
  createParam(xsp3CtrlScaParamString,         asynParamInt32,       &xsp3CtrlScaParam);
  createParam(xsp3CtrlTotalParamString,         asynParamInt32,       &xsp3CtrlTotalParam);
  createParam(xsp3CtrlDataPeriodParamString,         asynParamInt32,       &xsp3CtrlDataPeriodParam);
  createParam(xsp3CtrlMcaPeriodParamString,         asynParamInt32,       &xsp3CtrlMcaPeriodParam);
  createParam(xsp3CtrlScaPeriodParamString,         asynParamInt32,       &xsp3CtrlScaPeriodParam);
  createParam(xsp3CtrlTotalPeriodParamString,         asynParamInt32,       &xsp3CtrlTotalPeriodParam);
  
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

  //Set any paramLib parameters that need passing up to device support
  status = setIntegerParam(xsp3NumChannelsParam, numChannels_);
  status |= setIntegerParam(xsp3MaxNumChannelsParam, numChannels_); //Set this to what the system reports, not what we pass in
  status |= setIntegerParam(xsp3TriggerModeParam, 0);
  status |= setIntegerParam(xsp3NumFramesParam, 0);
  for (int chan=0; chan<numChannels_; chan++) {
    status |= setIntegerParam(chan, xsp3ChanSca1Param, 0);
    status |= setIntegerParam(chan, xsp3ChanSca2Param, 0);
    status |= setIntegerParam(chan, xsp3ChanSca3Param, 0);
    status |= setIntegerParam(chan, xsp3ChanSca4Param, 0);
    status |= setDoubleParam(chan,  xsp3ChanSca1CorrParam, 0);
    status |= setDoubleParam(chan,  xsp3ChanSca2CorrParam, 0);
    status |= setDoubleParam(chan,  xsp3ChanSca3CorrParam, 0);
    status |= setDoubleParam(chan,  xsp3ChanSca4CorrParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca1HlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca2HlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca3HlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca4HlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca1LlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca2LlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca3LlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca4LlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanTotalParam, 0);
    status |= setDoubleParam(chan,  xsp3ChanTotalCorrParam, 0);
  }

  //Init asynNDArray params
  //NDArraySizeX
  //NDArraySizeY
  //NDArraySize


  //Do we need to do a status read now?
  //statusRead();

  running_ = 1;
  status |= setStringParam(xsp3StatusParam, "System Init Complete");

  callParamCallbacks();

  if (status) {
    printf("%s:%s Unable to set driver parameters in constructor.\n", driverName, functionName);
  }

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


/** Report status of the driver.
  * Prints details about the detector in us if details>0.
  * It then calls the ADDriver::report() method.
  * \param[in] fp File pointed passed by caller where the output is written to.
  * \param[in] details Controls the level of detail in the report. */
void Xspress3::report(FILE *fp, int details)
{
 
  fprintf(fp, "Xspress3::report.\n");

  fprintf(fp, "Xspress3 port=%s\n", this->portName);
  if (details > 0) { 
    fprintf(fp, "Xspress3 driver details...\n");
  }

  fprintf(fp, "Xspress3 finished.\n");
  
  //Call the base class method
  asynNDArrayDriver::report(fp, details);

}

    


/**
 * Reimplementing this function from asynNDArrayDriver to deal with integer values.
 */ 
asynStatus Xspress3::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
  int function = pasynUser->reason;
  int addr = 0;
  asynStatus status = asynSuccess;
  int maxNumChannels = 0;
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
  else if (function == xsp3NumChannelsParam) {
   log(logFlow_, "Set number of channels", functionName);
   cout << "channels: " << value << endl;
   getIntegerParam(xsp3MaxNumChannelsParam, &maxNumChannels);
   if ((value > maxNumChannels) || (value < 1)) {
     log(logError_, "ERROR: num channels out of range.", functionName);
     return asynError;
   }
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
  } 
  else if (function == xsp3CtrlDataParam) {
    if (value == ctrlDisable_) {
      log(logFlow_, "Disabling all live data update.", functionName);
      setIntegerParam(xsp3CtrlMcaParam, 0);
      setIntegerParam(xsp3CtrlScaParam, 0);
      setIntegerParam(xsp3CtrlTotalParam, 0);
    } else if (value == ctrlEnable_) {
      log(logFlow_, "Enabling live data update.", functionName);
    }
  } 
  else if (function == xsp3CtrlMcaParam) {
    if (value == ctrlDisable_) {
      log(logFlow_, "Disabling MCA data update.", functionName);
    } else if (value == ctrlEnable_) {
      log(logFlow_, "Enabling MCA data update.", functionName);
      setIntegerParam(xsp3CtrlDataParam, 1);
    }
  } 
  else if (function == xsp3CtrlScaParam) {
    if (value == ctrlDisable_) {
      log(logFlow_, "Disabling SCA data update.", functionName);
    } else if (value == ctrlEnable_) {
      log(logFlow_, "Enabling SCA data update.", functionName);
      setIntegerParam(xsp3CtrlDataParam, 1);
    }
  } 
  else if (function == xsp3CtrlTotalParam) {
    if (value == ctrlDisable_) {
      log(logFlow_, "Disabling total data update.", functionName);
    } else if (value == ctrlEnable_) {
      log(logFlow_, "Enabling total data update.", functionName);
      setIntegerParam(xsp3CtrlDataParam, 1);
    }
  } 
  else if (function == xsp3CtrlDataPeriodParam) {
    log(logFlow_, "Setting scalar data update rate.", functionName);
  }
  else if (function == xsp3CtrlMcaPeriodParam) {
    log(logFlow_, "Setting MCA data update rate.", functionName);
  }
  else if (function == xsp3CtrlScaPeriodParam) {
    log(logFlow_, "Setting SCA data update rate.", functionName);
  }
  else if (function == xsp3CtrlTotalPeriodParam) {
    log(logFlow_, "Setting total data update rate.", functionName);
  }
  else {
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
  epicsFloat32 timeout = 0;
  epicsUInt32 forcedFastPolls = 0;

  const char* functionName = "Xspress3::statusTask";

  log(logFlow_, "Started.", functionName);

  
  while(1) {

    //Read timeout for polling freq.
    this->lock();
    if (forcedFastPolls > 0) {
      timeout = fastPollingPeriod_;
      forcedFastPolls--;
    } else {
      timeout = pollingPeriod_;
    }
    this->unlock();

    if (timeout != 0.0) {
      eventStatus = epicsEventWaitWithTimeout(statusEvent_, timeout);
    } else {
      eventStatus = epicsEventWait(statusEvent_);
    }              
    if (eventStatus == (epicsEventWaitOK || epicsEventWaitTimeout)) {
      //We got an event, rather than a timeout.  This is because other software
      //knows that data has arrived, or device should have changed state (parameters changed, etc.).
      //Force a minimum number of fast polls, because the device status
      //might not have changed in the first few polls
      forcedFastPolls = 5;
    }

    this->lock();
    
    if (debug_) {
      //log(logFlow_, "Got status event.", functionName);
    }
    
    /* Call the callbacks to update any changes */
    callParamCallbacks();
    this->unlock();
    
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
  int numChannels = 0;
  const char* functionName = "Xspress3::dataTask";
  NDArray *pArray;

  log(logFlow_, "Started.", functionName);

   while (1) {

     //SEE Pilatus driver for example for how to avoid start/stop deadlock.
     //May need to use acquire/aborted flags like in pilatus driver.

     eventStatus = epicsEventWait(startEvent_);          
     if (eventStatus == epicsEventWaitOK) {
        log(logFlow_, "Got start event.", functionName);
      }

     //Get the number of channels in use
     getIntegerParam(xsp3NumChannelsParam, &numChannels);

     //Start acqusition here.

     //Unlock and wait for a stop event (see below for example).

     //Readout data

     //Use the API function get_bins_per_mca for spectra length. (should be 4096?)

     //Create data arrays of the correct size using pArray = this->pNDArrayPool->alloc
     //Set NDArraySize
     //Increment image counters (NDArrayCounter)
     //Set uniqueId and timestamp in pArray
     //Set the scalars as attribute data. Get the list like: this->getAttributes(pArray->pAttributeList);

     //Notes on data format:
     //Save a 2D NDArray for the spectra data (channel * MCA bin)
     //Attach all the scalar data as attributes to this NDArray (seperate attributes per channel, per scalar).

     for (int chan=1; chan<=numChannels; chan++) {
       
     }

     //Call this->unlock(); before doCallbacksGenericPointer(pArray, NDArrayData, 0); (the last param is the address).
     //Then lock() again, and call pArray->release();
     
     //Post arrays to channel access, using doCallbacksFloat64Array. Might have to limit this to a preset freq by 
     //checking the time since last post. Or, not post at all depending on what the user set (until a stop acqusition is done).
     
     //callParamCallbacks();


     //Check for a stop event with a small timeout?
     //eventStatus = epicsEventWaitWithTimeout(stopEvent_, timeout);          
     //if (eventStatus == epicsEventWaitOK) {
     //  log(logFlow_, "Got stop event.", functionName);
     //}

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
