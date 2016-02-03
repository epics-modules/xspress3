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
const epicsInt32 Xspress3::maxNumRoi_ = 16;
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
const epicsUInt8 Xspress3::startEvent = 1;
const epicsUInt8 Xspress3::stopEvent = 2;

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
 * @param maxDriverFrames The maximum number of frames that can be acquired in one acquisition (eg. 16384)
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

    // Create a message queue to pass events e.g. start and stop. The
    // messages are epicsInt32s e.g. Xspress3::startEvent
    this->queueSize = 16;
    eventQueue = new epicsMessageQueue(this->queueSize, sizeof(this->startEvent));
    // Create the array to hold the scalars and make sure a float and
    // double pointer point at the array so that the same array can be
    // used regardless of dead-time correction on the hardware.
    pSCAd = new double[XSP3_SW_NUM_SCALERS * numChannels];
    pSCAui = reinterpret_cast<u_int32_t*>(pSCAd);
    this->createInitialParameters();
    //Initialize non static, non const, data members
    xsp3_handle_ = 0;
    this->lock();
    bool paramStatus = this->setInitialParameters(maxFrames, maxDriverFrames, numCards, maxSpectra);
    this->unlock();
    paramStatus = ((eraseSCAMCAROI() == asynSuccess) && paramStatus);
    //Create the thread that readouts the data 
    status = (epicsThreadCreate("GetDataTask",
                                epicsThreadPriorityHigh,
                                epicsThreadGetStackSize(epicsThreadStackMedium),
                                (EPICSTHREADFUNC)xsp3DataTaskC,
                                this) == NULL);
    if (status) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s epicsThreadCreate failure for data task.\n", functionName);
        return;
    }
    this->lock();
    if (simTest_) {
        printf( "Simulation: %d\n", simTest_ );
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
    this->unlock();
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
    createParam(xsp3RoiEnableParamString, asynParamInt32, &xsp3RoiEnableParam);
    createParam(xsp3DtcEnableParamString, asynParamInt32, &xsp3DtcEnableParam);

    createParam(pointsPerRowParamString, asynParamInt32, &pointsPerRowParam);
    createParam(readyForNextRowParamString, asynParamInt32, &readyForNextRowParam);
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
    for (int chan=0; chan<numChannels_; chan++) {
        paramStatus = ((setIntegerParam(chan, xsp3ChanSca4ThresholdParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setIntegerParam(chan, xsp3ChanSca5HlmParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setIntegerParam(chan, xsp3ChanSca6HlmParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setIntegerParam(chan, xsp3ChanSca5LlmParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setIntegerParam(chan, xsp3ChanSca6LlmParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setIntegerParam(chan, xsp3ChanDtcFlagsParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setDoubleParam(chan, xsp3ChanDtcAegParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setDoubleParam(chan, xsp3ChanDtcAeoParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setDoubleParam(chan, xsp3ChanDtcIwgParam, 0) == asynSuccess) && paramStatus);
        paramStatus = ((setDoubleParam(chan, xsp3ChanDtcIwoParam, 0) == asynSuccess) && paramStatus);
    }
    paramStatus = ((setIntegerParam(readyForNextRowParam, 0) == asynSuccess) && paramStatus);
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

    //Set up clocks on each card
    for (int i=0; i<xsp3_num_cards && status == asynSuccess; i++) {
      xsp3_status = xsp3->clocks_setup(xsp3_handle_, i, XSP3_CLK_SRC_XTAL,
                                      XSP3_CLK_FLAGS_MASTER | XSP3_CLK_FLAGS_NO_DITHER, 0);
      if (xsp3_status != XSP3_OK) {
	checkStatus(xsp3_status, "xsp3_clocks_setup", functionName);
	status = asynError;
      }
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
	      "%s ERROR: ROI %d low limit (%d) is higher than high limit (%d) on channel %d.\n", 
	      functionName, roi, llm, hlm, channel+1);
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
asynStatus Xspress3::eraseSCAMCAROI(void)
{
  int status = asynSuccess;
  int xsp3_num_channels = 0;
  int maxNumFrames = 0;
  const char *functionName = "Xspress3::eraseSCAMCAROI";

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Clear SCA data, MCA ROI data and all arrays.\n", functionName);

  getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);
  getIntegerParam(xsp3NumFramesDriverParam, &maxNumFrames);

  bool paramStatus = true;
  paramStatus = ((setIntegerParam(NDArrayCounter, 0) == asynSuccess) && paramStatus);
  paramStatus = ((setIntegerParam(xsp3FrameCountParam, 0) == asynSuccess) && paramStatus);

  for (int chan=0; chan<xsp3_num_channels; ++chan) {
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca0Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca1Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca2Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca3Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca4Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca5Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca6Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanSca7Param, 0) == asynSuccess) && paramStatus);
    callParamCallbacks(chan);
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
    int num_frames, trigger_mode;
    double exposureTime;
    int xsp3_status=XSP3_OK;

    getIntegerParam(xsp3TriggerModeParam, &trigger_mode);
    if (trigger_mode == XSP3_GTIMA_SRC_INTERNAL  &&
        xsp3->has_itfg(xsp3_handle_, 0) > 0 ) {
        getIntegerParam(ADNumImages, &num_frames);
        getDoubleParam(ADAcquireTime, &exposureTime);
        xsp3_status = xsp3->itfg_setup( xsp3_handle_, 0, num_frames, 
                                       (u_int32_t) floor(exposureTime*80E6+0.5),
                                       XSP3_ITFG_TRIG_MODE_BURST, XSP3_ITFG_GAP_MODE_1US );
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
            setupITFG();
	    xsp3_status = xsp3->histogram_start(xsp3_handle_, -1 );
	    if (xsp3_status != XSP3_OK) {
		checkStatus(xsp3_status, "xsp3_histogram_start", functionName);
		status = asynError;
	    }
	    if (status == asynSuccess) {
            this->pushEvent(this->startEvent);
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
            this->pushEvent(this->stopEvent);
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

  else if (function == xsp3RoiEnableParam) {
    if (value == ctrlDisable_) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Disabling ROI Calculations.\n", functionName);
    } else if (value == ctrlEnable_) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Enabling ROI Calculations.\n", functionName);
    }
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
    this->pushEvent(this->stopEvent);
}

/** 
 * Allocate an NDArray to put a detector frame into
 *
 * @param dims [maximum number of spectral bins, number of channels]
 * @param pMCA Reference to a pointer to the NDArray that will be allocated
 * @param dataType The NDDataType_t of the NDArray (NDUInt32 or NDFloat64)
 *
 * @return false if an allocation error occurs otherwise false
 */
bool Xspress3::createMCAArray(size_t dims[2], NDArray *&pMCA, NDDataType_t dataType)
{
    const char *functionName = "Xspress3::createMCAArray";
    bool allocated = false;
    pMCA = this->pNDArrayPool->alloc(2, dims, dataType, 0, NULL);
    if (pMCA == NULL) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s: ERROR: pNDArrayPool->alloc failed.\n", functionName);
        this->adReportError("Memory Error. Check IOC Log.");
    } else {
        allocated = true;
    }
    return allocated;
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
        xsp3Status = xsp3->scaler_read(this->xsp3_handle_, pSCA, 0, 0, frameNumber, XSP3_SW_NUM_SCALERS, this->numChannels_, 1);
        if (xsp3Status != XSP3_OK) {
            checkStatus(xsp3Status, "xsp3_scaler_read", functionName);
            error = true;
        }
    }
    return error;
}

/** 
 * Set parameters as they should be at the start of an acquisition
 *
 */
void Xspress3::setStartingParameters()
{
    this->lock();
    this->setIntegerParam(this->NDArrayCounter, 0);
    this->setIntegerParam(this->xsp3FrameCountParam, 0);
    this->setIntegerParam(this->ADStatus, ADStatusAcquire);
    this->setStringParam(this->ADStatusMessage, "Acquiring Data");
    this->setIntegerParam(this->ADNumImagesCounter, 0);
    this->callParamCallbacks();
    this->unlock();
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
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "setAcqStopParameters\n");
    this->lock();
    this->setIntegerParam(ADAcquire, ADAcquireFalse_);
    if (aborted) {
        this->setIntegerParam(ADStatus, ADStatusAborted);
        this->setStringParam(ADStatusMessage, "Stopped Acquiring");
    } else {
        this->setIntegerParam(ADStatus, ADStatusIdle);
        this->setStringParam(ADStatusMessage, "Completed Acquisition");
    }
    this->callParamCallbacks();
    this->unlock();
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
        this->unlock();
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "doNDCallbacksIfRequired: Calling NDArray callback\n");
        this->doCallbacksGenericPointer(pMCA, NDArrayData, 0);
        this->lock();
    }
}

void Xspress3::pushEvent(const epicsUInt8& message)
{
    epicsUInt8 mess = message;
    void *pMessage = reinterpret_cast<void*>(&mess);
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
              "push %d event on to queue\n", message);
    if (this->eventQueue->trySend(pMessage, sizeof(mess)) == -1) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                  "No room left on the event queue\n");
        throw 0;
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
        this->lock();
        this->setIntegerParam(xsp3FrameCountParam, numFrames);
        this->unlock();
    }
    return numFrames;
}

void Xspress3::grabFrame(int frameNumber, int frameOffset)
{
    NDArray *pMCA;
    size_t dims[2];
    bool error;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
              "grabFrame: number %d; offset %d\n", frameNumber, frameOffset);
    this->getDims(dims);
    if (this->dtcEnabled && this->createMCAArray(dims, pMCA, NDFloat64)) {
        error = this->readFrame(pSCAd, static_cast<double*>(pMCA->pData),
                                frameNumber, this->maxSpectra);
        this->writeOutScas(pSCAd, this->numChannels_);
    } else if (this->createMCAArray(dims, pMCA, NDUInt32)) {
        error = this->readFrame(pSCAui, static_cast<u_int32_t*>(pMCA->pData),
                                frameNumber, this->maxSpectra);
        this->writeOutScas(pSCAui, this->numChannels_);
    } else {
        error = true;
    }
    if (!error) {
        this->setNDArrayAttributes(pMCA, frameOffset + frameNumber);
        this->lock();
        setIntegerParam(NDArrayCounter, frameOffset + frameNumber);
        this->doNDCallbacksIfRequired(pMCA);
        this->unlock();
        pMCA->release();
    } else {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                  "Failed to grab frame\n");
    }
}

/** 
 * Check the event queue for "request"
 *
 * @param request event to match
 * @param block if true wait for an event
 *
 * @return true if the event matches request else false
 */
bool Xspress3::checkQueue(const epicsUInt8 request, bool block)
{
    epicsUInt8 event;
    void *pEvent = &event;
    int status;
    const char *functionName = "Xspress3::checkQueue";
    if (block) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
                  "%s Wait for an event\n", functionName);
        status = this->eventQueue->receive(pEvent, sizeof(event));
    } else {
        status = this->eventQueue->tryReceive(pEvent, sizeof(event));
    }
    if (status != -1) {
        if (event == request) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

/** 
 * Read numToAcquire frames in chunks of chunkSize
 *
 * A lap is either the total number of frames to acquire or a multiple
 * of chunkSize that fits into the Xspress3 buffer. The chunkSize is
 * typically the number of points in a row of a map so that resetting of
 * the Xspress3 buffers can happen between rows.
 *
 * @param chunkSize number of points between readyForNextRow
 * @param numToAcquire number of frames to collect this lap
 * @param startFrame number of the last frame or zero
 */
void Xspress3::doALap(int chunkSize, int numToAcquire, int startFrame)
{
    // If histogram_start takes too long this could drop frames but GDA
    // should wait for the readyForNextParam signal. It should be
    // possible to reset the time frames with an Frame0 signal on the
    // TTL0 input of the Xspress3 boxes according to William Helsby but
    // this is currently untested.
    int xsp3Status, xsp3NChannels, totalFrames = startFrame;
    epicsUInt8 event;
    void *pEvent = &event;
    const char *functionName = "Xspress3::doALap";
    getIntegerParam(xsp3NumChannelsParam, &xsp3NChannels);
    if (this->getNumFramesRead() == numToAcquire) {
        xsp3Status = this->xsp3->histogram_start(this->xsp3_handle_, -1);
        if (xsp3Status != XSP3_OK) {
            this->checkStatus(xsp3Status, "histogram_start", functionName);
        }
    }
    for (int i=0; i<numToAcquire; i+=chunkSize) {
        this->lock();
        this->setIntegerParam(this->readyForNextRowParam, 1);
        this->callParamCallbacks();
        this->unlock();
        for (int j=0; j<chunkSize; j++) {
            do {
                if (this->checkQueue(this->stopEvent, false)) {
                    asynPrint(this->pasynUserSelf, ASYN_TRACE_WARNING,
                              "Stop event so throw\n");
                    throw 0;
                }                
            } while (this->getNumFramesRead() <= (i + j));
            this->grabFrame(i + j, startFrame);
            totalFrames++;
            asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "doALap lock %d\n", totalFrames);
            this->lock();
            if (j == 0)
                this->setIntegerParam(this->readyForNextRowParam, 0);
            this->setIntegerParam(this->NDArrayCounter, totalFrames);
            this->setIntegerParam(this->xsp3FrameCountParam, totalFrames);
            this->callParamCallbacks();
            asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "doALap unlock %d\n", totalFrames);
            this->unlock();
        }
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "Clear histogram\n");
        xsp3Status = this->xsp3->histogram_clear(this->xsp3_handle_, 0,
                                                 xsp3NChannels, i,
                                                 chunkSize);
        this->checkStatus(xsp3Status, "histogram_clear", functionName);
    }
}

void Xspress3::startAcquisition()
{
    int numToAcquire, maxFrames, xsp3Status, lapLength, frameNum=0;
    int chunkSize;
    this->setStartingParameters();
    this->getIntegerParam(this->xsp3MaxFramesParam, &maxFrames);
    this->getIntegerParam(this->xsp3MaxSpectraParam, &this->maxSpectra);
    this->getIntegerParam(this->xsp3DtcEnableParam, &this->dtcEnabled);
    numToAcquire = this->getNumFramesToAcquire();
    this->getIntegerParam(this->pointsPerRowParam, &chunkSize);
    chunkSize = chunkSize <= numToAcquire ? chunkSize : numToAcquire;
    // The lap should take a round number of chunks so take off
    // the modulo if required
    lapLength = numToAcquire > maxFrames ?
        maxFrames - maxFrames%chunkSize : numToAcquire;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "Collect %d frames; %d per lap\n",
              numToAcquire, lapLength);
    this->getIntegerParam(this->xsp3MaxFramesParam, &maxFrames);
    while (numToAcquire > 0) {
        try {
            this->doALap(chunkSize < numToAcquire ? chunkSize : numToAcquire,
                         lapLength < numToAcquire ? lapLength : numToAcquire,
                         frameNum);
            numToAcquire -= lapLength;
            frameNum += lapLength;
            if (numToAcquire <= 0) {
                this->xsp3->histogram_stop(this->xsp3_handle_, -1);
                this->setAcqStopParameters(false);
                this->callParamCallbacks();
            }
        } catch (int error) {
            this->xsp3->histogram_stop(this->xsp3_handle_, -1);
            this->setAcqStopParameters(true);
            this->xsp3->histogram_clear(this->xsp3_handle_, 0,
                                        this->numChannels_, 0, maxFrames);
            numToAcquire = 0;
        }
    }

}

void Xspress3::mainLoop()
{
    epicsUInt8 event;
    void *pEvent = &event;
    int status;
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "Main loop\n");
    while (true) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
                  "Wait for start event\n");
        if (this->checkQueue(this->startEvent, true)) {
            this->startAcquisition();
        }
    }
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
    pXspAD->mainLoop();
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

