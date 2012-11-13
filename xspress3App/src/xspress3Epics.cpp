#include <iostream>
#include <string>
#include <stdexcept>
#include "dirent.h"

//Epics headers
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsExport.h>
#include <epicsString.h>
#include <iocsh.h>
#include <drvSup.h>
#include <registryFunction.h>

//Xspress3 API header
#include "xspress3.h"

#include "xspress3Epics.h"

using std::cout;
using std::endl;

//Definitions of static class data members
const epicsInt32 Xspress3::ctrlDisable_ = 0;
const epicsInt32 Xspress3::ctrlEnable_ = 1;
const epicsInt32 Xspress3::runFlag_MCA_SPECTRA_ = 0;
const epicsInt32 Xspress3::runFlag_PLAYB_MCA_SPECTRA_ = 1;
const epicsInt32 Xspress3::maxNumRoi_ = 4;

//C Function prototypes to tie in with EPICS
static void xsp3DataTaskC(void *drvPvt);


/**
 * Constructor for Xspress3::Xspress3. 
 * This must be called in the Epics IOC startup file.
 * @param portName The Asyn port name to use
 * @param numChannels The max number of channels to use (eg. 4 or 8)
 * @param numCards The number of Xspress3 systems (normally 1)
 * @param baseIP The base address used by the Xspress3 1Gig and 10Gig interfaces (eg. "192.168.0.1")
 * @param maxFrames The maximum number of frames that can be acquired in one acquisition (eg. 16384) 
 * @param maxSpectra The maximum size of each spectra (eg. 4096)
 * @param maxBuffers Used by asynPortDriver (set to -1 for unlimited)
 * @param maxMemory Used by asynPortDriver (set to -1 for unlimited)
 * @param debug This debug flag is passed to xsp3_config in the Xspress API (0 or 1)
 * @param simTest 0 or 1. Set to 1 to run up this driver in simulation mode. 
 */
Xspress3::Xspress3(const char *portName, int numChannels, int numCards, const char *baseIP, int maxFrames, int maxSpectra, int maxBuffers, size_t maxMemory, int debug, int simTest)
  : ADDriver(portName,
	     numChannels, /* maxAddr - channels use different param lists*/ 
	     NUM_DRIVER_PARAMS,
	     maxBuffers,
	     maxMemory,
	     asynInt32Mask | asynInt32ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynFloat64ArrayMask | asynDrvUserMask | asynOctetMask | asynGenericPointerMask, /* Interface mask */
	     asynInt32Mask | asynInt32ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynFloat64ArrayMask | asynOctetMask | asynGenericPointerMask,  /* Interrupt mask */
	     ASYN_CANBLOCK | ASYN_MULTIDEVICE, /* asynFlags.*/
	     1, /* Autoconnect */
	     0, /* Default priority */
	     0), /* Default stack size*/
    debug_(debug), numChannels_(numChannels), simTest_(simTest)
{
  int status = asynSuccess;
  const char *functionName = "Xspress3::Xspress3";

  strncpy(baseIP_, baseIP, 16);
  baseIP_[16] = '\0';

  //Create the epicsEvent for signaling to the status task when parameters should have changed.
  //This will cause it to do a poll immediately, rather than wait for the poll time period.
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
  createParam(xsp3NumChannelsParamString,   asynParamInt32,       &xsp3NumChannelsParam);
  createParam(xsp3MaxNumChannelsParamString,asynParamInt32,       &xsp3MaxNumChannelsParam);
  createParam(xsp3MaxSpectraParamString,asynParamInt32,       &xsp3MaxSpectraParam);
  createParam(xsp3MaxFramesParamString,asynParamInt32,       &xsp3MaxFramesParam);
  createParam(xsp3FrameCountParamString,asynParamInt32,       &xsp3FrameCountParam);
  createParam(xsp3TriggerModeParamString,   asynParamInt32,       &xsp3TriggerModeParam);
  createParam(xsp3FixedTimeParamString,   asynParamInt32,       &xsp3FixedTimeParam);
  createParam(xsp3NumFramesConfigParamString,     asynParamInt32,       &xsp3NumFramesConfigParam);
  createParam(xsp3NumCardsParamString,      asynParamInt32,       &xsp3NumCardsParam);
  createParam(xsp3ConfigPathParamString,    asynParamOctet,       &xsp3ConfigPathParam);
  createParam(xsp3ConfigSavePathParamString,    asynParamOctet,       &xsp3ConfigSavePathParam);
  createParam(xsp3ConnectParamString,      asynParamInt32,       &xsp3ConnectParam);
  createParam(xsp3ConnectedParamString,      asynParamInt32,       &xsp3ConnectedParam);
  createParam(xsp3DisconnectParamString,      asynParamInt32,       &xsp3DisconnectParam);
  createParam(xsp3SaveSettingsParamString,      asynParamInt32,       &xsp3SaveSettingsParam);
  createParam(xsp3RestoreSettingsParamString,      asynParamInt32,       &xsp3RestoreSettingsParam);
  createParam(xsp3RunFlagsParamString,      asynParamInt32,       &xsp3RunFlagsParam);
  //These params will use different param lists based on asyn address
  createParam(xsp3ChanMcaParamString,       asynParamInt32Array,  &xsp3ChanMcaParam);
  createParam(xsp3ChanMcaCorrParamString,   asynParamFloat64Array,&xsp3ChanMcaCorrParam);
  createParam(xsp3ChanMcaRoi1LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi1LlmParam);
  createParam(xsp3ChanMcaRoi2LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi2LlmParam);
  createParam(xsp3ChanMcaRoi3LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi3LlmParam);
  createParam(xsp3ChanMcaRoi4LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi4LlmParam);
  createParam(xsp3ChanMcaRoi1HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi1HlmParam);
  createParam(xsp3ChanMcaRoi2HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi2HlmParam);
  createParam(xsp3ChanMcaRoi3HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi3HlmParam);
  createParam(xsp3ChanMcaRoi4HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi4HlmParam);
  createParam(xsp3ChanMcaRoi1ParamString,     asynParamFloat64,&xsp3ChanMcaRoi1Param);
  createParam(xsp3ChanMcaRoi2ParamString,     asynParamFloat64,&xsp3ChanMcaRoi2Param);
  createParam(xsp3ChanMcaRoi3ParamString,     asynParamFloat64,&xsp3ChanMcaRoi3Param);
  createParam(xsp3ChanMcaRoi4ParamString,     asynParamFloat64,&xsp3ChanMcaRoi4Param);
  createParam(xsp3ChanMcaRoi1ArrayParamString, asynParamFloat64Array, &xsp3ChanMcaRoi1ArrayParam);
  createParam(xsp3ChanMcaRoi2ArrayParamString, asynParamFloat64Array, &xsp3ChanMcaRoi2ArrayParam);
  createParam(xsp3ChanMcaRoi3ArrayParamString, asynParamFloat64Array, &xsp3ChanMcaRoi3ArrayParam);
  createParam(xsp3ChanMcaRoi4ArrayParamString, asynParamFloat64Array, &xsp3ChanMcaRoi4ArrayParam);
  createParam(xsp3ChanSca0ParamString,      asynParamInt32,       &xsp3ChanSca0Param);
  createParam(xsp3ChanSca1ParamString,      asynParamInt32,       &xsp3ChanSca1Param);
  createParam(xsp3ChanSca2ParamString,      asynParamInt32,       &xsp3ChanSca2Param);
  createParam(xsp3ChanSca3ParamString,      asynParamInt32,       &xsp3ChanSca3Param);
  createParam(xsp3ChanSca4ParamString,      asynParamInt32,       &xsp3ChanSca4Param);
  createParam(xsp3ChanSca5ParamString,      asynParamInt32,       &xsp3ChanSca5Param);
  createParam(xsp3ChanSca6ParamString,      asynParamInt32,       &xsp3ChanSca6Param);
  createParam(xsp3ChanSca7ParamString,      asynParamInt32,       &xsp3ChanSca7Param);
  createParam(xsp3ChanSca5CorrParamString,  asynParamFloat64,     &xsp3ChanSca5CorrParam);
  createParam(xsp3ChanSca6CorrParamString,  asynParamFloat64,     &xsp3ChanSca6CorrParam);
  createParam(xsp3ChanSca0ArrayParamString,asynParamInt32Array,  &xsp3ChanSca0ArrayParam);
  createParam(xsp3ChanSca1ArrayParamString,asynParamInt32Array,  &xsp3ChanSca1ArrayParam);
  createParam(xsp3ChanSca2ArrayParamString,asynParamInt32Array,  &xsp3ChanSca2ArrayParam);
  createParam(xsp3ChanSca3ArrayParamString,asynParamInt32Array,  &xsp3ChanSca3ArrayParam);
  createParam(xsp3ChanSca4ArrayParamString,asynParamInt32Array,  &xsp3ChanSca4ArrayParam);
  createParam(xsp3ChanSca5ArrayParamString,asynParamInt32Array,  &xsp3ChanSca5ArrayParam);
  createParam(xsp3ChanSca6ArrayParamString,asynParamInt32Array,  &xsp3ChanSca6ArrayParam);
  createParam(xsp3ChanSca7ArrayParamString,asynParamInt32Array,  &xsp3ChanSca7ArrayParam);
  createParam(xsp3ChanSca5CorrArrayParamString,asynParamFloat64Array,  &xsp3ChanSca5CorrArrayParam);
  createParam(xsp3ChanSca6CorrArrayParamString,asynParamFloat64Array,  &xsp3ChanSca6CorrArrayParam);
  createParam(xsp3ChanSca4ThresholdParamString,   asynParamInt32,       &xsp3ChanSca4ThresholdParam);
  createParam(xsp3ChanSca5HlmParamString,   asynParamInt32,       &xsp3ChanSca5HlmParam);
  createParam(xsp3ChanSca6HlmParamString,   asynParamInt32,       &xsp3ChanSca6HlmParam);
  createParam(xsp3ChanSca5LlmParamString,   asynParamInt32,       &xsp3ChanSca5LlmParam);
  createParam(xsp3ChanSca6LlmParamString,   asynParamInt32,       &xsp3ChanSca6LlmParam);
  createParam(xsp3ChanDtcFlagsParamString,   asynParamInt32,       &xsp3ChanDtcFlagsParam);
  createParam(xsp3ChanDtcAegParamString,   asynParamFloat64,       &xsp3ChanDtcAegParam);
  createParam(xsp3ChanDtcAeoParamString,   asynParamFloat64,       &xsp3ChanDtcAeoParam);
  createParam(xsp3ChanDtcIwgParamString,   asynParamFloat64,       &xsp3ChanDtcIwgParam);
  createParam(xsp3ChanDtcIwoParamString,   asynParamFloat64,       &xsp3ChanDtcIwoParam);
  //These control data update rates and ROI calculations
  createParam(xsp3CtrlDataParamString,         asynParamInt32,       &xsp3CtrlDataParam);
  createParam(xsp3CtrlMcaParamString,         asynParamInt32,       &xsp3CtrlMcaParam);
  createParam(xsp3CtrlScaParamString,         asynParamInt32,       &xsp3CtrlScaParam);
  createParam(xsp3CtrlDataPeriodParamString,         asynParamInt32,       &xsp3CtrlDataPeriodParam);
  createParam(xsp3CtrlMcaPeriodParamString,         asynParamInt32,       &xsp3CtrlMcaPeriodParam);
  createParam(xsp3CtrlScaPeriodParamString,         asynParamInt32,       &xsp3CtrlScaPeriodParam);
  createParam(xsp3RoiEnableParamString,         asynParamInt32,       &xsp3RoiEnableParam);
  
  //Initialize non static, non const, data members
  acquiring_ = 0;
  running_ = 0;
  xsp3_handle_ = 0;
  
  //Create the thread that readouts the data 
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

  //Initialise any paramLib parameters that need passing up to device support
  status = setIntegerParam(xsp3NumChannelsParam, numChannels_);
  status |= setIntegerParam(xsp3MaxNumChannelsParam, numChannels_);
  status |= setIntegerParam(xsp3TriggerModeParam, 0);
  status |= setIntegerParam(xsp3FixedTimeParam, 0);
  status |= setIntegerParam(ADNumImages, 0);
  status |= setIntegerParam(xsp3NumFramesConfigParam, maxFrames);
  status |= setIntegerParam(xsp3MaxSpectraParam, maxSpectra);
  status |= setIntegerParam(xsp3MaxFramesParam, maxFrames);
  status |= setIntegerParam(xsp3NumCardsParam, numCards);
  status |= setIntegerParam(xsp3FrameCountParam, 0);
  status |= setIntegerParam(ADStatus, ADStatusDisconnected);
  status =  setStringParam (ADManufacturer, "Quantum Detectors");
  status |= setStringParam (ADModel, "Xspress3");
  for (int chan=0; chan<numChannels_; chan++) {
    status |= setIntegerParam(chan, xsp3ChanMcaRoi1LlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanMcaRoi2LlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanMcaRoi3LlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanMcaRoi4LlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanMcaRoi1HlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanMcaRoi2HlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanMcaRoi3HlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanMcaRoi4HlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca4ThresholdParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca5HlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca6HlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca5LlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanSca6LlmParam, 0);
    status |= setIntegerParam(chan, xsp3ChanDtcFlagsParam, 0);
    status |= setDoubleParam(chan, xsp3ChanDtcAegParam, 0);
    status |= setDoubleParam(chan, xsp3ChanDtcAeoParam, 0);
    status |= setDoubleParam(chan, xsp3ChanDtcIwgParam, 0);
    status |= setDoubleParam(chan, xsp3ChanDtcIwoParam, 0);
  }
  status |= eraseSCAMCAROI();

  if (simTest_) {
    status |= setStringParam(ADStatusMessage, "Init. Simulation Mode.");
  } else {
    status |= setStringParam(ADStatusMessage, "Init. System Disconnected.");
  }
  
  callParamCallbacks();

  if (status) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s Unable To Set Driver Parameters In Constructor.\n", functionName);
  }

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s End Of Constructor.\n", functionName);

}

Xspress3::~Xspress3() 
{
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "Xspress3::~Xspress3 Called.\n");
}


/**
 * Function to connect to the Xspress3 by calling xsp3_config and 
 * some other setup functions.
 */
asynStatus Xspress3::connect(void)
{
  asynStatus status = asynSuccess;
  int xsp3_num_cards = 0;
  int xsp3_num_tf = 0;
  int xsp3_status = 0;
  int xsp3_run_flags = 0;
  int xsp3_num_channels = 0;
  char configPath[256] = {0};
  char configSavePath[256] = {0};
  const char *functionName = "Xspress3::connect";

  getIntegerParam(xsp3NumCardsParam, &xsp3_num_cards);
  getIntegerParam(xsp3NumFramesConfigParam, &xsp3_num_tf);
  getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);
  getStringParam(xsp3ConfigPathParam, static_cast<int>(sizeof(configPath)), configPath);
  getStringParam(xsp3ConfigSavePathParam, static_cast<int>(sizeof(configSavePath)), configSavePath);

  //Set up the xspress3 system
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Calling xsp3_config.\n", functionName);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Base IP address is: %s\n", functionName, baseIP_);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Number of cards is: %d\n", functionName, xsp3_num_cards);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Number of frames is: %d\n", functionName, xsp3_num_tf);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Number of channels is: %d\n", functionName, numChannels_);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Config path is: %d\n", functionName, configPath);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Config save path is: %d\n", functionName, configSavePath);

  xsp3_handle_ = xsp3_config(xsp3_num_cards, xsp3_num_tf, const_cast<char *>(baseIP_), -1, NULL, xsp3_num_channels, 1, NULL, debug_, 0);
  if (xsp3_handle_ < 0) {
    checkStatus(xsp3_handle_, "xsp3_config", functionName);
    status = asynError;
  } else {

    //Set up clocks on each card
    for (int i=0; i<xsp3_num_cards; i++) {
      xsp3_status = xsp3_clocks_setup(xsp3_handle_, i, XSP3_CLK_SRC_INT, XSP3_CLK_FLAGS_MASTER, 0);
      if (xsp3_status != XSP3_OK) {
	checkStatus(xsp3_status, "xsp3_clocks_setup", functionName);
	status = asynError;
      }
    }
    
    //Restore settings from a file
    xsp3_status = xsp3_restore_settings(xsp3_handle_, configPath, 0);
    if (xsp3_status != XSP3_OK) {
      checkStatus(xsp3_status, "xsp3_restore_settings", functionName);
      status = asynError;
    }

    //Can we do xsp3_format_run here? For normal user operation all the arguments seem to be set to zero.
    for (int chan=0; chan<xsp3_num_channels; chan++) {
      xsp3_status = xsp3_format_run(xsp3_handle_, chan, 0, 0, 0, 0, 0, 12);
      if (xsp3_status < XSP3_OK) {
	checkStatus(xsp3_status, "xsp3_format_run", functionName);
	status = asynError;
      } else {
	asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Channel: %d, Number of time frames configured: %d\n", functionName, chan, xsp3_status);
      }
    }

    //Read run flags parameter
    getIntegerParam(xsp3RunFlagsParam, &xsp3_run_flags);
    if (xsp3_run_flags == runFlag_MCA_SPECTRA_) {
      xsp3_status = xsp3_set_run_flags(xsp3_handle_, XSP3_RUN_FLAGS_SCALERS | XSP3_RUN_FLAGS_HIST);
    } else if (xsp3_run_flags == runFlag_PLAYB_MCA_SPECTRA_) {
      xsp3_status = xsp3_set_run_flags(xsp3_handle_, XSP3_RUN_FLAGS_PLAYBACK | XSP3_RUN_FLAGS_SCALERS | XSP3_RUN_FLAGS_HIST);
    } else {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s Invalid run flag option when trying to set xsp3_set_run_flags.\n", functionName);
      status = asynError;
    }
    if (xsp3_status < XSP3_OK) {
      checkStatus(xsp3_status, "xsp3_set_run_flags", functionName);
      status = asynError;
    } 

    //Set connected status
    if (status == asynSuccess) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Finished setting up Xspress3.\n", functionName);
      setStringParam(ADStatusMessage, "System Connected");
      setIntegerParam(ADStatus, ADStatusIdle);
      setIntegerParam(xsp3ConnectedParam, 1);
    } else {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR setting up Xspress3.\n", functionName);
      setStringParam(ADStatusMessage, "ERROR: failed to connect");
      setIntegerParam(ADStatus, ADStatusDisconnected);
      setIntegerParam(xsp3ConnectedParam, 0);
    }

    //Need to read existing SCA params, and DTC params
    if (status == asynSuccess) {
      status = readSCAParams();
    }
    if (status == asynSuccess) {
      status = readDTCParams();
    }
    
  } // End of if xsp3_handle_ < 0

  return status;
}

/**
 * Read the SCA window limits (for SCA 5 and 6) and threshold for SCA 4, for each channel.
 */
asynStatus Xspress3::readSCAParams(void) 
{
  asynStatus status = asynSuccess;
  int xsp3_num_channels = 0;
  int xsp3_status = 0;
  u_int32_t xsp3_sca_param1 = 0;
  u_int32_t xsp3_sca_param2 = 0;
  const char *functionName = "Xspress3::readSCAParams";

  getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);

  for (int chan=0; chan<xsp3_num_channels; chan++) {
    //SCA 5 window limits
    xsp3_status = xsp3_get_window(xsp3_handle_, chan, 0, &xsp3_sca_param1, &xsp3_sca_param2);
    if (xsp3_status < XSP3_OK) {
      checkStatus(xsp3_status, "xsp3_get_window", functionName);
      status = asynError;
    } else {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Channel %d, Read back SCA5 window limits: %d, %d\n", 
		functionName, chan, xsp3_sca_param1, xsp3_sca_param2);
      setIntegerParam(chan, xsp3ChanSca5LlmParam, xsp3_sca_param1);
      setIntegerParam(chan, xsp3ChanSca5HlmParam, xsp3_sca_param2);
    }
    //SCA 6 window limits
    xsp3_status = xsp3_get_window(xsp3_handle_, chan, 1, &xsp3_sca_param1, &xsp3_sca_param2);
    if (xsp3_status < XSP3_OK) {
      checkStatus(xsp3_status, "xsp3_get_window", functionName);
      status = asynError;
    } else {
      setIntegerParam(chan, xsp3ChanSca6LlmParam, xsp3_sca_param1);
      setIntegerParam(chan, xsp3ChanSca6HlmParam, xsp3_sca_param2);
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Channel %d, Read back SCA6 window limits: %d, %d\n", 
		functionName, chan, xsp3_sca_param1, xsp3_sca_param2);
    }
    //SCA 4 threshold limit
    xsp3_status = xsp3_get_good_thres(xsp3_handle_, chan, &xsp3_sca_param1);
    if (xsp3_status < XSP3_OK) {
      checkStatus(xsp3_status, "xsp3_get_good_thres", functionName);
      status = asynError;
    } else {
      setIntegerParam(chan, xsp3ChanSca4ThresholdParam, xsp3_sca_param1);
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Channel %d, Read back SCA4 threshold limit: %d\n", 
		functionName, chan, xsp3_sca_param1);
    }
    
    callParamCallbacks(chan);
  }

  return status;
}

/**
 * Read the dead time correction (DTC) parameters for each channel.
 */
asynStatus Xspress3::readDTCParams(void) 
{
  asynStatus status = asynSuccess;
  int xsp3_num_channels = 0;
  int xsp3_status = 0;
  int xsp3_dtc_flags = 0;
  double xsp3_dtc_all_event_grad = 0.0;
  double xsp3_dtc_all_event_off = 0.0;
  double xsp3_dtc_in_window_grad = 0.0;
  double xsp3_dtc_in_window_off = 0.0;
  const char *functionName = "Xspress3::readDTCParams";

  getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);

  for (int chan=0; chan<xsp3_num_channels; chan++) {
    xsp3_status = xsp3_getDeadtimeCorrectionParameters(xsp3_handle_, 
						       chan, 
						       &xsp3_dtc_flags, 
						       &xsp3_dtc_all_event_grad,
						       &xsp3_dtc_all_event_off,
						       &xsp3_dtc_in_window_off,
						       &xsp3_dtc_in_window_grad);
    if (xsp3_status < XSP3_OK) {
      checkStatus(xsp3_status, "xsp3_getDeadtimeCorrectionParameters", functionName);
      status = asynError;
    } else {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, 
		"%s Channel %d Dead Time Correction Params: Flags: %d, All Event Grad: %f, All Event Off: %f, In Win Off: %f, In Win Grad: %f\n", 
		functionName, chan, xsp3_dtc_flags, xsp3_dtc_all_event_grad, xsp3_dtc_all_event_off, xsp3_dtc_in_window_off, xsp3_dtc_in_window_grad);
      
      setIntegerParam(chan, xsp3ChanDtcFlagsParam, xsp3_dtc_flags);
      setDoubleParam(chan, xsp3ChanDtcAegParam, xsp3_dtc_all_event_grad);
      setDoubleParam(chan, xsp3ChanDtcAeoParam, xsp3_dtc_all_event_off);
      setDoubleParam(chan, xsp3ChanDtcIwgParam, xsp3_dtc_in_window_grad);
      setDoubleParam(chan, xsp3ChanDtcIwoParam, xsp3_dtc_in_window_off);
    }
    
    callParamCallbacks(chan);
  }

  return status;
}



/**
 * Disconnect from the xspress3 system. 
 * This simply calls xsp3_close().
 */
asynStatus Xspress3::disconnect(void)
{
  asynStatus status = asynSuccess;
  int xsp3_status = 0;
  const char *functionName = "Xspress3::disconnect";

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Calling disconnect. This calls xsp3_close().\n", functionName);

  if ((status = checkConnected()) == asynSuccess) {
    xsp3_status = xsp3_close(xsp3_handle_);
    if (xsp3_status != XSP3_OK) {
      checkStatus(xsp3_status, "xsp3_close", functionName);
      status = asynError;
    }
    setStringParam(ADStatusMessage, "System disconnected.");
    setIntegerParam(ADStatus, ADStatusDisconnected);
    setIntegerParam(xsp3ConnectedParam, 0);
  }
  
  return status;
}

/**
 * Check the connected status.
 * @return asynSuccess if connected, or asynError if disconnected.
 */
asynStatus Xspress3::checkConnected(void) 
{
  int xsp3_connected = 0;
  const char *functionName = "Xspress3::checkConnected";

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Checking connected status.\n", functionName);

  getIntegerParam(xsp3ConnectedParam, &xsp3_connected);
  if (xsp3_connected != 1) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: We are not connected.\n", functionName);
    setStringParam(ADStatusMessage, "ERROR: Not Connected");
    return asynError;
  }

  return asynSuccess;
}


/**
 * Save the system settings for the xspress3 system. 
 * This simply calls xsp3_save_settings().
 */
asynStatus Xspress3::saveSettings(void)
{
  asynStatus status = asynSuccess;
  int xsp3_status = 0;
  char configSavePath[256] = {0};
  int connected = 0;
  const char *functionName = "Xspress3::saveSettings";

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Saving Xspress3 settings. This calls xsp3_save_settings().\n", functionName);

  getIntegerParam(xsp3ConnectedParam, &connected);
  getStringParam(xsp3ConfigSavePathParam, static_cast<int>(sizeof(configSavePath)), configSavePath);

  if ((configSavePath == NULL) || (connected != 1)) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: No config save path set, or not connected.\n", functionName);
    setIntegerParam(ADStatus, ADStatusError);
    status = asynError;
  } else {
    xsp3_status = xsp3_save_settings(xsp3_handle_, configSavePath);
    if (xsp3_status != XSP3_OK) {
      checkStatus(xsp3_status, "xsp3_save_settings", functionName);
      setStringParam(ADStatusMessage, "Error Saving Configuration.");
      setIntegerParam(ADStatus, ADStatusError);
      status = asynError;
    } else {
      setStringParam(ADStatusMessage, "Saved Configuration.");
    }
  }

  return status;
}

/**
 * Restore the system settings for the xspress3 system. 
 * This simply calls xsp3_restore_settings().
 */
asynStatus Xspress3::restoreSettings(void)
{
  asynStatus status = asynSuccess;
  int xsp3_status = 0;
  char configPath[256] = {0};
  int connected = 0;
  const char *functionName = "Xspress3::restoreSettings";

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Restoring Xspress3 settings. This calls xsp3_restore_settings().\n", functionName);

  getIntegerParam(xsp3ConnectedParam, &connected);
  getStringParam(xsp3ConfigPathParam, static_cast<int>(sizeof(configPath)), configPath);

  if ((configPath == NULL) || (connected != 1)) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: No config path set, or not connected.\n", functionName);
    setIntegerParam(ADStatus, ADStatusError);
    status = asynError;
  } else {
    xsp3_status = xsp3_restore_settings(xsp3_handle_, configPath, 0);
    if (xsp3_status != XSP3_OK) {
      checkStatus(xsp3_status, "xsp3_restore_settings", functionName);
      setStringParam(ADStatusMessage, "Error Restoring Configuration.");
      setIntegerParam(ADStatus, ADStatusError);
      status = asynError;
    } else {
      setStringParam(ADStatusMessage, "Restored Configuration.");
    }
  }
  
  return status;
}


/**
 * Funtion to log an error if any of the Xsp3 functions return an error.
 * The function also take a pointer to the name of the function.
 * @param status The int status returned from an xsp3 function.
 * @param function The name of the called xsp3 function.
 * @param parentFunction The name of the parent function.
 */
void Xspress3::checkStatus(int status, const char *function, const char *parentFunction)
{

  if (status == XSP3_OK) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s returned XSP3_OK.\n", function);
  } else if (status == XSP3_ERROR) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s returned XSP3_ERROR.\n", function);
  } else if (status == XSP3_INVALID_PATH) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s returned XSP3_ILLEGAL_PATH.\n", function);
  } else if (status == XSP3_ILLEGAL_CARD) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s returned XSP3_ILLEGAL_CARD.\n", function);
  } else if (status == XSP3_ILLEGAL_SUBPATH) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s returned XSP3_ILLEGAL_SUBPATH.\n", function);
  } else if (status == XSP3_INVALID_DMA_STREAM) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s returned XSP3_INVALID_DMA_STREAM.\n", function);
  } else if (status == XSP3_RANGE_CHECK) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s returned XSP3_RANGE_CHECK.\n", function);
  } else if (status == XSP3_INVALID_SCOPE_MOD) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s returned XSP3_INVALID_SCOPE_MOD.\n", function);
  } else {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s returned unknown XSP3 error code.\n", function);
  }

  if (status != XSP3_OK) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "API Error Message: %s.\n", xsp3_get_error_message());
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "EPICS Driver Calling Function: %s.\n", parentFunction);
  }
 
}

/**
 * Wrapper for xsp3_set_window that does some error checking.
 * @param channel The asyn address, mapping to xsp3 channel.
 * @param sca The SCA number
 * @param llm The low limit
 * @param hlm The high limit
 * @return asynStatus
 */
asynStatus Xspress3::setWindow(int channel, int sca, int llm, int hlm)
{
  asynStatus status = asynSuccess;
  int xsp3_status = 0;
  const char *functionName = "Xspress3::setWindow";

  if ((status = checkConnected()) == asynSuccess) {
    if (llm > hlm) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: SCA low limit is higher than high limit.\n", functionName);
      setStringParam(ADStatusMessage, "ERROR: SCA low limit is higher than high limit.");
      setIntegerParam(ADStatus, ADStatusError);
      status = asynError;
    } else {
      xsp3_status = xsp3_set_window(xsp3_handle_, channel, sca, llm, hlm); 
      if (xsp3_status != XSP3_OK) {
	checkStatus(xsp3_status, "xsp3_set_window", functionName);
	setStringParam(ADStatusMessage, "Error Setting SCA Window.");
	setIntegerParam(ADStatus, ADStatusError);
	status = asynError;
      } else {
	setStringParam(ADStatusMessage, "Set SCA Window.");
      }
    }
  }
  
  return status;
}

/**
 * Function to check that the ROI limits make sense.
 * @param channel The asyn address, mapping to xsp3 channel.
 * @param roi The ROI number
 * @param llm The low limit
 * @param hlm The high limit
 * @return asynStatus
 */
asynStatus Xspress3::checkRoi(int channel, int roi, int llm, int hlm)
{
  asynStatus status = asynSuccess;
  int maxSpectra = 0;
  const char *functionName = "Xspress3::checkRoi";

  getIntegerParam(xsp3MaxSpectraParam, &maxSpectra);

  if ((llm < 0) || (hlm < 0)) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Negative ROI limits not allowed.\n", functionName);
    setStringParam(ADStatusMessage, "ERROR: Negative ROI limits not allowed.");
    setIntegerParam(ADStatus, ADStatusError);
    status = asynError;
  }

  if ((llm >= maxSpectra) || (hlm > maxSpectra)) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: ROI limits set too high.\n", functionName);
    setStringParam(ADStatusMessage, "ERROR: ROI limits set too high.");
    setIntegerParam(ADStatus, ADStatusError);
    status = asynError;
  }

  if (llm > hlm) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, 
	      "%s ERROR: ROI %d low limit is higher than high limit on channel %d.\n", 
	      functionName, roi, channel);
    setStringParam(ADStatusMessage, "ERROR: ROI low limit is higher than high limit.");
    setIntegerParam(ADStatus, ADStatusError);
    status = asynError;
  }

  if (status != asynError) {
    setStringParam(ADStatusMessage, "Successfully set ROI limit.");
    setIntegerParam(ADStatus, ADStatusIdle);
  }
  
  return status;
}


/**
 * Call xsp3_histogram_clear, and clear scalar data.
 * @return asynStatus
 */
asynStatus Xspress3::erase(void)
{
  asynStatus status = asynSuccess;
  int xsp3_status = 0;
  int xsp3_time_frames = 0;
  int xsp3_num_channels = 0;
  const char *functionName = "Xspress3::erase";

  if ((status = checkConnected()) == asynSuccess) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Erase data.\n", functionName);

    status = eraseSCAMCAROI();

    getIntegerParam(xsp3NumFramesConfigParam, &xsp3_time_frames);
    getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);

    xsp3_status = xsp3_histogram_clear(xsp3_handle_, 0, xsp3_num_channels, 0, xsp3_time_frames);
    if (xsp3_status != XSP3_OK) {
      checkStatus(xsp3_status, "xsp3_histogram_clear", functionName);
      setIntegerParam(ADStatus, ADStatusError);
      status = asynError;
    } else {
      if (status == asynSuccess) {
	setStringParam(ADStatusMessage, "Erased Data");
      } else {
	setStringParam(ADStatusMessage, "Problem Erasing Data");
	setIntegerParam(ADStatus, ADStatusError);
      }
    }
  }
  
  return status;
}

/**
 * Function to clear the SCA data.
 */
asynStatus Xspress3::eraseSCAMCAROI(void)
{
  int status = asynSuccess;
  int xsp3_num_channels = 0;
  int maxNumFrames = 0;
  epicsInt32 *pSCA = NULL;
  epicsFloat64 *pROI = NULL;
  const char *functionName = "Xspress3::eraseSCA";

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Clear SCA data, MCA ROI data and all arrays.\n", functionName);

  getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);
  getIntegerParam(xsp3MaxFramesParam, &maxNumFrames);

  pSCA = static_cast<epicsInt32*>(calloc(maxNumFrames, sizeof(epicsUInt32)));
  if (pSCA == NULL) {
    perror(functionName);
    return asynError;
  }
  pROI = static_cast<epicsFloat64*>(calloc(maxNumFrames, sizeof(epicsFloat64)));
  if (pROI == NULL) {
    perror(functionName);
    free(pSCA);
    return asynError;
  }
  
  status |= setIntegerParam(NDArrayCounter, 0);
  status |= setIntegerParam(xsp3FrameCountParam, 0);

  for (int chan=0; chan<xsp3_num_channels; chan++) {
    status |= setIntegerParam(chan, xsp3ChanSca0Param, 0);
    status |= setIntegerParam(chan, xsp3ChanSca1Param, 0);
    status |= setIntegerParam(chan, xsp3ChanSca2Param, 0);
    status |= setIntegerParam(chan, xsp3ChanSca3Param, 0);
    status |= setIntegerParam(chan, xsp3ChanSca4Param, 0);
    status |= setIntegerParam(chan, xsp3ChanSca5Param, 0);
    status |= setIntegerParam(chan, xsp3ChanSca6Param, 0);
    status |= setIntegerParam(chan, xsp3ChanSca7Param, 0);
    status |= setDoubleParam(chan,  xsp3ChanSca5CorrParam, 0);
    status |= setDoubleParam(chan,  xsp3ChanSca6CorrParam, 0);

    status |= setDoubleParam(chan, xsp3ChanMcaRoi1Param, 0);
    status |= setDoubleParam(chan, xsp3ChanMcaRoi2Param, 0);
    status |= setDoubleParam(chan, xsp3ChanMcaRoi3Param, 0);
    status |= setDoubleParam(chan, xsp3ChanMcaRoi4Param, 0);
    
    doCallbacksFloat64Array(pROI, maxNumFrames, xsp3ChanMcaRoi1ArrayParam, chan);
    doCallbacksFloat64Array(pROI, maxNumFrames, xsp3ChanMcaRoi2ArrayParam, chan);
    doCallbacksFloat64Array(pROI, maxNumFrames, xsp3ChanMcaRoi3ArrayParam, chan);
    doCallbacksFloat64Array(pROI, maxNumFrames, xsp3ChanMcaRoi4ArrayParam, chan);

    doCallbacksInt32Array(pSCA, maxNumFrames, xsp3ChanSca0ArrayParam, chan);
    doCallbacksInt32Array(pSCA, maxNumFrames, xsp3ChanSca1ArrayParam, chan);
    doCallbacksInt32Array(pSCA, maxNumFrames, xsp3ChanSca2ArrayParam, chan);
    doCallbacksInt32Array(pSCA, maxNumFrames, xsp3ChanSca3ArrayParam, chan);
    doCallbacksInt32Array(pSCA, maxNumFrames, xsp3ChanSca4ArrayParam, chan);
    doCallbacksInt32Array(pSCA, maxNumFrames, xsp3ChanSca5ArrayParam, chan);
    doCallbacksInt32Array(pSCA, maxNumFrames, xsp3ChanSca6ArrayParam, chan);
    doCallbacksInt32Array(pSCA, maxNumFrames, xsp3ChanSca7ArrayParam, chan);

    callParamCallbacks(chan);
  }

  if (status != asynSuccess) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR erasing data.\n", functionName);
  }

  free(pSCA);
  free(pROI);

  return static_cast<asynStatus>(status);
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
 * Function to map the database trigger mode 
 * value to the macros defined by the API.
 * @param mode The database value
 * @param apiMode This returns the correct value for the API
 * @return asynStatus
 */
asynStatus Xspress3::mapTriggerMode(int mode, int *apiMode)
{
  asynStatus status = asynSuccess;
  const char *functionName = "Xspress3::mapTriggerMode";

  if (mode == 0) {
    *apiMode = XSP3_GTIMA_SRC_FIXED;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_FIXED, value: %d\n", 
	      functionName, XSP3_GTIMA_SRC_FIXED);
  } else if (mode == 1) {
    *apiMode = XSP3_GTIMA_SRC_INTERNAL;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_INTERNAL, value: %d\n", 
	      functionName, XSP3_GTIMA_SRC_INTERNAL);
  } else if (mode == 2) {
    *apiMode = XSP3_GTIMA_SRC_IDC;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_IDC, value: %d\n", 
	      functionName, XSP3_GTIMA_SRC_IDC);
  } else if (mode == 3) {
    *apiMode = XSP3_GTIMA_SRC_TTL_VETO_ONLY;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_TTL_VETO_ONLY, value: %d\n", 
	      functionName, XSP3_GTIMA_SRC_TTL_VETO_ONLY);
  } else if (mode == 4) {
    *apiMode = XSP3_GTIMA_SRC_TTL_BOTH;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_TTL_BOTH, value: %d\n", 
	      functionName, XSP3_GTIMA_SRC_TTL_BOTH);
  } else if (mode == 5) {
    *apiMode = XSP3_GTIMA_SRC_LVDS_VETO_ONLY;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_LVDS_VETO_ONLY, value: %d\n", 
	      functionName, XSP3_GTIMA_SRC_LVDS_VETO_ONLY);
  } else if (mode == 6) {
    *apiMode = XSP3_GTIMA_SRC_LVDS_BOTH;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_LVDS_BOTH, value: %d\n", 
	      functionName, XSP3_GTIMA_SRC_LVDS_BOTH);
  } else {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Mapping an unknown trigger mode. mode: %d\n", functionName, mode);
    setStringParam(ADStatusMessage, "ERROR Unknown Trigger Mode");
    setIntegerParam(ADStatus, ADStatusError);
    status = asynError;
  }

  return status;
}

/**
 * Reimplementing this function from ADDriver to deal with integer values.
 */ 
asynStatus Xspress3::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
  int function = pasynUser->reason;
  int addr = 0;
  int xsp3_status = 0;
  int xsp3_num_cards = 0;
  int xsp3_sca_lim = 0;
  int xsp3_roi_lim = 0;
  int xsp3_time_frames = 0;
  int xsp3_trigger_mode = 0;
  int adStatus = 0;
  asynStatus status = asynSuccess;
  int xsp3_num_channels = 0;
  const char *functionName = "Xspress3::writeInt32";
  
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Calling writeInt32.\n", functionName);

  //Read address (ie. channel number).
  status = getAddress(pasynUser, &addr); 
  if (status!=asynSuccess) {
    return(status);
  }

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s asynUser->reason: &d, value: %d, addr: %d\n", functionName, function, value, addr);

  getIntegerParam(ADStatus, &adStatus);

  if (function == xsp3ResetParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s System Reset.\n", functionName);
    if ((status = checkConnected()) == asynSuccess) {
      //What do we do here?
    }
  } 
  else if (function == xsp3EraseParam) {
    if (adStatus != ADStatusAcquire) {
      //If we are in sim mode, simply clear the params. Otherwise, use the API erase function too.
      if (simTest_) {
	status = eraseSCAMCAROI();
	setStringParam(ADStatusMessage, "Erased Data");
      } else {
	status = erase();
      }
    }
  } 
  else if (function == ADAcquire) {
    if (value) {
      if (adStatus != ADStatusAcquire) {
	if (simTest_) {
	  epicsEventSignal(this->startEvent_);
	} else {
	  if ((status = checkConnected()) == asynSuccess) {
	    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Start Data Collection.\n", functionName);
	    getIntegerParam(xsp3NumCardsParam, &xsp3_num_cards);
	    for (int card=0; card<xsp3_num_cards; card++) {
	      xsp3_status = xsp3_histogram_start(xsp3_handle_, card);
	      if (xsp3_status != XSP3_OK) {
		checkStatus(xsp3_status, "xsp3_histogram_start", functionName);
		status = asynError;
	      }
	    }
	    if (status == asynSuccess) {
	      epicsEventSignal(this->startEvent_);
	    }
	  }
	}
      }
    } else {
      if (adStatus == ADStatusAcquire) {
	if (simTest_) {
	  epicsEventSignal(this->stopEvent_);
	} else {
	  if ((status = checkConnected()) == asynSuccess) {
	    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Stop Data Collection.\n", functionName);
	    getIntegerParam(xsp3NumCardsParam, &xsp3_num_cards);
	    for (int card=0; card<xsp3_num_cards; card++) {
	      xsp3_status = xsp3_histogram_stop(xsp3_handle_, card);
	      if (xsp3_status != XSP3_OK) {
		checkStatus(xsp3_status, "xsp3_histogram_stop", functionName);
		status = asynError;
	      }
	    }
	    if (status == asynSuccess) {
	      epicsEventSignal(this->stopEvent_);
	    }
	  }
	}
      }
    }
  } 
  else if (function == xsp3NumChannelsParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set Number Of Channels.\n", functionName);
    getIntegerParam(xsp3MaxNumChannelsParam, &xsp3_num_channels);
    if ((value > xsp3_num_channels) || (value < 1)) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Num Channels Out Of Range.\n", functionName);
      status = asynError;
    }
  }
  else if (function == xsp3TriggerModeParam) {
    if ((status = checkConnected()) == asynSuccess) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set Trigger Mode.\n", functionName);
      getIntegerParam(xsp3NumCardsParam, &xsp3_num_cards);
      for (int card=0; card<xsp3_num_cards; card++) {
	status = mapTriggerMode(value, &xsp3_trigger_mode);
	if (status != asynError) {
	  xsp3_status = xsp3_set_glob_timeA(xsp3_handle_, card, XSP3_GLOB_TIMA_TF_SRC(xsp3_trigger_mode));
	  if (xsp3_status != XSP3_OK) {
	    checkStatus(xsp3_status, "xsp3_set_glob_timeA", functionName);
	    status = asynError;
	  }
	}
      }
    }
  } 
  else if (function == xsp3FixedTimeParam) {
    if (!simTest_) {
      if ((status = checkConnected()) == asynSuccess) {
	getIntegerParam(xsp3NumCardsParam, &xsp3_num_cards);
	for (int card=0; card<xsp3_num_cards; card++) {
	  xsp3_status = xsp3_set_glob_timeFixed(xsp3_handle_, card, value);
	  if (xsp3_status != XSP3_OK) {
	    checkStatus(xsp3_status, "xsp3_set_glob_timeFixed", functionName);
	    status = asynError;
	  }
	}
      }
    }
  } 
  else if (function == ADNumImages) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set Number Of Frames To Read Out.\n", functionName);
    getIntegerParam(xsp3NumFramesConfigParam, &xsp3_time_frames);
    if (value > xsp3_time_frames) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Num Frames Higher Than Number Configured..\n", functionName);
      status = asynError;
    }
  }
  else if (function == xsp3NumFramesConfigParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set Number Of Frames For Intial Configuration.\n", functionName);
    getIntegerParam(xsp3MaxFramesParam, &xsp3_time_frames);
    if (value > xsp3_time_frames) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Num Frames For Config Too High.\n", functionName);
      status = asynError;
    }
  }
  else if (function == xsp3ConnectParam) {
    status = connect();
  }
  else if (function == xsp3DisconnectParam) {
    status = disconnect();
  }
  else if (function == xsp3SaveSettingsParam) {
    if ((adStatus != ADStatusAcquire) && (!simTest_)) {
      status = saveSettings();
      if (status == asynSuccess) {
	//Clear the save path so the user is forced to use a new path next time.
	setStringParam(xsp3ConfigSavePathParam, " ");
      }
    } else {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Saving Not Allowed In This Mode.\n", functionName);
      status = asynError;
    }
  }
  else if (function == xsp3RestoreSettingsParam) {
    if ((adStatus != ADStatusAcquire) && (!simTest_)) {
      status = restoreSettings();
    } else {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Restoring Not Allowed In This Mode.\n", functionName);
      status = asynError;
    }
  }
  else if (function == xsp3ChanSca4ThresholdParam) {
    if ((status = checkConnected()) == asynSuccess) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The SCA4 Threshold Register.\n", functionName);
      xsp3_status = xsp3_set_good_thres(xsp3_handle_, addr, value);
      if (xsp3_status != XSP3_OK) {
	checkStatus(xsp3_status, "xsp3_set_good_thres", functionName);
	status = asynError;
      }
    }
  } 
  else if (function == xsp3ChanSca5HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The SCA5 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanSca5LlmParam, &xsp3_sca_lim);
    status = setWindow(addr, 0, xsp3_sca_lim, value); 
  } 
  else if (function == xsp3ChanSca6HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The SCA6 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanSca6LlmParam, &xsp3_sca_lim);
    status = setWindow(addr, 1, xsp3_sca_lim, value); 
  } 
  else if (function == xsp3ChanSca5LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The SCA5 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanSca5HlmParam, &xsp3_sca_lim);
    status = setWindow(addr, 0, value, xsp3_sca_lim); 
  } 
  else if (function == xsp3ChanSca6LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The SCA6 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanSca6HlmParam, &xsp3_sca_lim);
    status = setWindow(addr, 1, value, xsp3_sca_lim);
  } 
  else if (function == xsp3ChanMcaRoi1LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI1 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi1HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 1, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi2LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI2 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi2HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 2, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi3LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI3 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi3HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 3, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi4LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI4 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi4HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 4, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi1HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI1 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi1LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 1, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi2HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI2 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi2LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 2, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi3HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI3 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi3LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 3, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi4HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI4 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi4LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 4, xsp3_roi_lim, value); 
  }
  else if (function == xsp3RoiEnableParam) {
    if (value == ctrlDisable_) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Disabling ROI Calculations.\n", functionName);
    } else if (value == ctrlEnable_) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Enabling ROI Calculations.\n", functionName);
    }
  } 
  else if (function == xsp3CtrlDataParam) {
    if (value == ctrlDisable_) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Disabling All Live Data Update.\n", functionName);
      setIntegerParam(xsp3CtrlMcaParam, 0);
      setIntegerParam(xsp3CtrlScaParam, 0);
    } else if (value == ctrlEnable_) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Enabling All Live Data Update.\n", functionName);
    }
  } 
  else if (function == xsp3CtrlMcaParam) {
    if (value == ctrlDisable_) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Disabling MCA Array Live Data Update.\n", functionName);
    } else if (value == ctrlEnable_) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Enabling MCA Array Live Data Update.\n", functionName);
      setIntegerParam(xsp3CtrlDataParam, 1);
    }
  } 
  else if (function == xsp3CtrlScaParam) {
    if (value == ctrlDisable_) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Disabling SCA Array Live Data Update.\n", functionName);
    } else if (value == ctrlEnable_) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Enabling SCA Array Live Data Update.\n", functionName);
      setIntegerParam(xsp3CtrlDataParam, 1);
    }
  } 
  else if (function == xsp3CtrlDataPeriodParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set SCA And ROI Data Update Rate.\n", functionName);
  }
  else if (function == xsp3CtrlMcaPeriodParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set MCA Array Data Update Rate.\n", functionName);
  }
  else if (function == xsp3CtrlScaPeriodParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set SCA Array Data Update Rate.\n", functionName);
  }
  else if (function == xsp3RunFlagsParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The Run Flags.\n", functionName);
  }
  else {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s No Matching Parameter In Xspress3 Driver.\n", functionName);
  }

  if (status != asynSuccess) {
    callParamCallbacks(addr);
    return asynError;
  }

  status = (asynStatus) setIntegerParam(addr, function, value);
  if (status!=asynSuccess) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, 
	      "%s Error Setting Parameter. Asyn addr: &d, asynUser->reason: &d, value: %d\n", 
	      functionName, addr, function, value);
    return(status);
  }

  //Do callbacks so higher layers see any changes 
  callParamCallbacks(addr);

  return status;
}

/**
 * Reimplementing this function from ADDriver to deal with floating point values.
 */ 
asynStatus Xspress3::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
  int function = pasynUser->reason;
  int addr = 0;
  asynStatus status = asynSuccess;
  const char *functionName = "Xspress3::writeFloat64";
  
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Calling writeFloat64.\n", functionName);
  //Read address (ie. channel number).
  status = getAddress(pasynUser, &addr); 
  if (status!=asynSuccess) {
    return(status);
  }
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s asynUser->reason: &d, value: %d, addr: %d\n", functionName, function, value, addr);

  //Set in param lib so the user sees a readback straight away. We might overwrite this in the 
  //status task, depending on the parameter.
  status = (asynStatus) setDoubleParam(function, value);

  //Do callbacks so higher layers see any changes 
  callParamCallbacks();

  return status;
}


/**
 * Reimplementing this function from asynNDArrayDriver to deal with strings.
 */
asynStatus Xspress3::writeOctet(asynUser *pasynUser, const char *value, 
                                    size_t nChars, size_t *nActual)
{
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;
    const char *functionName = "Xspress3::writeOctet";

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Calling writeOctet.\n", functionName);
    
    if (function == xsp3ConfigPathParam) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set Config Path Param.\n", functionName);
    } else if (function == xsp3ConfigSavePathParam) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set Config Save Path Param.\n", functionName);
      status = checkSaveDir(value);
    } else {
        /* If this parameter belongs to a base class call its method */
      if (function < FIRST_DRIVER_COMMAND) {
	status = asynNDArrayDriver::writeOctet(pasynUser, value, nChars, nActual);
      }
    }
    
    if (status != asynSuccess) {
      callParamCallbacks();
      return asynError;
    }
    
    /* Set the parameter in the parameter library. */
    status = (asynStatus)setStringParam(function, (char *)value);
    /* Do callbacks so higher layers see any changes */
    status = (asynStatus)callParamCallbacks();
    
    if (status) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, 
	      "%s Error Setting Parameter. asynUser->reason: &d\n", 
	      functionName, function);
    }

    *nActual = nChars;
    return status;
}

/**
 * Check if a save directory is empty. Reurn an error if not.
 * This is to prevent users overwriting existing condfig files.
 */
asynStatus Xspress3::checkSaveDir(const char *dirName) 
{
  asynStatus status = asynSuccess;
  int countFiles = 0;
  struct dirent *d = NULL;
  const char *functionName = "Xspress3::checkSaveDir";

  DIR *dir = opendir(dirName);
  if (dir == NULL) {
    //Directory doesn't exist.
    perror(functionName);
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Cannot Open Save Dir.\n", functionName);
    setStringParam(ADStatusMessage, "ERROR Opening Save Directory.");
    setIntegerParam(ADStatus, ADStatusError);
    status = asynError;
  }
  if (status != asynError) {
    while ((d = readdir(dir)) != NULL) {
      if (++countFiles > 2) {
	asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Files Already Exist In This Directory.\n", functionName);
	setStringParam(ADStatusMessage, "ERROR Files Already Exist In This Directory.");
	setIntegerParam(ADStatus, ADStatusError);
	status = asynError;
	break;
      }
    }
  }
  closedir(dir);

  if (status != asynError) {
    setStringParam(ADStatusMessage, "Set Save Directory.");
    setIntegerParam(ADStatus, ADStatusIdle);
  }

  return status;
}


/**
 * Wait until data collection has finished on all channels.
 * This function should only be called after stopping histogramming.
 * It is a wrapper for xsp3_histogram_is_busy, which it checks twice for all channels.
 * @param checkTimes How many times do we check the Xspress3 is not busy?
 * @return asynStatus - asynSuccess if we are not busy. asynError if we checked checkTimes times and we are still not busy.
 */ 
asynStatus Xspress3::checkHistBusy(int checkTimes) 
{
  asynStatus status = asynSuccess;
  int notBusyCount = 0;
  int notBusyChan = 0;
  int notBusyTotalCount = 0;
  int numChannels = 0;
  const char* functionName = "Xspress3::checkHistBusy";

  //Get the number of channels actually in use.
  getIntegerParam(xsp3NumChannelsParam, &numChannels);
 
  while (notBusyCount<2) {
    for (int chan=0; chan<numChannels; chan++) {
      if ((xsp3_histogram_is_busy(xsp3_handle_, chan)) == 0) {
	++notBusyChan;
      }
    }
    if (notBusyChan == numChannels) {
      ++notBusyCount;
    }
    notBusyChan = 0;
    notBusyTotalCount++;
    /////////TODO - check this works if we force xsp3_histogram_is_busy to fail.
    if (notBusyTotalCount==checkTimes) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: we polled xsp3_histogram_is_busy %d times. Giving up.\n", functionName, checkTimes);
      setStringParam(ADStatusMessage, "ERROR: Xspress3 Did Not Stop. Giving Up.");
      setIntegerParam(ADStatus, ADStatusError);
      status = asynError;
      break;
    }
  }

  return status;
}



/**
 * Data readout task.
 * Calculate statistics and post waveforms.
 */
void Xspress3::dataTask(void)
{
  epicsEventWaitStatus eventStatus;
  epicsFloat64 timeout = 0.01;
  int numChannels = 0;
  int numFrames = 0;
  int acquire = 0;
  int xsp3_status = 0;
  int status = 0;
  int xsp3_num_cards = 0;
  int frame_count = 0;
  int frameCounter = 0;
  int maxNumFrames = 0;
  int maxSpectra = 0;
  int dataTimeout = 0;
  int scaArrayTimeout = 0;
  int mcaArrayTimeout = 0;
  epicsTimeStamp startTimeData;
  epicsTimeStamp startTimeSca;
  epicsTimeStamp startTimeMca;
  epicsTimeStamp diffTime;
  epicsTimeStamp nowTime;
  int scalerUpdate = 0;
  int scalerArrayUpdate = 0;
  int mcaArrayUpdate = 0;
  double startTimeDataVal = 0.0;
  double startTimeScaVal = 0.0;
  double startTimeMcaVal = 0.0;
  double diffTimeDataVal = 0.0;
  double diffTimeScaVal = 0.0;
  double diffTimeMcaVal = 0.0;
  const char* functionName = "Xspress3::dataTask";
  epicsUInt32 *pSCA;
  epicsInt32 *pSCA_DATA[numChannels_][XSP3_SW_NUM_SCALERS];
  epicsFloat64 *pMCA[numChannels_];
  epicsFloat64 *pMCA_ROI[numChannels_][maxNumRoi_];
  NDArray *pMCA_NDARRAY;
  int roiEnabled = 0;
  epicsInt32 roiMin[maxNumRoi_];
  epicsInt32 roiMax[maxNumRoi_];
  epicsFloat64 roiSum[maxNumRoi_];
  int allocError = 0;
  int dumpOffset = 0;
  int lastFrameCount = 0;
  int framesToReadOut = 0;
  int stillBusy = 0;
 
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Started Data Thread.\n", functionName);

  //Create array for scalar data (max frame * number of SCAs).
  getIntegerParam(xsp3MaxFramesParam, &maxNumFrames);
  getIntegerParam(xsp3MaxSpectraParam, &maxSpectra);
  pSCA = static_cast<epicsUInt32*>(calloc(XSP3_SW_NUM_SCALERS*maxNumFrames*numChannels_, sizeof(epicsUInt32)));
  //Create array to hold SCA data for the duration of the scan, one per SCA, per channel.
  for (int chan=0; chan<numChannels_; chan++) {
    for (int sca=0; sca<XSP3_SW_NUM_SCALERS; sca++) {
      pSCA_DATA[chan][sca] = static_cast<epicsInt32*>(calloc(maxNumFrames, sizeof(epicsInt32)));
    }
  }

  //Create data arrays for MCA spectra and ROI arrays
  for (int chan=0; chan<numChannels_; chan++) {
    //We do this for each frame below, using this->pNDArrayPool->alloc, in the acquire loop.
    pMCA[chan] = static_cast<epicsFloat64*>(calloc(maxSpectra, sizeof(epicsFloat64))); 
    for (int roi=0; roi<maxNumRoi_; roi++) {
      pMCA_ROI[chan][roi] = static_cast<epicsFloat64*>(calloc(maxNumFrames, sizeof(epicsFloat64)));
    }
  }
  
  while (1) {
    
     eventStatus = epicsEventWait(startEvent_);          
     if (eventStatus == epicsEventWaitOK) {
       asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Got Start Event.\n", functionName);
       acquire = 1;
       frameCounter = 0;
       lock();
       //Need to clear local arrays here for each new acqusition.
       memset(pSCA, 0, (XSP3_SW_NUM_SCALERS*maxNumFrames*numChannels_)*sizeof(epicsUInt32));
       for (int chan=0; chan<numChannels_; chan++) {
	 memset(pMCA[chan], 0, maxSpectra*sizeof(epicsFloat64));
	 for (int sca=0; sca<XSP3_SW_NUM_SCALERS; sca++) {
	   memset(pSCA_DATA[chan][sca], 0, maxNumFrames*sizeof(epicsInt32));
	 }
	 for (int roi=0; roi<maxNumRoi_; roi++) {
	   memset(pMCA_ROI[chan][roi], 0, maxNumFrames*sizeof(epicsFloat64));
	 }
       }
       setIntegerParam(xsp3FrameCountParam, 0);
       setIntegerParam(NDArrayCounter, 0);
       setIntegerParam(ADStatus, ADStatusAcquire);
       setStringParam(ADStatusMessage, "Acquiring Data");
       callParamCallbacks();
     }
     
     dumpOffset = 0;
     lastFrameCount = 0;
     framesToReadOut = 0;
     stillBusy = 0;

     /* Get the current time */
     epicsTimeGetCurrent(&startTimeData);
     epicsTimeGetCurrent(&startTimeSca);
     epicsTimeGetCurrent(&startTimeMca);
     

     //Get the number of channels actually in use.
     getIntegerParam(xsp3NumChannelsParam, &numChannels);
     //Read how many frames we want to read out before stopping.
     getIntegerParam(ADNumImages, &numFrames);

     while (acquire) {
       setIntegerParam(ADStatus, ADStatusAcquire);
       unlock();

       //Wait for a stop event, with a short timeout.
       eventStatus = epicsEventWaitWithTimeout(stopEvent_, timeout);          
       if (eventStatus == epicsEventWaitOK) {
	 asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Got Stop Event.\n", functionName);
	 acquire = 0;
       }
       lock();
       
       if (acquire==0) {
	 setIntegerParam(ADAcquire, 0);
	 setIntegerParam(ADStatus, ADStatusAborted);
	 setStringParam(ADStatusMessage, "Stopped Acquiring");
	 callParamCallbacks();
       }

       //If we have stopped, wait until we are not busy on all channels.
       stillBusy = 0;
       if (!simTest_) {
	 if (acquire == 0) {
	   if (checkHistBusy(20) == asynError) {
	     stillBusy = 1;
	   }
	 }
       }

       //Read how many data frames have been transferred.
       if (!simTest_) {
	 getIntegerParam(xsp3NumCardsParam, &xsp3_num_cards);
	 for (int card=0; card<xsp3_num_cards; card++) {
           xsp3_status = xsp3_scaler_check_desc(xsp3_handle_, card);
	   if (xsp3_status < XSP3_OK) {
	     checkStatus(xsp3_status, "xsp3_dma_check_desc", functionName);
	     status = asynError;
	   }
	   frame_count = xsp3_status;
	   asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s frame_count: %d.\n", functionName, frame_count);
	   ///////////////////////////////////////hack, until xsp3_dma_check_desc is fixed///////////////////////////////
	   //if (acquire == 0) {
	   //  frame_count = numFrames;
	   //}
	   //////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 }
       } else {
	 //In sim mode we transfer 10 frame each time
	 frame_count = lastFrameCount+10;
       }
       
       if ((!acquire) && (stillBusy == 1)) {
	 frame_count = 0;
	 framesToReadOut = 0;
	 callParamCallbacks();
       }

       //Take the value from the last card for now
       setIntegerParam(xsp3FrameCountParam, frame_count);
       
       if (frame_count > lastFrameCount) {
	 framesToReadOut = frame_count - lastFrameCount;
	 lastFrameCount = frame_count;

	 getIntegerParam(NDArrayCounter, &frameCounter);
	 frameCounter += framesToReadOut;
	 int remainingFrames = framesToReadOut;
	 //Check we are not overflowing or reading too many frames.
	 if (frameCounter >= maxNumFrames) {
	   remainingFrames = maxNumFrames - (frameCounter - framesToReadOut);
	   asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Stopping Acqusition. We Reached The Max Num Of Frames.\n", functionName);
	   setStringParam(ADStatusMessage, "Stopped. Max Frames Reached.");
	   setIntegerParam(ADAcquire, 0);
	   acquire=0;
	   setIntegerParam(ADStatus, ADStatusAborted);
	 } else if (frameCounter >= numFrames) {
	   remainingFrames = numFrames - (frameCounter - framesToReadOut);
	   setStringParam(ADStatusMessage, "Completed Acqusition.");
	   setIntegerParam(ADAcquire, 0);
	   setIntegerParam(ADStatus, ADStatusIdle);
	   acquire=0;
	 }

	 int frameOffset = frameCounter-framesToReadOut;
	 if (acquire==0) {
	   frameCounter = frameOffset+remainingFrames;
	 }

	 asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s frame_count: %d.\n", functionName, frame_count);
	 asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s framesToReadOut: %d.\n", functionName, framesToReadOut);
	 asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s frameCounter: %d.\n", functionName, frameCounter);
	 asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s remainingFrames: %d.\n", functionName, remainingFrames);
	 asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s frameOffset: %d.\n", functionName, frameOffset);

	 //epicsThreadSleep(0.05);
	 epicsUInt32 *pData = NULL;
	 //Readout multiple frames of scaler data here into local array.
	 if (!stillBusy) {
	   if (!simTest_) {
	     pData = pSCA+(frameOffset*(XSP3_SW_NUM_SCALERS * numChannels));
	     xsp3_status = xsp3_scaler_read(xsp3_handle_, pData, 0, 0, frameOffset, XSP3_SW_NUM_SCALERS, numChannels, remainingFrames);
	     if (xsp3_status < XSP3_OK) {
	       checkStatus(xsp3_status, "xsp3_scaler_read", functionName);
	       //What to do here?
	     }
	   } else {
	     //Fill the array with dummy data in simTest_ mode
	     pData = pSCA;
	     for (int i=0; i<(XSP3_SW_NUM_SCALERS*maxNumFrames*numChannels); ++i) {
	       *(pData++) = i;
	     }
	   }
	 }

	 //Dump data for testing
	 epicsUInt32 *pDumpData = pSCA;
	 for (int frame=frameOffset; frame<frameCounter; frame++) {
	   for (int chan=0; chan<numChannels; ++chan) {
	     for (int sca=0; sca<XSP3_SW_NUM_SCALERS; sca++) {
	       cout << " frame:" << frame << " chan:" << chan << " sca:" << sca << " data[" << dumpOffset << "]:" << *(pDumpData+dumpOffset) << endl;
	       ++dumpOffset;
	     }
	   }
	 }

	 int dims[2] = {maxSpectra, numChannels};
	 epicsUInt32 *pScaData = pSCA+(frameOffset*numChannels*XSP3_SW_NUM_SCALERS);
	 epicsInt32 *pScaDataArray = NULL;
	 
	 //For each frame, read out the MCA and copy the MCA and SCA data into local arrays for channel access, and pack into a NDArray.
	 if (!stillBusy) {
	   for (int frame=frameOffset; frame<(frameOffset+remainingFrames); ++frame) {
	     
	     allocError == 0;
	     setIntegerParam(NDArrayCounter, frame+1);
	     
	     asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Frame: %d.\n", functionName, frame);
	     
	     //NDArray to hold the all channels spectra and attributes for each frame.
	     if ((pMCA_NDARRAY = this->pNDArrayPool->alloc(2, dims, NDFloat64, 0, NULL)) == NULL) {
	       asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s: ERROR: pNDArrayPool->alloc failed.\n", functionName);
	       setStringParam(ADStatusMessage, "Memory Error. Check IOC Log.");
	       setIntegerParam(ADStatus, ADStatusError);
	       setIntegerParam(ADAcquire, 0);
	       acquire = 0;
	       allocError = 1;
	     } else {
	       for (int chan=0; chan<numChannels; ++chan) {
		 if (!simTest_) {
		   //Read out the MCA spectra for this channel.
		   xsp3_status = xsp3_histogram_read_chan(xsp3_handle_, reinterpret_cast<u_int32_t*>(pMCA[chan]), chan, 0, 0, frame, 1, 1, 1);
		   if (xsp3_status != XSP3_OK) {
		     checkStatus(xsp3_status, "xsp3_histogram_read_chan", functionName);
		     status = asynError;
		   }
		 } else {
		   //If in simMode_, fill the MCA array with sim data for this channel.
		   epicsFloat64 *pMCA_DATA = pMCA[chan];
		   for (int i=0; i<maxSpectra; ++i) {
		     //Generate a sine wave, with an element of randomness in it.
		     *(pMCA_DATA++) = sin(static_cast<epicsFloat64>(i)/90.0)*((rand()%20)+100);
		   }
		 }
		 //For this channel and frame, copy the scaler data into pSCA_DATA for channel access later on.
		 for (int sca=0; sca<XSP3_SW_NUM_SCALERS; ++sca) {
		   pScaDataArray = (static_cast<epicsInt32*>((pSCA_DATA[chan][sca])+frame));
		   *pScaDataArray = *(pScaData++);
		 }
		 
		 setIntegerParam(chan, xsp3ChanSca0Param, *((pSCA_DATA[chan][0])+frame));
		 setIntegerParam(chan, xsp3ChanSca1Param, *((pSCA_DATA[chan][1])+frame));
		 setIntegerParam(chan, xsp3ChanSca2Param, *((pSCA_DATA[chan][2])+frame));
		 setIntegerParam(chan, xsp3ChanSca3Param, *((pSCA_DATA[chan][3])+frame));
		 setIntegerParam(chan, xsp3ChanSca4Param, *((pSCA_DATA[chan][4])+frame));
		 setIntegerParam(chan, xsp3ChanSca5Param, *((pSCA_DATA[chan][5])+frame));
		 setIntegerParam(chan, xsp3ChanSca6Param, *((pSCA_DATA[chan][6])+frame));
		 setIntegerParam(chan, xsp3ChanSca7Param, *((pSCA_DATA[chan][7])+frame));
		 
		 //Calculate MCA ROI here, if we have enabled it. Put the results into pMCA_ROI[chan][roi].
		 getIntegerParam(xsp3RoiEnableParam, &roiEnabled);
		 if (roiEnabled) {
		   
		   getIntegerParam(chan, xsp3ChanMcaRoi1LlmParam, &roiMin[0]);
		   getIntegerParam(chan, xsp3ChanMcaRoi1HlmParam, &roiMax[0]);
		   getIntegerParam(chan, xsp3ChanMcaRoi2LlmParam, &roiMin[1]);
		   getIntegerParam(chan, xsp3ChanMcaRoi2HlmParam, &roiMax[1]);
		   getIntegerParam(chan, xsp3ChanMcaRoi3LlmParam, &roiMin[2]);
		   getIntegerParam(chan, xsp3ChanMcaRoi3HlmParam, &roiMax[2]);
		   getIntegerParam(chan, xsp3ChanMcaRoi4LlmParam, &roiMin[3]);
		   getIntegerParam(chan, xsp3ChanMcaRoi4HlmParam, &roiMax[3]);
		   
		   asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Calculating ROI Data.\n", functionName);
		   memset(&roiSum, 0, sizeof(epicsFloat64)*maxNumRoi_);
		   for (int energy=0; energy<maxSpectra; ++energy) {
		     for (int roi=0; roi<maxNumRoi_; ++roi) {
		       if ((energy>=roiMin[roi]) && (energy<roiMax[roi])) {
			 roiSum[roi] += *(pMCA[chan]+energy);
		       }
		     }
		   }
		   for (int roi=0; roi<maxNumRoi_; ++roi) {
		     asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Chan %d Roi %d: %f.\n", functionName, chan, roi, roiSum[roi]);
		     *(pMCA_ROI[chan][roi]+frame) = roiSum[roi];
		   }
		 }
		 
		 setDoubleParam(chan, xsp3ChanMcaRoi1Param, *((pMCA_ROI[chan][0])+frame));
		 setDoubleParam(chan, xsp3ChanMcaRoi2Param, *((pMCA_ROI[chan][1])+frame));
		 setDoubleParam(chan, xsp3ChanMcaRoi3Param, *((pMCA_ROI[chan][2])+frame));
		 setDoubleParam(chan, xsp3ChanMcaRoi4Param, *((pMCA_ROI[chan][3])+frame));
		 
	       } //End of channel loop
	     } //end of else, if the alloc worked.
	   
	     //Pack the MCA data into an NDArray for this frame.   Format is: [chan1 spectra][chan2 spectra][etc]
	     if (allocError == 0) {
	       for (int chan=0; chan<numChannels; ++chan) {
		 memcpy(reinterpret_cast<epicsFloat64*>(pMCA_NDARRAY->pData)+(chan*maxSpectra), pMCA[chan], maxSpectra*sizeof(epicsFloat64));
	       }
	       
	       int arrayCallbacks = 0;
	       getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
	       //Do callbacks on NDArray for plugins.
	       epicsTimeGetCurrent(&nowTime);
	       pMCA_NDARRAY->uniqueId = frame;
	       pMCA_NDARRAY->timeStamp = nowTime.secPastEpoch + nowTime.nsec / 1.e9;
	       pMCA_NDARRAY->pAttributeList->add("TIMESTAMP", "Host Timestamp", NDAttrFloat64, &(pMCA_NDARRAY->timeStamp));
	       this->getAttributes(pMCA_NDARRAY->pAttributeList);
	       if (arrayCallbacks) {
		 unlock();
		 asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s: Calling NDArray callback\n", functionName);
		 doCallbacksGenericPointer(pMCA_NDARRAY, NDArrayData, 0);
		 lock();
	       }
	       //Free the NDArray 
	       pMCA_NDARRAY->release();
	     } //end of if (allocError == 0)
	     
	   } //end of frame loop
	 } //end of if (!stillBusy)
	   
	 //Control the update rate of scalers and arrays over channel access.
	 getIntegerParam(xsp3CtrlDataParam, &scalerUpdate); 
	 getIntegerParam(xsp3CtrlScaParam, &scalerArrayUpdate); 
	 getIntegerParam(xsp3CtrlMcaParam, &mcaArrayUpdate); 
	 getIntegerParam(xsp3CtrlDataPeriodParam, &dataTimeout);
	 getIntegerParam(xsp3CtrlScaPeriodParam, &scaArrayTimeout);
	 getIntegerParam(xsp3CtrlMcaPeriodParam, &mcaArrayTimeout);

	 epicsTimeGetCurrent(&diffTime);

	 startTimeDataVal = startTimeData.secPastEpoch + startTimeData.nsec / 1.e9;
	 startTimeScaVal = startTimeSca.secPastEpoch + startTimeSca.nsec / 1.e9;
	 startTimeMcaVal = startTimeMca.secPastEpoch + startTimeMca.nsec / 1.e9;
	 diffTimeDataVal = (diffTime.secPastEpoch + diffTime.nsec / 1.e9) - startTimeDataVal;
	 diffTimeScaVal = (diffTime.secPastEpoch + diffTime.nsec / 1.e9) - startTimeScaVal;
	 diffTimeMcaVal = (diffTime.secPastEpoch + diffTime.nsec / 1.e9) - startTimeMcaVal;

	 if (diffTimeDataVal < (dataTimeout/1000.0)) {
	   scalerUpdate = 0;
	 } else {
	   epicsTimeGetCurrent(&startTimeData);
	 }
	 if (diffTimeScaVal < (scaArrayTimeout/1000.0)) {
	   scalerArrayUpdate = 0;
	 } else {
	   epicsTimeGetCurrent(&startTimeSca);
	 }
	 if (diffTimeMcaVal < (mcaArrayTimeout/1000.0)) {
	   mcaArrayUpdate = 0;
	 } else {
	   epicsTimeGetCurrent(&startTimeMca);
	 }
	 
	 int offset = frameCounter;
	 //Post scalar array data if we have enabled it and the timer has expired, or if we have stopped acquiring. 
	 if ((scalerArrayUpdate == 1) || !acquire) {
	   for (int chan=0; chan<numChannels; ++chan) {
	     asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Posting Scaler Arrays On Chan %d.\n", functionName, chan);
	     doCallbacksInt32Array(pSCA_DATA[chan][0], offset, xsp3ChanSca0ArrayParam, chan);
	     doCallbacksInt32Array(pSCA_DATA[chan][1], offset, xsp3ChanSca1ArrayParam, chan);
	     doCallbacksInt32Array(pSCA_DATA[chan][2], offset, xsp3ChanSca2ArrayParam, chan);
	     doCallbacksInt32Array(pSCA_DATA[chan][3], offset, xsp3ChanSca3ArrayParam, chan);
	     doCallbacksInt32Array(pSCA_DATA[chan][4], offset, xsp3ChanSca4ArrayParam, chan);
	     doCallbacksInt32Array(pSCA_DATA[chan][5], offset, xsp3ChanSca5ArrayParam, chan);
	     doCallbacksInt32Array(pSCA_DATA[chan][6], offset, xsp3ChanSca6ArrayParam, chan);
	     doCallbacksInt32Array(pSCA_DATA[chan][7], offset, xsp3ChanSca7ArrayParam, chan);
	     if (roiEnabled) {
	       asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Posting ROI Arrays On Chan %d.\n", functionName, chan);
	       doCallbacksFloat64Array(pMCA_ROI[chan][0], offset, xsp3ChanMcaRoi1ArrayParam, chan);
	       doCallbacksFloat64Array(pMCA_ROI[chan][1], offset, xsp3ChanMcaRoi2ArrayParam, chan);
	       doCallbacksFloat64Array(pMCA_ROI[chan][2], offset, xsp3ChanMcaRoi3ArrayParam, chan);
	       doCallbacksFloat64Array(pMCA_ROI[chan][3], offset, xsp3ChanMcaRoi4ArrayParam, chan);
	     }
	   }
	 }

	 //Post the most recent MCA array data if we have enabled it and the timer has expired, or if we have stopped acquiring.
	 if ((mcaArrayUpdate == 1) || !acquire) {
	   for (int chan=0; chan<numChannels; ++chan) {
	     asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Posting MCA Arrays On Chan %d.\n", functionName, chan);
	     doCallbacksFloat64Array(pMCA[chan], maxSpectra, xsp3ChanMcaCorrParam, chan);
	   }
	 }

	 //If we are updating scaler data, or have stopped acquiring, do channel access.
	 if (scalerUpdate || !acquire) {
	   //Need to call callParamCallbacks for each list (ie. channel, indexed by Asyn address).
	   for (int addr=0; addr<maxAddr; addr++) {
	     asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Calling callParamCallbacks for addr %d.\n", functionName, addr);
	     callParamCallbacks(addr);
	   }
	 }

       }  //end of if (frame_count > lastFrameCount)

       frame_count = 0;
       framesToReadOut = 0;

     } //end of while(acquire)

     unlock();

  } //end of while(1)

  asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s: ERROR: Exiting dataTask main loop.\n", functionName);
  free(pSCA);
  for (int chan=0; chan<numChannels_; chan++) {
    for (int sca=0; sca<XSP3_SW_NUM_SCALERS; sca++) {
      free(pSCA_DATA[chan][sca]);
    }
  }
  for (int chan=0; chan<numChannels_; chan++) {
    free(pMCA[chan]); 
    for (int roi=0; roi<maxNumRoi_; roi++) {
      free(pMCA_ROI[chan][roi]);
    }
  }

}


//Global C utility functions to tie in with EPICS
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
 * @param numChannels The max number of channels (eg. 8)
 * @param numCards The number of Xspress3 systems (normally 1)
 * @param baseIP The base address used by the Xspress3 1Gig and 10Gig interfaces (eg. "192.168.0.1")
 * @param maxFrames The maximum number of frames that can be acquired in one acquisition (eg. 16384) 
 * @param maxSpectra The maximum size of each spectra (eg. 4096)
 * @param maxBuffers Used by asynPortDriver (set to -1 for unlimited)
 * @param maxMemory Used by asynPortDriver (set to -1 for unlimited)
 * @param debug This debug flag is passed to xsp3_config in the Xspress API (0 or 1)
 * @param simTest 0 or 1. Set to 1 to run up this driver in simulation mode. 
 */
extern "C" {


  int xspress3Config(const char *portName, int numChannels, int numCards, const char *baseIP, int maxFrames, int maxSpectra, int maxBuffers, size_t maxMemory, int debug, int simTest)
  {
    asynStatus status = asynSuccess;
    
    /*Instantiate class.*/
    try {
      new Xspress3(portName, numChannels, numCards, baseIP, maxFrames, maxSpectra, maxBuffers, maxMemory, debug, simTest);
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
  static const iocshArg xspress3ConfigArg2 = {"Num Cards", iocshArgInt};
  static const iocshArg xspress3ConfigArg3 = {"Base IP Address", iocshArgString};
  static const iocshArg xspress3ConfigArg4 = {"Max Frames", iocshArgInt};
  static const iocshArg xspress3ConfigArg5 = {"Max Spectra", iocshArgInt};
  static const iocshArg xspress3ConfigArg6 = {"Max Buffers", iocshArgInt};
  static const iocshArg xspress3ConfigArg7 = {"Max Memory", iocshArgInt};
  static const iocshArg xspress3ConfigArg8 = {"Debug", iocshArgInt};
  static const iocshArg xspress3ConfigArg9 = {"Sim Test", iocshArgInt};
  static const iocshArg * const xspress3ConfigArgs[] =  {&xspress3ConfigArg0,
							 &xspress3ConfigArg1,
							 &xspress3ConfigArg2,
							 &xspress3ConfigArg3,
							 &xspress3ConfigArg4,
							 &xspress3ConfigArg5,
							 &xspress3ConfigArg6,
							 &xspress3ConfigArg7,
							 &xspress3ConfigArg8,
							 &xspress3ConfigArg9};
  
  static const iocshFuncDef configXspress3 = {"xspress3Config", 10, xspress3ConfigArgs};
  static void configXspress3CallFunc(const iocshArgBuf *args)
  {
    xspress3Config(args[0].sval, args[1].ival, args[2].ival, args[3].sval, args[4].ival, args[5].ival, args[6].ival, args[7].ival, args[8].ival, args[9].ival);
  }
  
  static void xspress3Register(void)
  {
    iocshRegister(&configXspress3, configXspress3CallFunc);
  }
  
  epicsExportRegistrar(xspress3Register);

} // extern "C"


/****************************************************************************************/

