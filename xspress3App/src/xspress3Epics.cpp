/**
 * Author: Diamond Light Source, Copyright 2013
 *
 * License: This file is part of 'xspress3'
 *
 * 'xspress3' is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 'xspress3' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with 'xspress3'.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @brief Asyn port driver for the Xspress3 system.
 *
 * @author Matthew Pearson
 * @date Sept 2012
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include "dirent.h"
#include <sys/types.h>
#include <syscall.h>
#include <stdarg.h>

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
const epicsInt32 Xspress3::maxStringSize_ = 256;
const epicsInt32 Xspress3::maxCheckHistPolls_ = 20;
const epicsInt32 Xspress3::mbboTriggerFIXED_ = 0;
const epicsInt32 Xspress3::mbboTriggerINTERNAL_ = 1;
const epicsInt32 Xspress3::mbboTriggerIDC_ = 2;
const epicsInt32 Xspress3::mbboTriggerTTLVETO_ = 3;
const epicsInt32 Xspress3::mbboTriggerTTLBOTH_ = 4;
const epicsInt32 Xspress3::mbboTriggerLVDSVETO_ = 5;
const epicsInt32 Xspress3::mbboTriggerLVDSBOTH_ = 6;
const epicsInt32 Xspress3::ADAcquireFalse_ = 0;
const epicsInt32 Xspress3::ADAcquireTrue_ = 1;

const int INTERFACE_MASK = asynInt32Mask | asynInt32ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynFloat64ArrayMask | asynDrvUserMask | asynOctetMask | asynGenericPointerMask;
const int INTERRUPT_MASK = asynInt32Mask | asynInt32ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynFloat64ArrayMask | asynOctetMask | asynGenericPointerMask;

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
Xspress3::Xspress3(const char *portName, int numChannels, int numCards, const char *baseIP, int maxFrames, int maxDriverFrames, int maxSpectra, int maxBuffers, size_t maxMemory, int debug, int simTest)
  : ADDriver(portName,
	     numChannels, /* maxAddr - channels use different param lists*/
	     NUM_DRIVER_PARAMS,
	     maxBuffers,
	     maxMemory,
	     INTERFACE_MASK,
	     INTERRUPT_MASK,
	     ASYN_CANBLOCK | ASYN_MULTIDEVICE, /* asynFlags.*/
	     1, /* Autoconnect */
	     0, /* default priority */
	     0), /* Default stack size*/
    debug_(debug), numChannels_(numChannels), simTest_(simTest), baseIP_(baseIP)
{
  int status = asynSuccess;
  const char *functionName = "Xspress3::Xspress3";

  //strncpy(baseIP_, baseIP, 16);
  //baseIP_[16] = '\0';

  //Create the epicsEvent for signaling to the status task when parameters should have changed.
  //This will cause it to do a poll immediately, rather than wait for the poll time period.
  startEvent_ = epicsEventMustCreate(epicsEventEmpty);
  if (!startEvent_) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s epicsEventCreate failure for start event.\n", functionName);
    return;
  }
  stopEvent_ = epicsEventMustCreate(epicsEventEmpty);
  if (!stopEvent_) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s epicsEventCreate failure for start event.\n", functionName);
    return;
  }
  this->createInitialParameters();
  //Initialize non static, non const, data members
  xsp3_handle_ = 0;
  bool paramStatus = this->setInitialParameters(maxFrames, maxDriverFrames, numCards, maxSpectra);
  paramStatus = ((eraseSCA() == asynSuccess) && paramStatus);
  //Create the thread that readouts the data
  status = (epicsThreadCreate("GeDataTask",
                              epicsThreadPriorityHigh,
                              epicsThreadGetStackSize(epicsThreadStackMedium),
                              (EPICSTHREADFUNC)xsp3DataTaskC,
                              this) == NULL);
  if (status) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s epicsThreadCreate failure for data task.\n", functionName);
    return;
  }

  printf( "Simulation: %d\n", simTest_ );
  if (simTest_) {
    paramStatus = ((setStringParam(ADStatusMessage, "Init. Simulation Mode.") == asynSuccess) && paramStatus);
    xsp3 = new xsp3Simulator(this->pasynUserSelf,numChannels,maxSpectra);
  } else {
    paramStatus = ((setStringParam(ADStatusMessage, "Init. System Disconnected.") == asynSuccess) && paramStatus);
    xsp3 = new xsp3Detector(this->pasynUserSelf);
  }

  callParamCallbacks();

  if (!paramStatus) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s Unable To Set Driver Parameters In Constructor.\n", functionName);
  }

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s End Of Constructor.\n", functionName);

}

 /**
 * A constructor for the test suite.
 *
 * This constructor defines a lot of parameters in it for ease of testing.
 *
 * @param portName The asyn port name, this must be unique to each instance.
 * @param numChannels The number of channels to simulate.
 *
 */
Xspress3::Xspress3(const char *portName, int numChannels) : ADDriver(portName, numChannels, NUM_DRIVER_PARAMS, -1, -1, INTERFACE_MASK, INTERRUPT_MASK, ASYN_CANBLOCK | ASYN_MULTIDEVICE, 1, 0, 0), debug_(1), numChannels_(numChannels), simTest_(1), baseIP_("127.0.0.1")
{
    const char *functionName = "Xspress3::Xspress3";
    const int maxFrames = 1000;
    const int maxDriverFrames = 1000;
    const int maxSpectra = 4096;
    const int numCards = 1;
    const int simTest = 1;
    this->lock();
    this->createInitialParameters();
    //Initialize non static, non const, data members
    xsp3_handle_ = 0;
    bool paramStatus = this->setInitialParameters(maxFrames, maxDriverFrames, numCards, maxSpectra);
    paramStatus = ((eraseSCA() == asynSuccess) && paramStatus);
    if (simTest) {
        paramStatus = ((setStringParam(ADStatusMessage, "Init. Simulation Mode.") == asynSuccess) && paramStatus);
        xsp3 = new xsp3Simulator(this->pasynUserSelf,numChannels,maxSpectra);
    } else {
        paramStatus = ((setStringParam(ADStatusMessage, "Init. System Disconnected.") == asynSuccess) && paramStatus);
        xsp3 = new xsp3Detector(this->pasynUserSelf);
    }

    callParamCallbacks();

    if (!paramStatus) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s Unable To Set Driver Parameters In Constructor.\n", functionName);
    }

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s End Of Constructor.\n", functionName);

}

/**
 * Create the asyn parameters required to set up and control the xspress3.
 *
 * This method is normally called by the constructor.
 */
void Xspress3::createInitialParameters()
{
    //Add the params to the paramLib
    //createParam adds the parameters to all param lists automatically (using maxAddr).
    createParam(xsp3FirstParamString, asynParamInt32, &xsp3FirstParam);
    createParam(xsp3ResetParamString, asynParamInt32, &xsp3ResetParam);
    createParam(xsp3EraseParamString, asynParamInt32, &xsp3EraseParam);
    createParam(xsp3NumChannelsParamString, asynParamInt32, &xsp3NumChannelsParam);
    createParam(xsp3MaxNumChannelsParamString, asynParamInt32, &xsp3MaxNumChannelsParam);
    createParam(xsp3MaxSpectraParamString, asynParamInt32, &xsp3MaxSpectraParam);
    createParam(xsp3MaxFramesParamString, asynParamInt32, &xsp3MaxFramesParam);
    createParam(xsp3FrameCountParamString, asynParamInt32, &xsp3FrameCountParam);
    createParam(xsp3TriggerModeParamString, asynParamInt32, &xsp3TriggerModeParam);
    createParam(xsp3FixedTimeParamString, asynParamInt32, &xsp3FixedTimeParam);
    createParam(xsp3NumFramesConfigParamString, asynParamInt32, &xsp3NumFramesConfigParam);
    createParam(xsp3NumFramesDriverParamString, asynParamInt32, &xsp3NumFramesDriverParam);
    createParam(xsp3NumCardsParamString, asynParamInt32, &xsp3NumCardsParam);
    createParam(xsp3ConfigPathParamString, asynParamOctet, &xsp3ConfigPathParam);
    createParam(xsp3ConfigSavePathParamString, asynParamOctet, &xsp3ConfigSavePathParam);
    createParam(xsp3ConnectParamString, asynParamInt32, &xsp3ConnectParam);
    createParam(xsp3ConnectedParamString, asynParamInt32, &xsp3ConnectedParam);
    createParam(xsp3DisconnectParamString, asynParamInt32, &xsp3DisconnectParam);
    createParam(xsp3SaveSettingsParamString, asynParamInt32, &xsp3SaveSettingsParam);
    createParam(xsp3RestoreSettingsParamString, asynParamInt32, &xsp3RestoreSettingsParam);

    createParam(xsp3RunFlagsParamString, asynParamInt32, &xsp3RunFlagsParam);
    createParam(xsp3TriggerParamString, asynParamInt32, &xsp3TriggerParam);
    createParam(xsp3InvertF0ParamString, asynParamInt32, &xsp3InvertF0Param);
    createParam(xsp3InvertVetoParamString, asynParamInt32, &xsp3InvertVetoParam);
    createParam(xsp3DebounceParamString, asynParamInt32, &xsp3DebounceParam);
    // itfg
    createParam(xsp3PulsePerTriggerParamString, asynParamInt32, &xsp3PulsePerTriggerParam);
    createParam(xsp3ITFGStartParamString, asynParamInt32, &xsp3ITFGStartParam);
    createParam(xsp3ITFGStopParamString, asynParamInt32, &xsp3ITFGStopParam);

    createParam(xsp3ChanSca0ParamString, asynParamFloat64, &xsp3ChanSca0Param);
    createParam(xsp3ChanSca1ParamString, asynParamFloat64, &xsp3ChanSca1Param);
    createParam(xsp3ChanSca2ParamString, asynParamFloat64, &xsp3ChanSca2Param);
    createParam(xsp3ChanSca3ParamString, asynParamFloat64, &xsp3ChanSca3Param);
    createParam(xsp3ChanSca4ParamString, asynParamFloat64, &xsp3ChanSca4Param);
    createParam(xsp3ChanSca5ParamString, asynParamFloat64, &xsp3ChanSca5Param);
    createParam(xsp3ChanSca6ParamString, asynParamFloat64, &xsp3ChanSca6Param);
    createParam(xsp3ChanSca7ParamString, asynParamFloat64, &xsp3ChanSca7Param);
    createParam(xsp3ChanSca4ThresholdParamString, asynParamInt32, &xsp3ChanSca4ThresholdParam);
    createParam(xsp3ChanSca5HlmParamString, asynParamInt32, &xsp3ChanSca5HlmParam);
    createParam(xsp3ChanSca6HlmParamString, asynParamInt32, &xsp3ChanSca6HlmParam);
    createParam(xsp3ChanSca5LlmParamString, asynParamInt32, &xsp3ChanSca5LlmParam);
    createParam(xsp3ChanSca6LlmParamString, asynParamInt32, &xsp3ChanSca6LlmParam);
    createParam(xsp3ChanDtcFlagsParamString, asynParamInt32, &xsp3ChanDtcFlagsParam);
    createParam(xsp3ChanDtcAegParamString, asynParamFloat64, &xsp3ChanDtcAegParam);
    createParam(xsp3ChanDtcAeoParamString, asynParamFloat64, &xsp3ChanDtcAeoParam);
    createParam(xsp3ChanDtcIwgParamString, asynParamFloat64, &xsp3ChanDtcIwgParam);
    createParam(xsp3ChanDtcIwoParamString, asynParamFloat64, &xsp3ChanDtcIwoParam);
    //These controls calculations
    createParam(xsp3DtcEnableParamString, asynParamInt32, &xsp3DtcEnableParam);
    createParam(xsp3EventWidthParamString, asynParamFloat64, &xsp3EventWidthParam);
    createParam(xsp3ChanDTPercentParamString, asynParamFloat64, &xsp3ChanDTPercentParam);
    createParam(xsp3ChanDTFactorParamString, asynParamFloat64, &xsp3ChanDTFactorParam);
    createParam(xsp3LastParamString, asynParamInt32, &xsp3LastParam);
}

/**
 * Set the parameters created in Xspress3::createInitialParameters
 *
 * @param maxFrames
 * @param maxDriverFrames
 * @param numCards
 * @param maxSpectra
 *
 * @return
 */
bool Xspress3::setInitialParameters(int maxFrames, int maxDriverFrames, int numCards, int maxSpectra)
{
    bool paramStatus = true;
    //Initialise any paramLib parameters that need passing up to device support
    paramStatus = ((setIntegerParam(xsp3NumChannelsParam, numChannels_) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3MaxNumChannelsParam, numChannels_) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3TriggerModeParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3FixedTimeParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(ADNumImages, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3NumFramesConfigParam, maxFrames) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3NumFramesDriverParam, maxDriverFrames) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3MaxSpectraParam, maxSpectra) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3MaxFramesParam, maxFrames) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3NumCardsParam, numCards) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3FrameCountParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(ADStatus, ADStatusDisconnected) == asynSuccess) && paramStatus);
    paramStatus = ((setStringParam (ADManufacturer, "Quantum Detectors") == asynSuccess) && paramStatus);
    paramStatus = ((setStringParam (ADModel, "Xspress3") == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(ADMaxSizeX, maxSpectra) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(ADMaxSizeY, numChannels_) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(ADSizeX, maxSpectra) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(ADSizeY, numChannels_) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(NDArraySizeX, maxSpectra) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(NDArraySizeY, numChannels_) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(NDArraySize, (maxSpectra * numChannels_ * sizeof(double)))  == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3TriggerParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3InvertF0Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3InvertVetoParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3DebounceParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3PulsePerTriggerParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3ITFGStartParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(xsp3ITFGStopParam, 0) == asynSuccess) && paramStatus);

    for (int chan=0; chan<numChannels_; chan++) {
        paramStatus = ((setIntegerParam(chan, xsp3ChanSca4ThresholdParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setIntegerParam(chan, xsp3ChanSca5HlmParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setIntegerParam(chan, xsp3ChanSca6HlmParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setIntegerParam(chan, xsp3ChanSca5LlmParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setIntegerParam(chan, xsp3ChanSca6LlmParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setIntegerParam(chan, xsp3ChanDtcFlagsParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setDoubleParam(chan, xsp3ChanDtcAegParam, 0.0) == asynSuccess) && paramStatus);
        paramStatus = ((setDoubleParam(chan, xsp3ChanDtcAeoParam, 0.0) == asynSuccess) && paramStatus);
        paramStatus = ((setDoubleParam(chan, xsp3ChanDtcIwgParam, 0.0) == asynSuccess) && paramStatus);
        paramStatus = ((setDoubleParam(chan, xsp3ChanDtcIwoParam, 0.0) == asynSuccess) && paramStatus);
        paramStatus = ((setDoubleParam(chan, xsp3EventWidthParam, 5.0) == asynSuccess) && paramStatus);
        paramStatus = ((setDoubleParam(chan, xsp3ChanDTPercentParam, 0.0) == asynSuccess) && paramStatus);
        paramStatus = ((setDoubleParam(chan, xsp3ChanDTFactorParam, 1.0) == asynSuccess) && paramStatus);
    }
    return paramStatus;
}

/**
 * Xspress3 destructor
 *
 * The destructor ensures that the lock is released.
 */
Xspress3::~Xspress3()
{
    this->unlock();
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
  int xsp3_num_channels = 0;
  char configPath[maxStringSize_] = {0};
  char configSavePath[maxStringSize_] = {0};
  const char *functionName = "Xspress3::connect";

  getIntegerParam(xsp3NumCardsParam, &xsp3_num_cards);
  getIntegerParam(xsp3NumFramesConfigParam, &xsp3_num_tf);
  getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);
  getStringParam(xsp3ConfigPathParam, maxStringSize_, configPath);
  getStringParam(xsp3ConfigSavePathParam, maxStringSize_, configSavePath);

  //Set up the xspress3 system
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Calling xsp3_config.\n", functionName);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Base IP address is: %s\n", functionName, baseIP_.c_str());
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Number of cards is: %d\n", functionName, xsp3_num_cards);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Number of frames is: %d\n", functionName, xsp3_num_tf);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Number of channels is: %d\n", functionName, numChannels_);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Config path is: %s\n", functionName, configPath);
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Config save path is: %s\n", functionName, configSavePath);

  xsp3_handle_ = xsp3->config(xsp3_num_cards, xsp3_num_tf, const_cast<char *>(baseIP_.c_str()), -1, NULL, xsp3_num_channels, 1, NULL, debug_, 0);
  if (xsp3_handle_ < 0) {
    checkStatus(xsp3_handle_, "xsp3_config", functionName);
    status = asynError;
  } else {
    setIntegerParam(xsp3ConnectedParam, 1);

    int generation = xsp3->get_generation(xsp3_handle_, 0);

    //Set up clocks on each card
    for (int i=0; i<xsp3_num_cards && status == asynSuccess; i++) {
      xsp3_status = xsp3->clocks_setup(xsp3_handle_, i, generation == 3 ? XSP3M_CLK_SRC_LMK61E2 : (generation == 2 ? XSP3M_CLK_SRC_CDCM61004 : XSP3_CLK_SRC_XTAL),
                                      XSP3_CLK_FLAGS_MASTER | XSP3_CLK_FLAGS_NO_DITHER, 0);
      if (xsp3_status < 0) {
	      checkStatus(xsp3_status, "xsp3_clocks_setup", functionName);
	      status = asynError;
      }

      printf("xsp3_clocks_setup: Measured frequency %.2f MHz\n", float(xsp3_status)/1.0e6);
    }

    // Limit frames for Mini > 1 channel
    if (generation == 2 && numChannels_ > 1) {
        int paramStatus;
        paramStatus = ((setIntegerParam(xsp3NumFramesConfigParam, 12216) == asynSuccess) && paramStatus);
        paramStatus = ((setIntegerParam(xsp3NumFramesDriverParam, 12216) == asynSuccess) && paramStatus);
    }

    //Restore settings from a file
    if (status == asynSuccess)
        status = restoreSettings();

    //Set completion status
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
    xsp3_status = xsp3->get_window(xsp3_handle_, chan, 0, &xsp3_sca_param1, &xsp3_sca_param2);
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
    xsp3_status = xsp3->get_window(xsp3_handle_, chan, 1, &xsp3_sca_param1, &xsp3_sca_param2);
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
    xsp3_status = xsp3->get_good_thres(xsp3_handle_, chan, &xsp3_sca_param1);
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
    xsp3_status = xsp3->getDeadtimeCorrectionParameters(xsp3_handle_,
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
      setDoubleParam(chan, xsp3ChanDtcAegParam, static_cast<epicsFloat64>(xsp3_dtc_all_event_grad));
      setDoubleParam(chan, xsp3ChanDtcAeoParam, static_cast<epicsFloat64>(xsp3_dtc_all_event_off));
      setDoubleParam(chan, xsp3ChanDtcIwgParam, static_cast<epicsFloat64>(xsp3_dtc_in_window_grad));
      setDoubleParam(chan, xsp3ChanDtcIwoParam, static_cast<epicsFloat64>(xsp3_dtc_in_window_off));
    }

    callParamCallbacks(chan);
  }

  return status;
}



/**
 * Read the event width for each channel
 */
asynStatus Xspress3::readTrigB(void)
{
    const char *functionName = "Xspress3::readTrigB";
    asynStatus status = asynSuccess;
    Xspress3_TriggerB trig_b;
    int xsp3_status;

    int xsp3_num_channels = 0;
    getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);

    for (int chan=0; chan<xsp3_num_channels; chan++) {
        xsp3_status = xsp3->get_trigger_b(xsp3_handle_, chan, &trig_b);

	printf("xsp_get_trigger_b: chan=%d, status=%d, width=%d\n", chan, xsp3_status, trig_b.event_time);
        if (xsp3_status < XSP3_OK) {
            checkStatus(xsp3_status, "xsp3_get_trigger_b", functionName);
            status = asynError;
        } else {
	    /* MN 31-Aug-2016, from Stu Fisher:
	       for detectors with variable width events (and corresponding firmware?),
	       the following line should be changed to
	       int width = trig_b.enb_variable_width ? (trig_b.event_time-3) : trig_b.event_time;
   	    */
	  double width = trig_b.enb_variable_width ? (trig_b.event_time-3.0) : 1.0*trig_b.event_time;
	  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Channel %d Event Width: %.1f\n", functionName, chan, width);
	  setDoubleParam(chan, xsp3EventWidthParam, width);
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
    xsp3_status = xsp3->close(xsp3_handle_);
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
  char configSavePath[maxStringSize_] = {0};
  int connected = 0;
  const char *functionName = "Xspress3::saveSettings";

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Saving Xspress3 settings. This calls xsp3_save_settings().\n", functionName);

  getIntegerParam(xsp3ConnectedParam, &connected);
  getStringParam(xsp3ConfigSavePathParam, maxStringSize_, configSavePath);

  if ((configSavePath == NULL) || (connected != 1)) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: No config save path set, or not connected.\n", functionName);
    setIntegerParam(ADStatus, ADStatusError);
    status = asynError;
  } else {
    xsp3_status = xsp3->save_settings(xsp3_handle_, configSavePath);
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
  char configPath[maxStringSize_] = {0};
  int connected = 0;
  const char *functionName = "Xspress3::restoreSettings";

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Restoring Xspress3 settings. This calls xsp3_restore_settings().\n", functionName);

  getIntegerParam(xsp3ConnectedParam, &connected);
  getStringParam(xsp3ConfigPathParam, maxStringSize_, configPath);

  if ((configPath == NULL) || (connected != 1)) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: No config path set, or not connected.\n", functionName);
    setIntegerParam(ADStatus, ADStatusError);
    status = asynError;
  } else {
    xsp3_status = xsp3->restore_settings(xsp3_handle_, configPath, 0);
    if (xsp3_status != XSP3_OK) {
      checkStatus(xsp3_status, "xsp3_restore_settings", functionName);
      setStringParam(ADStatusMessage, "Error Restoring Configuration.");
      setIntegerParam(ADStatus, ADStatusError);
      status = asynError;
    } else {
      setStringParam(ADStatusMessage, "Restored Configuration.");
    }
  }

  //Can we do xsp3_format_run here? For normal user operation all the arguments seem to be set to zero.
  int xsp3_num_channels;
  getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);
  for (int chan=0; chan<xsp3_num_channels; chan++) {
    xsp3_status = xsp3->format_run(xsp3_handle_, chan, 0, 0, 0, 0, 0, 12);
    if (xsp3_status < XSP3_OK) {
      checkStatus(xsp3_status, "xsp3_format_run", functionName);
      status = asynError;
    } else {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Channel: %d, Number of time frames configured: %d\n", functionName, chan, xsp3_status);
    }
  }

  //Read run flags parameter
  int xsp3_run_flags;
  getIntegerParam(xsp3RunFlagsParam, &xsp3_run_flags);
  if (xsp3_run_flags == runFlag_MCA_SPECTRA_) {
    xsp3_status = xsp3->set_run_flags(xsp3_handle_, XSP3_RUN_FLAGS_SCALERS | XSP3_RUN_FLAGS_HIST);
  } else if (xsp3_run_flags == runFlag_PLAYB_MCA_SPECTRA_) {
    xsp3_status = xsp3->set_run_flags(xsp3_handle_, XSP3_RUN_FLAGS_PLAYBACK | XSP3_RUN_FLAGS_SCALERS | XSP3_RUN_FLAGS_HIST);
  } else {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s Invalid run flag option when trying to set xsp3_set_run_flags.\n", functionName);
    status = asynError;
  }

  if (xsp3_status < XSP3_OK) {
    checkStatus(xsp3_status, "xsp3_set_run_flags", functionName);
    status = asynError;
  }

    //Need to write the window params, and then read existing SCA params
    if (status == asynSuccess) {
        int lo_limit, hi_limit;
        // Set window for the first Scalar ROI
        for (int chan=0; chan<xsp3_num_channels && status == asynSuccess; chan++) {
            getIntegerParam(chan, xsp3ChanSca5HlmParam, &hi_limit);
            getIntegerParam(chan, xsp3ChanSca5LlmParam, &lo_limit);
            status = setWindow(chan, 0, lo_limit, hi_limit);
        }

        // Set window for the first Scalar ROI
        for (int chan=0; chan<xsp3_num_channels && status == asynSuccess; chan++) {
            getIntegerParam(chan, xsp3ChanSca6HlmParam, &hi_limit);
            getIntegerParam(chan, xsp3ChanSca6LlmParam, &lo_limit);
            status = setWindow(chan, 1, lo_limit, hi_limit);
        }
        status = readSCAParams();
    }

    // Set the trigger mode
    if (status == asynSuccess) {
       int trigger_mode, invert_f0, invert_veto, debounce;
       getIntegerParam(xsp3TriggerModeParam, &trigger_mode);
       getIntegerParam(xsp3InvertF0Param, &invert_f0);
       getIntegerParam(xsp3InvertVetoParam, &invert_veto);
       getIntegerParam(xsp3DebounceParam, &debounce);
       status = setTriggerMode(trigger_mode, invert_f0, invert_veto, debounce );
    }

    // Read the DTC parameters
    if (status == asynSuccess) {
        status = readDTCParams();
    }

    // Read Trig B for DTC
    if (status == asynSuccess) {
        status = readTrigB();
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
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "API Error Message: %s.\n", xsp3->get_error_message());
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
      xsp3_status = xsp3->set_window(xsp3_handle_, channel, sca, llm, hlm);
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

    status = eraseSCA();

    getIntegerParam(xsp3NumFramesDriverParam, &xsp3_time_frames);
    getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);

    xsp3_status = xsp3->histogram_clear(xsp3_handle_, 0, xsp3_num_channels, 0, xsp3_time_frames);
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
 * Function to clear the data.
 */
asynStatus Xspress3::eraseSCA(void)
{
  int status = asynSuccess;
  int xsp3_num_channels = 0;
  int maxNumFrames = 0;
  const char *functionName = "Xspress3::eraseSCA";

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Clear SCA data and all arrays.\n", functionName);

  getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);
  getIntegerParam(xsp3NumFramesDriverParam, &maxNumFrames);

  bool paramStatus = true;
  paramStatus = ((setIntegerParam(NDArrayCounter, 0) == asynSuccess) && paramStatus);
  paramStatus = ((setIntegerParam(xsp3FrameCountParam, 0) == asynSuccess) && paramStatus);

  for (int chan=0; chan<xsp3_num_channels; ++chan) {
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca0Param, 0.0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca1Param, 0.0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca2Param, 0.0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca3Param, 0.0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca4Param, 0.0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca5Param, 0.0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca6Param, 0.0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca7Param, 0.0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanDTPercentParam, 0.0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanDTFactorParam, 1.0) == asynSuccess) && paramStatus);

    //callParamCallbacks(chan);
  }

  // Send a blank frame
  NDArray *pMCA;
  int xsp3_max_spectra=0;
  getIntegerParam(xsp3MaxSpectraParam, &xsp3_max_spectra);

  NDDataType_t dataType= this->getDataType();


  size_t dims[2];
  this->getDims(dims);

  pMCA= this->pNDArrayPool->alloc(2, dims, dataType, 0, NULL);

  if (pMCA !=NULL) {
    memset(pMCA->pData,0,pMCA->dataSize);
    this->setNDArrayAttributes(pMCA, -1);

    this->lock();

    this->callParamCallbacks();
    this->unlock();
    this->doNDCallbacksIfRequired(pMCA);

    pMCA->release();

  }

  if (!paramStatus) {
    status = asynError;
  }

  if (status != asynSuccess) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR erasing data.\n", functionName);
  }

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

asynStatus Xspress3::setupITFG(void)
{
    asynStatus status = asynSuccess;
    const char *functionName = "Xspress3::setupITFG";
    int num_frames, trigger_mode, ppt;
    double exposureTime;
    int xsp3_status=XSP3_OK;

    getIntegerParam(xsp3TriggerModeParam, &trigger_mode);
    if (trigger_mode == mbboTriggerINTERNAL_ &&
        xsp3->has_itfg(xsp3_handle_, 0) > 0 ) {
        getIntegerParam(ADNumImages, &num_frames);
        getDoubleParam(ADAcquireTime, &exposureTime);
        xsp3_status = xsp3->itfg_setup( xsp3_handle_, 0, num_frames,
                                       (u_int32_t) floor(exposureTime*80E6+0.5),
                                       XSP3_ITFG_TRIG_MODE_BURST, XSP3_ITFG_GAP_MODE_1US );
    }

    getIntegerParam(xsp3PulsePerTriggerParam, &ppt);
    if (trigger_mode == mbboTriggerTTLVETO_ &&
        (xsp3->has_itfg(xsp3_handle_, 0) > 0) && ppt) {

        getIntegerParam(ADNumImages, &num_frames);
        // printf("setupIFTG - Pulse per trigger: %d\n", ppt);
        xsp3_status = xsp3->itfg_setup2( xsp3_handle_, 0, num_frames,
                                       (u_int32_t) ppt,
                                       XSP3_ITFG_TRIG_MODE_HARDWARE,
                                       XSP3_ITFG_GAP_MODE_1US, XSP3_ITFG_TRIG_ACQ_PAUSED_ALL, 0, 0 );
    }

    if (xsp3_status != XSP3_OK) {
        checkStatus(xsp3_status, " xsp3_itfg_setup", functionName);
        status = asynError;
    }

    return status;
}

/**
 * Function to map the database trigger mode
 * value to the macros defined by the API.
 * @param mode The database value
 * @param apiMode This returns the correct value for the API
 * @return asynStatus
 */
asynStatus Xspress3::mapTriggerMode(int mode, int invert_f0, int invert_veto, int debounce, int *apiMode)
{
  asynStatus status = asynSuccess;
  const char *functionName = "Xspress3::mapTriggerMode";

  if (mode == mbboTriggerFIXED_) {
    *apiMode = XSP3_GTIMA_SRC_FIXED;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_FIXED, value: %d\n",
	      functionName, XSP3_GTIMA_SRC_FIXED);
  } else if (mode == mbboTriggerINTERNAL_) {
    *apiMode = XSP3_GTIMA_SRC_INTERNAL;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_INTERNAL, value: %d\n",
	      functionName, XSP3_GTIMA_SRC_INTERNAL);
  } else if (mode == mbboTriggerIDC_) {
    *apiMode = XSP3_GTIMA_SRC_IDC;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_IDC, value: %d\n",
	      functionName, XSP3_GTIMA_SRC_IDC);
  } else if (mode == mbboTriggerTTLVETO_) {
    *apiMode = XSP3_GTIMA_SRC_TTL_VETO_ONLY;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_TTL_VETO_ONLY, value: %d\n",
	      functionName, XSP3_GTIMA_SRC_TTL_VETO_ONLY);
  } else if (mode == mbboTriggerTTLBOTH_) {
    *apiMode = XSP3_GTIMA_SRC_TTL_BOTH;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_TTL_BOTH, value: %d\n",
	      functionName, XSP3_GTIMA_SRC_TTL_BOTH);
  } else if (mode == mbboTriggerLVDSVETO_) {
    *apiMode = XSP3_GTIMA_SRC_LVDS_VETO_ONLY;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_LVDS_VETO_ONLY, value: %d\n",
	      functionName, XSP3_GTIMA_SRC_LVDS_VETO_ONLY);
  } else if (mode == mbboTriggerLVDSBOTH_) {
    *apiMode = XSP3_GTIMA_SRC_LVDS_BOTH;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Trigger Mode XSP3_GTIMA_SRC_LVDS_BOTH, value: %d\n",
	      functionName, XSP3_GTIMA_SRC_LVDS_BOTH);
  } else {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Mapping an unknown trigger mode. mode: %d\n", functionName, mode);
    setStringParam(ADStatusMessage, "ERROR Unknown Trigger Mode");
    setIntegerParam(ADStatus, ADStatusError);
    status = asynError;
  }

  if ( invert_f0 )   *apiMode |= XSP3_GLOB_TIMA_F0_INV;
  if ( invert_veto ) *apiMode |= XSP3_GLOB_TIMA_VETO_INV;
  if ( debounce )    *apiMode |= XSP3_GLOB_TIMA_DEBOUNCE(debounce);

  return status;
}

asynStatus Xspress3::setTriggerMode(int mode, int invert_f0, int invert_veto, int debounce )
{
    const char *functionName = "Xspress3::setTriggerMode";
    asynStatus status = asynSuccess;
    int xsp3_num_cards;
    int xsp3_trigger_mode = 0;

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set Trigger Mode.\n", functionName);
    getIntegerParam(xsp3NumCardsParam, &xsp3_num_cards);
    for (int card=0; card<xsp3_num_cards && status == asynSuccess; card++) {
        if ( card == 0 ) {
            status = mapTriggerMode(mode, invert_f0, invert_veto, debounce, &xsp3_trigger_mode);
        }
        else
        {
            status = mapTriggerMode(mbboTriggerTTLVETO_, invert_f0, 0, debounce, &xsp3_trigger_mode);
        }

        int xsp3_status = xsp3->set_glob_timeA(xsp3_handle_, card, xsp3_trigger_mode);
        if (xsp3_status != XSP3_OK) {
            checkStatus(xsp3_status, "xsp3_set_glob_timeA", functionName);
            status = asynError;
        }
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
  int xsp3_sca_lim = 0;
  int xsp3_time_frames = 0;
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

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s asynUser->reason: %d, value: %d, addr: %d\n", functionName, function, value, addr);

  getIntegerParam(ADStatus, &adStatus);

  if (function == xsp3ResetParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s System Reset.\n", functionName);
    if ((status = checkConnected()) == asynSuccess) {
      //What do we do here?
    }
  }
  else if (function == xsp3EraseParam) {
    if (adStatus != ADStatusAcquire) {
 	status = erase();
    }
  }
  else if (function == ADAcquire) {
    if (value) {
      if (adStatus != ADStatusAcquire) {
	  if ((status = checkConnected()) == asynSuccess) {
	    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Starting Data Collection.\n", functionName);
	    //MNewville:  explicitly stop histogram before starting.
	    getIntegerParam(xsp3NumFramesDriverParam, &xsp3_time_frames);
	    getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);
	    xsp3_status = xsp3->histogram_stop(xsp3_handle_, -1);
	    // MNewville July 2021, do not clear histogram here
	    // xsp3_status = xsp3->histogram_clear(xsp3_handle_, 0, xsp3_num_channels, 0, xsp3_time_frames);
            setupITFG();
	    xsp3_status = xsp3->histogram_start(xsp3_handle_, -1 );
	    if (xsp3_status != XSP3_OK) {
		checkStatus(xsp3_status, "xsp3_histogram_start", functionName);
		status = asynError;
	    }
	    if (status == asynSuccess) {
	      epicsEventSignal(this->startEvent_);
              asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Started Data Collection.\n", functionName);
	    }
	}
      }
    } else {
      if (adStatus == ADStatusAcquire) {
	  if ((status = checkConnected()) == asynSuccess) {
	    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Stop Data Collection.\n", functionName);
	    xsp3_status = xsp3->histogram_stop(xsp3_handle_, -1);
	    if (xsp3_status != XSP3_OK) {
		checkStatus(xsp3_status, "xsp3_histogram_stop", functionName);
		status = asynError;
	    }
	    if (status == asynSuccess) {
	      epicsEventSignal(this->stopEvent_);
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
  else if (function == xsp3TriggerModeParam ||
           function == xsp3InvertF0Param    ||
           function == xsp3InvertVetoParam  ||
           function == xsp3DebounceParam      ) {
    if ((status = checkConnected()) == asynSuccess) {
      int trigger_mode, invert_f0, invert_veto, debounce;

      getIntegerParam(xsp3TriggerModeParam, &trigger_mode);
      getIntegerParam(xsp3InvertF0Param, &invert_f0);
      getIntegerParam(xsp3InvertVetoParam, &invert_veto);
      getIntegerParam(xsp3DebounceParam, &debounce);

      if (function == xsp3TriggerModeParam)     trigger_mode=value;
      else if (function == xsp3InvertF0Param)   invert_f0=value;
      else if (function == xsp3InvertVetoParam) invert_veto=value;
      else debounce=value;

      status = setTriggerMode(trigger_mode, invert_f0, invert_veto, debounce );
    }
  }
  else if (function == xsp3FixedTimeParam) {
      if ((status = checkConnected()) == asynSuccess) {
	  xsp3_status = xsp3->set_glob_timeFixed(xsp3_handle_, -1, value);
	  if (xsp3_status != XSP3_OK) {
	    checkStatus(xsp3_status, "xsp3_set_glob_timeFixed", functionName);
	    status = asynError;
	  }
      }
  }
  else if (function == ADNumImages) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set Number Of Frames To Read Out.\n", functionName);
    getIntegerParam(xsp3NumFramesDriverParam, &xsp3_time_frames);
    if (value > xsp3_time_frames) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Num Frames Higher Than Number Configured.\n", functionName);
      status = asynError;
    }
  }
  else if (function == xsp3NumFramesConfigParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set Number Of Frames For Intial API Configuration.\n", functionName);
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
    if ((adStatus != ADStatusAcquire)) {
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
    if ((adStatus != ADStatusAcquire)) {
      status = restoreSettings();
    } else {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Restoring Not Allowed In This Mode.\n", functionName);
      status = asynError;
    }
  }
  else if (function == xsp3ChanSca4ThresholdParam) {
    if ((status = checkConnected()) == asynSuccess) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The SCA4 Threshold Register.\n", functionName);
      xsp3_status = xsp3->set_good_thres(xsp3_handle_, addr, value);
      if (xsp3_status != XSP3_OK) {
	checkStatus(xsp3_status, "xsp3_set_good_thres", functionName);
	status = asynError;
      }
    }
  }
  else if (function == xsp3TriggerParam) {
    if ((status = checkConnected()) == asynSuccess && adStatus == ADStatusAcquire && value) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Triggering Next Frame.\n", functionName);
      xsp3_status = xsp3->histogram_continue(xsp3_handle_, -1);
      if (xsp3_status != XSP3_OK) {
          checkStatus(xsp3_status, "xsp3_histogram_continue", functionName);
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

  else if (function == xsp3RunFlagsParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The Run Flags.\n", functionName);
  }

  else if (function == xsp3PulsePerTriggerParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Changing trigger mode for pulse per trigger.\n", functionName);
    int trigger_mode, invert_f0, invert_veto, debounce;

    getIntegerParam(xsp3TriggerModeParam, &trigger_mode);
    getIntegerParam(xsp3InvertF0Param, &invert_f0);
    getIntegerParam(xsp3InvertVetoParam, &invert_veto);
    getIntegerParam(xsp3DebounceParam, &debounce);

    if (trigger_mode == mbboTriggerTTLVETO_) {
      if (value > 0) {
        printf("PPT > 0, setting trigger mode to internal\n");
        status = setTriggerMode(mbboTriggerINTERNAL_, invert_f0, invert_veto, debounce );
      } else {
        printf("PPT = 0, resetting trigger mode to %d\n", trigger_mode);
        status = setTriggerMode(trigger_mode, invert_f0, invert_veto, debounce );
      }
    }

  }

  else if (function == xsp3ITFGStartParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Restarting ITFG.\n", functionName);
    printf("Restarting ITFG\n");
    xsp3->itfg_start(xsp3_handle_, 0);
  }

  else if (function == xsp3ITFGStopParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Pausing ITFG.\n", functionName);
    printf("Pausing ITFG\n");
    xsp3->itfg_stop(xsp3_handle_, 0);
  }

  else {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s No Matching Parameter In Xspress3 Driver.\n", functionName);
  }

  if (status != asynSuccess) {
    callParamCallbacks(addr);
    return asynError;
  }

  //The data readout thread will set ADAcquire back to false.
  if (function == ADAcquire) {
    if (!value) {
      return status;
    }
  }

  status = (asynStatus) setIntegerParam(addr, function, value);
  if (status!=asynSuccess) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
	      "%s Error Setting Parameter. Asyn addr: %d, asynUser->reason: %d, value: %d\n",
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
  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s asynUser->reason: %d, value: %f, addr: %d\n", functionName, function, value, addr);

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
      if (function < XSP3_FIRST_DRIVER_COMMAND) {
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
	      "%s Error Setting Parameter. asynUser->reason: %d\n",
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
      if ((xsp3->histogram_is_any_busy(xsp3_handle_)) == 0) {
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
 * Update areaDetector parameters to show an error
 *
 * @param message A string (const char*) to write to ADStatusMessage.
 */
void Xspress3::adReportError(const char* message)
{
    setStringParam(ADStatusMessage, message);
    setIntegerParam(ADStatus, ADStatusError);
    setIntegerParam(ADAcquire, ADAcquireFalse_);
}

/**
 * Malloc an array large enough to hold the SCAs
 *
 * @param pSCA A reference to a pointer that will point to the allocated memory
 *
 * @return true if an allocation error occurs otherwise false
 */
bool Xspress3::createSCAArray(void *&pSCA)
{
    const char *functionName = "Xspress3::createSCAArray";
    pSCA = malloc(XSP3_SW_NUM_SCALERS * this->numChannels_ * sizeof(double));
    if (pSCA == NULL) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s: ERROR: SCA malloc failed.\n", functionName);
        this->adReportError("Memory Error. Check IOC Log.");
        return true;
    } else {
        return false;
    }
}

/**
 * Allocate an NDArray to put a detector frame into
 *
 * @param dims [maximum number of spectral bins, number of channels]
 * @param pMCA Reference to a pointer to the NDArray that will be allocated
 * @param dataType The NDDataType_t of the NDArray (NDUInt32 or NDFloat64)
 *
 * @return true if an allocation error occurs otherwise false
 */
bool Xspress3::createMCAArray(size_t dims[2], NDArray *&pMCA, NDDataType_t dataType)
{
    const char *functionName = "Xspress3::createMCAArray";
    bool error = false;
    pMCA = this->pNDArrayPool->alloc(2, dims, dataType, 0, NULL);
    if (pMCA == NULL) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s: ERROR: pNDArrayPool->alloc failed.\n", functionName);
        this->adReportError("Memory Error. Check IOC Log.");
        error = true;
    }
    return error;
}

/**
 * Read a frame, of dead-time corrected data, from the hardware into
 * MCAData
 *
 * @param pSCA A pointer to the array to hold the SCAs
 * @param pMCAData A pointer to the array to hold the MCA
 * @param frameNumber The frame to read from the current capture
 * @param maxSpectra The maximum number of spectral bins in the MCA array
 *
 * @return true if an allocation error occurs otherwise false
 */
bool Xspress3::readFrame(double* pSCA, double* pMCAData, int frameNumber, int maxSpectra)
{
    bool error = false;
    int xsp3Status = 0;
    const char* functionName = "Xspress3::readFrame";
    xsp3Status = xsp3->hist_dtc_read4d(this->xsp3_handle_, pMCAData, pSCA, 0, 0, 0, frameNumber, maxSpectra, 1, this->numChannels_, 1);

    if (xsp3Status != XSP3_OK) {
        checkStatus(xsp3Status, "xsp3_hist_dtc_read4d", functionName);
        error = true;
    } else {
        setIntegerParam(NDArrayCounter, frameNumber+1);
    }
    return error;
}

bool Xspress3::readFrame(u_int32_t* pSCA, u_int32_t* pMCAData, int frameNumber, int maxSpectra)
{
    bool error = false;
    int xsp3Status = 0;
    const char* functionName = "Xspress3::readFrame";
    xsp3Status = xsp3->histogram_read4d(this->xsp3_handle_, pMCAData, 0, 0, 0, frameNumber, maxSpectra, 1, this->numChannels_, 1);
    if (xsp3Status != XSP3_OK) {
        checkStatus(xsp3Status, "xsp3_histogram_read4d", functionName);
        error = true;
    } else {
        setIntegerParam(NDArrayCounter, frameNumber);
        xsp3Status = xsp3->scaler_read(this->xsp3_handle_, pSCA, 0, 0, frameNumber, XSP3_SW_NUM_SCALERS, this->numChannels_, 1);
        if (xsp3Status != XSP3_OK) {
            checkStatus(xsp3Status, "xsp3_scaler_read", functionName);
            error = true;
        }
        else
        {
        setIntegerParam(NDArrayCounter, frameNumber+1);
        }
    }
    return error;
}

/**
 * A shortcut to wait for the stop event and print diagnostics if necessary
 *
 * @param timeout The period to wait for the stop event before giving up
 * @param message The message to print if the stop event has occured within timeout
 *
 * @return the status returned by epicsEventWaitWithTimeout
 */
const int Xspress3::checkForStopEvent(double timeout, const char *message)
{
    int eventStatus;
    eventStatus = epicsEventWaitWithTimeout(this->stopEvent_, timeout);
    if (eventStatus == epicsEventWaitOK) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, message);
    }
    return eventStatus;
}

/**
 * A shortcut to wait indefinitely for a start event and print diagnostics if necessary
 *
 * @param message The message to print if the event has occured and requested
 *
 * @return the status returned by epicsEventWaitWithTimeout
 */
const int Xspress3::waitForStartEvent(const char *message)
{
    int eventStatus;
    eventStatus = epicsEventWait(startEvent_);
    if (eventStatus == epicsEventWaitOK) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, message);
    }
    return eventStatus;
}

/**
 * Write the SCAs to the AD parameters
 *
 * @param pSCA A pointer to an array of SCAs from the hardware
 * @param numChannels The number of xspress3 channels in the SCA array
 */
void Xspress3::writeOutScas(void *&pSCA, int numChannels, NDDataType_t dataType)
{
    double allevt, resets, evtwidth, ctime;
    double dtperc=0.0;
    double dtfact=1.0;
    if (dataType == NDFloat64) {
      double *pScaData = static_cast<double*>(pSCA);
      for (int chan=0; chan<numChannels; ++chan) {
        this->setDoubleParam(chan, this->xsp3ChanSca0Param, static_cast<epicsFloat64>(pScaData[0]));
        this->setDoubleParam(chan, this->xsp3ChanSca1Param, static_cast<epicsFloat64>(pScaData[1]));
        this->setDoubleParam(chan, this->xsp3ChanSca2Param, static_cast<epicsFloat64>(pScaData[2]));
        this->setDoubleParam(chan, this->xsp3ChanSca3Param, static_cast<epicsFloat64>(pScaData[3]));
        this->setDoubleParam(chan, this->xsp3ChanSca4Param, static_cast<epicsFloat64>(pScaData[4]));
        this->setDoubleParam(chan, this->xsp3ChanSca5Param, static_cast<epicsFloat64>(pScaData[5]));
        this->setDoubleParam(chan, this->xsp3ChanSca6Param, static_cast<epicsFloat64>(pScaData[6]));
        this->setDoubleParam(chan, this->xsp3ChanSca7Param, static_cast<epicsFloat64>(pScaData[7]));

        // MN set percent deadtime and deadtime correction factor here
        ctime  = pScaData[0];
        if (ctime > 10) {
          getDoubleParam(chan, xsp3EventWidthParam, &evtwidth);
          resets = pScaData[1];
          allevt = pScaData[3];
          dtperc = 100.0*(allevt*(evtwidth+1) + resets)/ctime;
          dtfact = ctime/(ctime - (allevt*(evtwidth+1) + resets));
        }
        // printf(":D> chan=%i, event_width=%.1f DTpercent=%.3f, DTfactor=%.6f", chan, evtwidth, dtperc, dtfact);
        setDoubleParam(chan, xsp3ChanDTPercentParam, static_cast<epicsFloat64>(dtperc));
        setDoubleParam(chan, xsp3ChanDTFactorParam, static_cast<epicsFloat64>(dtfact));

        pScaData += XSP3_SW_NUM_SCALERS;
      }
    }
    else {
      u_int32_t *pScaData = static_cast<u_int32_t*>(pSCA);
      for (int chan=0; chan<numChannels; ++chan) {
        this->setDoubleParam(chan, this->xsp3ChanSca0Param, static_cast<epicsFloat64>(pScaData[0]));
        this->setDoubleParam(chan, this->xsp3ChanSca1Param, static_cast<epicsFloat64>(pScaData[1]));
        this->setDoubleParam(chan, this->xsp3ChanSca2Param, static_cast<epicsFloat64>(pScaData[2]));
        this->setDoubleParam(chan, this->xsp3ChanSca3Param, static_cast<epicsFloat64>(pScaData[3]));
        this->setDoubleParam(chan, this->xsp3ChanSca4Param, static_cast<epicsFloat64>(pScaData[4]));
        this->setDoubleParam(chan, this->xsp3ChanSca5Param, static_cast<epicsFloat64>(pScaData[5]));
        this->setDoubleParam(chan, this->xsp3ChanSca6Param, static_cast<epicsFloat64>(pScaData[6]));
        this->setDoubleParam(chan, this->xsp3ChanSca7Param, static_cast<epicsFloat64>(pScaData[7]));

        // MN set percent deadtime and deadtime correction factor here
        ctime  = 1.0*pScaData[0];
        if (ctime > 10) {
          getDoubleParam(chan, xsp3EventWidthParam, &evtwidth);
          resets = 1.0*pScaData[1];
          allevt = 1.0*pScaData[3];
          dtperc = 100.0*(allevt*(evtwidth+1) + resets)/ctime;
          dtfact = ctime/(ctime - (allevt*(evtwidth+1) + resets));
        }
        // printf(":I> chan=%i, event_width=%.1f DTpercent=%.3f, DTfactor=%.6f\n", chan, evtwidth, dtperc, dtfact);
        setDoubleParam(chan, xsp3ChanDTPercentParam, static_cast<epicsFloat64>(dtperc));
        setDoubleParam(chan, xsp3ChanDTFactorParam, static_cast<epicsFloat64>(dtfact));

        pScaData += XSP3_SW_NUM_SCALERS;
      }
    }
}

/**
 * Set parameters as they should be at the start of an acquisition
 *
 */
void Xspress3::setStartingParameters()
{
    this->setIntegerParam(this->NDArrayCounter, 0);
    this->setIntegerParam(this->xsp3FrameCountParam, 0);
    this->setIntegerParam(this->ADStatus, ADStatusAcquire);
    this->setStringParam(this->ADStatusMessage, "Acquiring Data");
    this->callParamCallbacks();
}

/**
 * Dead time corrected data is double precision floating point
 * uncorrected data is unsigned 32 bit integers so find out
 * which one and return it
 * @return const NDDataType_t the type as specified in the NDDataType_t enum
 */
const NDDataType_t Xspress3::getDataType()
{
    int deadTimeCorrect;
    this->getIntegerParam(this->xsp3DtcEnableParam, &deadTimeCorrect);
    if (deadTimeCorrect) {
        return NDFloat64;
    } else {
        return NDUInt32;
    }
}

/**
 * Get the dimensions of a frame from the xsp3 parameters
 * as [maxSpectra, numChannels]
 *
 * @param dims A reference to an array to store the dimensions in
 */
void Xspress3::getDims(size_t (&dims)[2])
{
    int numChannels, maxSpectra;
    this->getIntegerParam(this->xsp3NumChannelsParam, &numChannels);
    this->getIntegerParam(this->xsp3MaxSpectraParam, &maxSpectra);
    dims[0] = maxSpectra;
    dims[1] = numChannels;
}

/**
 * Sets the uniqueId of *pMCA to the frame number and sets the timeStamp
 * to the current time.
 *
 * @param pMCA A reference to a pointer to an NDArray
 * @param frameNumber The number of the frame to be written to pMCA->uniqueId
 */
void Xspress3::setNDArrayAttributes(NDArray *&pMCA, int frameNumber)
{
    int arrayCallbacks = 0;
    epicsTimeStamp currentTime;
    this->getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
    epicsTimeGetCurrent(&currentTime);
    pMCA->uniqueId = frameNumber;
    pMCA->timeStamp = currentTime.secPastEpoch + currentTime.nsec/1e9;
    pMCA->pAttributeList->add("TIMESTAMP", "Host Timestamp", NDAttrFloat64, &(pMCA->timeStamp));
    this->getAttributes(pMCA->pAttributeList);
}

/**
 * Set the parameters to show that acquisition has stopped
 *
 * @param aborted true if the acquisition was aborted early
 */
void Xspress3::setAcqStopParameters(bool aborted)
{
    this->setIntegerParam(ADAcquire, ADAcquireFalse_);
    if (aborted) {
        this->setIntegerParam(ADStatus, ADStatusAborted);
        this->setStringParam(ADStatusMessage, "Stopped Acquiring");
    } else {
        this->setIntegerParam(ADStatus, ADStatusIdle);
        this->setStringParam(ADStatusMessage, "Completed Acquisition");
    }
    this->callParamCallbacks();
}

/**
 * A getter for ADNumImages
 *
 *
 * @return The number of frames stored in ADNumImages
 */
int Xspress3::getNumFramesToAcquire()
{
    int numFrames;
    this->getIntegerParam(ADNumImages, &numFrames);
    return numFrames;
}

void Xspress3::doNDCallbacksIfRequired(NDArray *pMCA)
{
    int arrayCallbacks;
    this->getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
    if (arrayCallbacks) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "doNDCallbacksIfRequired: Calling NDArray callback\n");
        this->doCallbacksGenericPointer(pMCA, NDArrayData, 0);
    }
}

int Xspress3::getNumFramesRead()
{
    int numFrames = 0;
    int xsp3Status = xsp3->scaler_check_progress(this->xsp3_handle_);
    if (xsp3Status < XSP3_OK) {
        this->checkStatus(xsp3Status, "xsp3_dma_check_desc", "getNumFrameRead");
    } else {
        numFrames = xsp3Status;
        this->setIntegerParam(xsp3FrameCountParam, numFrames);
    }
    return numFrames;
}

void Xspress3::xspAsynPrint(int asynPrintType, const char *format, ...)
{
    const int maxMessageLen=1024;
    va_list pArg;
    char message[maxMessageLen];
    va_start(pArg, format);
    vsprintf(message, format, pArg);
    asynPrint(this->pasynUserSelf, asynPrintType, message);
    va_end(pArg);
}

/**
 * A function, ordinarily to be run in a seperate thread, to wait for
 * and then execute acquisitions.
 *
 * @param xspAD A pointer to an instance of Xspress3
 */
static void xsp3DataTaskC(void *xspAD)
{
    Xspress3 *pXspAD = (Xspress3 *)xspAD;
    void *pSCA;
    NDArray *pMCA;
    NDDataType_t dataType;
    bool acquire, aborted, error=false;
    int numChannels, maxSpectra, frameNumber, numFrames=0, acquired, lastAcquired;
    size_t dims[2];
    const double timeout = 0.00001;
    const int checkTimes = 20;
    // The scalar array can be reused so create it now
    pXspAD->createSCAArray(pSCA);
    while (1) {
        acquired = lastAcquired = frameNumber = 0;
        aborted = false;
        pXspAD->checkForStopEvent(timeout, "Got stop event before start event.\n");
        if (pXspAD->waitForStartEvent("Got start event.\n") == epicsEventWaitOK) {
            acquire = true;
            pXspAD->lock();
            pXspAD->setStartingParameters();
            pXspAD->unlock();
        }
        dataType = pXspAD->getDataType();
        pXspAD->getDims(dims);
        maxSpectra = dims[0];
        numChannels = dims[1];
        numFrames = pXspAD->getNumFramesToAcquire();
        pXspAD->xspAsynPrint(ASYN_TRACE_FLOW, "Collect %d frames\n", numFrames);
        while (acquire && (frameNumber < numFrames)) {
            acquired = pXspAD->getNumFramesRead();
            if (frameNumber < acquired) {
                lastAcquired = acquired;
                if (!pXspAD->createMCAArray(dims, pMCA, dataType)) {
                    if (dataType == NDFloat64) {
                        error = pXspAD->readFrame(static_cast<double*>(pSCA), static_cast<double*>(pMCA->pData), frameNumber, maxSpectra);
                    }
                    else {
                        error = pXspAD->readFrame(static_cast<u_int32_t*>(pSCA), static_cast<u_int32_t*>(pMCA->pData), frameNumber, maxSpectra);
                    }
                    if (error) {
                        pXspAD->xspAsynPrint(ASYN_TRACE_ERROR, "There was an error during read out %d\n", error);
                    }

                    pXspAD->lock();
                    pXspAD->writeOutScas(pSCA, numChannels, dataType);
                    pXspAD->unlock();
                    frameNumber++;
                    pXspAD->setNDArrayAttributes(pMCA, frameNumber);
                    pXspAD->lock();
                    pXspAD->callParamCallbacks();
                    pXspAD->unlock();
                    pXspAD->doNDCallbacksIfRequired(pMCA);
                    pMCA->release();
                }
                else {
                    pXspAD->xspAsynPrint(ASYN_TRACE_ERROR, "Did not create a new array!\n");
                }

            }
            if (pXspAD->checkForStopEvent(timeout, "Got stop event.\n") == epicsEventWaitOK) {
                acquire = false;
                aborted = true;
                pXspAD->checkHistBusy(checkTimes);
                pXspAD->lock();
                pXspAD->setAcqStopParameters(true);
                pXspAD->unlock();
            }
        }
        if (!aborted) {
            pXspAD->lock();
            pXspAD->setAcqStopParameters(false);
            pXspAD->unlock();
        }
    }
}

/*************************************************************************************/
/** The following functions have C linkage, and can be called directly or from iocsh */



extern "C" {

/**
 * Config function for IOC shell. It instantiates an instance of the driver.
 * @param portName The Asyn port name to use
 * @param numChannels The max number of channels (eg. 8)
 * @param numCards The number of Xspress3 systems (normally 1)
 * @param baseIP The base address used by the Xspress3 1Gig and 10Gig interfaces (eg. "192.168.0.1")
 * @param maxFrames The maximum number of frames that can be acquired in one acquisition (eg. 16384)
 * @param maxDriverFrames The maximum number of frames that can be acquired in one acquisition (eg. 16384)??
 * @param maxSpectra The maximum size of each spectra (eg. 4096)
 * @param maxBuffers Used by asynPortDriver (set to -1 for unlimited)
 * @param maxMemory Used by asynPortDriver (set to -1 for unlimited)
 * @param debug This debug flag is passed to xsp3_config in the Xspress API (0 or 1)
 * @param simTest 0 or 1. Set to 1 to run up this driver in simulation mode.
 */
  int xspress3Config(const char *portName, int numChannels, int numCards, const char *baseIP, int maxFrames, int maxDriverFrames, int maxSpectra, int maxBuffers, size_t maxMemory, int debug, int simTest)
  {
    asynStatus status = asynSuccess;

    /*Instantiate class.*/
    try {
      new Xspress3(portName, numChannels, numCards, baseIP, maxFrames, maxDriverFrames, maxSpectra, maxBuffers, maxMemory, debug, simTest);
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
  static const iocshArg xspress3ConfigArg5 = {"Max driver Frames", iocshArgInt};
  static const iocshArg xspress3ConfigArg6 = {"Max Spectra", iocshArgInt};
  static const iocshArg xspress3ConfigArg7 = {"Max Buffers", iocshArgInt};
  static const iocshArg xspress3ConfigArg8 = {"Max Memory", iocshArgInt};
  static const iocshArg xspress3ConfigArg9 = {"Debug", iocshArgInt};
  static const iocshArg xspress3ConfigArg10 = {"Sim Test", iocshArgInt};
  static const iocshArg * const xspress3ConfigArgs[] =  {&xspress3ConfigArg0,
							 &xspress3ConfigArg1,
							 &xspress3ConfigArg2,
							 &xspress3ConfigArg3,
							 &xspress3ConfigArg4,
							 &xspress3ConfigArg5,
							 &xspress3ConfigArg6,
							 &xspress3ConfigArg7,
							 &xspress3ConfigArg8,
							 &xspress3ConfigArg9,
							 &xspress3ConfigArg10};


  static const iocshFuncDef configXspress3 = {"xspress3Config", 11, xspress3ConfigArgs};
  static void configXspress3CallFunc(const iocshArgBuf *args)
  {
    xspress3Config(args[0].sval, args[1].ival, args[2].ival, args[3].sval, args[4].ival, args[5].ival, args[6].ival, args[7].ival, args[8].ival, args[9].ival, args[10].ival);
  }

  static void xspress3Register(void)
  {
    iocshRegister(&configXspress3, configXspress3CallFunc);
  }

  epicsExportRegistrar(xspress3Register);

} // extern "C"


/****************************************************************************************/
