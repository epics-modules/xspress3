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

#define XSP3IF_DEBUG 1
#ifdef XSP3IF_DEBUG
#include "xspress3Interface.h"
#endif

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
	     asynInt32Mask | asynInt32ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynFloat64ArrayMask | asynDrvUserMask | asynOctetMask | asynGenericPointerMask, /* Interface mask */
	     asynInt32Mask | asynInt32ArrayMask | asynFloat64Mask | asynFloat32ArrayMask | asynFloat64ArrayMask | asynOctetMask | asynGenericPointerMask,  /* Interrupt mask */
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

  //Add the params to the paramLib 
  //createParam adds the parameters to all param lists automatically (using maxAddr).
  createParam(xsp3FirstParamString,         asynParamInt32,       &xsp3FirstParam);
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
  createParam(xsp3NumFramesDriverParamString,     asynParamInt32,       &xsp3NumFramesDriverParam);
  createParam(xsp3NumCardsParamString,      asynParamInt32,       &xsp3NumCardsParam);
  createParam(xsp3ConfigPathParamString,    asynParamOctet,       &xsp3ConfigPathParam);
  createParam(xsp3ConfigSavePathParamString,    asynParamOctet,       &xsp3ConfigSavePathParam);
  createParam(xsp3ConnectParamString,      asynParamInt32,       &xsp3ConnectParam);
  createParam(xsp3ConnectedParamString,      asynParamInt32,       &xsp3ConnectedParam);
  createParam(xsp3DisconnectParamString,      asynParamInt32,       &xsp3DisconnectParam);
  createParam(xsp3SaveSettingsParamString,      asynParamInt32,       &xsp3SaveSettingsParam);
  createParam(xsp3RestoreSettingsParamString,      asynParamInt32,       &xsp3RestoreSettingsParam);
  createParam(xsp3RunFlagsParamString,      asynParamInt32,       &xsp3RunFlagsParam);
  createParam(xsp3TriggerParamString,      asynParamInt32,       &xsp3TriggerParam);
  createParam(xsp3InvertF0ParamString,      asynParamInt32,       &xsp3InvertF0Param);
  createParam(xsp3InvertVetoParamString,      asynParamInt32,       &xsp3InvertVetoParam);
  createParam(xsp3DebounceParamString,      asynParamInt32,       &xsp3DebounceParam);
  //These params will use different param lists based on asyn address
  createParam(xsp3ChanMcaRoi1LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi1LlmParam);
  createParam(xsp3ChanMcaRoi2LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi2LlmParam);
  createParam(xsp3ChanMcaRoi3LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi3LlmParam);
  createParam(xsp3ChanMcaRoi4LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi4LlmParam);
  createParam(xsp3ChanMcaRoi5LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi5LlmParam);
  createParam(xsp3ChanMcaRoi6LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi6LlmParam);
  createParam(xsp3ChanMcaRoi7LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi7LlmParam);
  createParam(xsp3ChanMcaRoi8LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi8LlmParam);
  createParam(xsp3ChanMcaRoi9LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi9LlmParam);
  createParam(xsp3ChanMcaRoi10LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi10LlmParam);
  createParam(xsp3ChanMcaRoi11LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi11LlmParam);
  createParam(xsp3ChanMcaRoi12LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi12LlmParam);
  createParam(xsp3ChanMcaRoi13LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi13LlmParam);
  createParam(xsp3ChanMcaRoi14LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi14LlmParam);
  createParam(xsp3ChanMcaRoi15LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi15LlmParam);
  createParam(xsp3ChanMcaRoi16LlmParamString,  asynParamInt32, &xsp3ChanMcaRoi16LlmParam);
  createParam(xsp3ChanMcaRoi1HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi1HlmParam);
  createParam(xsp3ChanMcaRoi2HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi2HlmParam);
  createParam(xsp3ChanMcaRoi3HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi3HlmParam);
  createParam(xsp3ChanMcaRoi4HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi4HlmParam);
  createParam(xsp3ChanMcaRoi5HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi5HlmParam);
  createParam(xsp3ChanMcaRoi6HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi6HlmParam);
  createParam(xsp3ChanMcaRoi7HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi7HlmParam);
  createParam(xsp3ChanMcaRoi8HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi8HlmParam);
  createParam(xsp3ChanMcaRoi9HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi9HlmParam);
  createParam(xsp3ChanMcaRoi10HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi10HlmParam);
  createParam(xsp3ChanMcaRoi11HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi11HlmParam);
  createParam(xsp3ChanMcaRoi12HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi12HlmParam);
  createParam(xsp3ChanMcaRoi13HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi13HlmParam);
  createParam(xsp3ChanMcaRoi14HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi14HlmParam);
  createParam(xsp3ChanMcaRoi15HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi15HlmParam);
  createParam(xsp3ChanMcaRoi16HlmParamString,  asynParamInt32, &xsp3ChanMcaRoi16HlmParam);
  createParam(xsp3ChanMcaRoi1ParamString,     asynParamFloat64,&xsp3ChanMcaRoi1Param);
  createParam(xsp3ChanMcaRoi2ParamString,     asynParamFloat64,&xsp3ChanMcaRoi2Param);
  createParam(xsp3ChanMcaRoi3ParamString,     asynParamFloat64,&xsp3ChanMcaRoi3Param);
  createParam(xsp3ChanMcaRoi4ParamString,     asynParamFloat64,&xsp3ChanMcaRoi4Param);
  createParam(xsp3ChanMcaRoi5ParamString,     asynParamFloat64,&xsp3ChanMcaRoi5Param);
  createParam(xsp3ChanMcaRoi6ParamString,     asynParamFloat64,&xsp3ChanMcaRoi6Param);
  createParam(xsp3ChanMcaRoi7ParamString,     asynParamFloat64,&xsp3ChanMcaRoi7Param);
  createParam(xsp3ChanMcaRoi8ParamString,     asynParamFloat64,&xsp3ChanMcaRoi8Param);
  createParam(xsp3ChanMcaRoi9ParamString,     asynParamFloat64,&xsp3ChanMcaRoi9Param);
  createParam(xsp3ChanMcaRoi10ParamString,     asynParamFloat64,&xsp3ChanMcaRoi10Param);
  createParam(xsp3ChanMcaRoi11ParamString,     asynParamFloat64,&xsp3ChanMcaRoi11Param);
  createParam(xsp3ChanMcaRoi12ParamString,     asynParamFloat64,&xsp3ChanMcaRoi12Param);
  createParam(xsp3ChanMcaRoi13ParamString,     asynParamFloat64,&xsp3ChanMcaRoi13Param);
  createParam(xsp3ChanMcaRoi14ParamString,     asynParamFloat64,&xsp3ChanMcaRoi14Param);
  createParam(xsp3ChanMcaRoi15ParamString,     asynParamFloat64,&xsp3ChanMcaRoi15Param);
  createParam(xsp3ChanMcaRoi16ParamString,     asynParamFloat64,&xsp3ChanMcaRoi16Param);
  createParam(xsp3ChanSca0ParamString,      asynParamFloat64,       &xsp3ChanSca0Param);
  createParam(xsp3ChanSca1ParamString,      asynParamFloat64,       &xsp3ChanSca1Param);
  createParam(xsp3ChanSca2ParamString,      asynParamFloat64,       &xsp3ChanSca2Param);
  createParam(xsp3ChanSca3ParamString,      asynParamFloat64,       &xsp3ChanSca3Param);
  createParam(xsp3ChanSca4ParamString,      asynParamFloat64,       &xsp3ChanSca4Param);
  createParam(xsp3ChanSca5ParamString,      asynParamFloat64,       &xsp3ChanSca5Param);
  createParam(xsp3ChanSca6ParamString,      asynParamFloat64,       &xsp3ChanSca6Param);
  createParam(xsp3ChanSca7ParamString,      asynParamFloat64,       &xsp3ChanSca7Param);
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
  //These controls calculations
  createParam(xsp3RoiEnableParamString,         asynParamInt32,       &xsp3RoiEnableParam);
  createParam(xsp3DtcEnableParamString,         asynParamInt32,       &xsp3DtcEnableParam);
  createParam(xsp3LastParamString,         asynParamInt32,       &xsp3LastParam);
  
  //Initialize non static, non const, data members
  acquiring_ = 0;
  running_ = 0;
  xsp3_handle_ = 0;
  
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
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi1LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi2LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi3LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi4LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi5LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi6LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi7LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi8LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi9LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi10LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi11LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi12LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi13LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi14LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi15LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi16LlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi1HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi2HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi3HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi4HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi5HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi6HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi7HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi8HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi9HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi10HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi11HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi12HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi13HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi14HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi15HlmParam, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setIntegerParam(chan, xsp3ChanMcaRoi16HlmParam, 0) == asynSuccess) && paramStatus);
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
  paramStatus = ((eraseSCAMCAROI() == asynSuccess) && paramStatus);

  if (simTest_) {
    paramStatus = ((setStringParam(ADStatusMessage, "Init. Simulation Mode.") == asynSuccess) && paramStatus);
  } else {
    paramStatus = ((setStringParam(ADStatusMessage, "Init. System Disconnected.") == asynSuccess) && paramStatus);
  }
  
  callParamCallbacks();

  if (!paramStatus) {
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

  xsp3_handle_ = xsp3_config(xsp3_num_cards, xsp3_num_tf, const_cast<char *>(baseIP_.c_str()), -1, NULL, xsp3_num_channels, 1, NULL, debug_, 0);
  if (xsp3_handle_ < 0) {
    checkStatus(xsp3_handle_, "xsp3_config", functionName);
    status = asynError;
  } else {

    //Set up clocks on each card
    for (int i=0; i<xsp3_num_cards; i++) {
      xsp3_status = xsp3_clocks_setup(xsp3_handle_, i, XSP3_CLK_SRC_XTAL,
                                      XSP3_CLK_FLAGS_MASTER | XSP3_CLK_FLAGS_DITHER, 0);
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
  
  //Can we do xsp3_format_run here? For normal user operation all the arguments seem to be set to zero.
  int xsp3_num_channels;
  getIntegerParam(xsp3NumChannelsParam, &xsp3_num_channels);
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
  int xsp3_run_flags;
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

  //TODO:
  // Call format_run, run_flags, timing_globA, scalar_windows
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
 
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi1Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi2Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi3Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi4Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi5Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi6Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi7Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi8Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi9Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi10Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi11Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi12Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi13Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi14Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi15Param, 0) == asynSuccess) && paramStatus);
    paramStatus = ((setDoubleParam(chan, xsp3ChanMcaRoi16Param, 0) == asynSuccess) && paramStatus);
    
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
	    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Starting Data Collection.\n", functionName);
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
              asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Started Data Collection.\n", functionName);
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
      status = mapTriggerMode(trigger_mode, invert_f0, invert_veto, debounce, &xsp3_trigger_mode);

      if (status != asynError) {
          asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set Trigger Mode.\n", functionName);
          getIntegerParam(xsp3NumCardsParam, &xsp3_num_cards);
          for (int card=0; card<xsp3_num_cards; card++) {
              xsp3_status = xsp3_set_glob_timeA(xsp3_handle_, card, xsp3_trigger_mode);
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
  else if (function == xsp3TriggerParam) {
    if ((status = checkConnected()) == asynSuccess && adStatus == ADStatusAcquire && value) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Triggering Next Frame.\n", functionName);
      getIntegerParam(xsp3NumCardsParam, &xsp3_num_cards);
      for (int card=0; card<xsp3_num_cards; card++) {
          xsp3_status = xsp3_histogram_continue(xsp3_handle_, card);
          if (xsp3_status != XSP3_OK) {
              checkStatus(xsp3_status, "xsp3_histogram_continue", functionName);
              status = asynError;
          }
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
  //ROI low limits
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
  else if (function == xsp3ChanMcaRoi5LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI5 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi5HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 5, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi6LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI6 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi6HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 6, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi7LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI7 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi7HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 7, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi8LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI8 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi8HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 8, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi9LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI9 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi9HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 9, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi10LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI10 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi10HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 10, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi11LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI11 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi11HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 11, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi12LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI12 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi12HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 12, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi13LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI13 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi13HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 13, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi14LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI14 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi14HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 14, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi15LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI15 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi15HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 15, value, xsp3_roi_lim); 
  }
  else if (function == xsp3ChanMcaRoi16LlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI16 Low Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi16HlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 16, value, xsp3_roi_lim); 
  }
  //ROI high limits
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
  else if (function == xsp3ChanMcaRoi5HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI5 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi5LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 5, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi6HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI6 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi6LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 6, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi7HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI7 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi7LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 7, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi8HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI8 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi8LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 8, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi9HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI9 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi9LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 9, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi10HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI10 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi10LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 10, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi11HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI11 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi11LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 11, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi12HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI12 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi12LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 12, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi13HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI13 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi13LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 13, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi14HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI14 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi14LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 14, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi15HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI15 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi15LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 15, xsp3_roi_lim, value); 
  }
  else if (function == xsp3ChanMcaRoi16HlmParam) {
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Set The MCA ROI16 High Limit.\n", functionName);
    getIntegerParam(addr, xsp3ChanMcaRoi16LlmParam, &xsp3_roi_lim);
    status = checkRoi(addr, 16, xsp3_roi_lim, value); 
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
  epicsFloat64 timeout = 0.001;
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
  epicsTimeStamp nowTime;
  const char* functionName = "Xspress3::dataTask";
  epicsFloat64 *pSCA;
  epicsFloat64 *pMCA;
  epicsInt32 *pSCA_INT;
  epicsInt32 *pMCA_INT;
  NDArray *pMCA_NDARRAY;
  int roiEnabled = 0;
  epicsInt32 roiMin[maxNumRoi_];
  epicsInt32 roiMax[maxNumRoi_];
  epicsFloat64 roiSum[maxNumRoi_];
  int allocError = 0;
  int dumpOffset = 0;
  int lastFrameCount = 0;
  int framesToReadOut = 0;
  bool stillBusy = false;
  int dtcEnable = 0;
  bool aborted = false;
  bool completed = false;

  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Started Data Thread.\n", functionName);

  //Create array for scalar data (max frame * number of SCAs).
  getIntegerParam(xsp3NumFramesDriverParam, &maxNumFrames);
  getIntegerParam(xsp3MaxSpectraParam, &maxSpectra);
  pSCA = static_cast<epicsFloat64*>(calloc(XSP3_SW_NUM_SCALERS*maxNumFrames*numChannels_, sizeof(epicsFloat64)));
  assert( pSCA != NULL );
  pSCA_INT = static_cast<epicsInt32*>(calloc(XSP3_SW_NUM_SCALERS*maxNumFrames*numChannels_, sizeof(epicsInt32)));
  assert( pSCA_INT != NULL );

  //Create data arrays for MCA spectra
  pMCA = static_cast<epicsFloat64*>(calloc(maxSpectra*numChannels_*maxNumFrames, sizeof(epicsFloat64)));
  assert( pMCA != NULL );
  pMCA_INT = static_cast<epicsInt32*>(calloc(maxSpectra*numChannels_*maxNumFrames, sizeof(epicsInt32)));
  assert( pMCA_INT != NULL );

  //Code to set CPU affinitiy.
  //This doesn't prevent the data readout thread being pre-empted by visualization threads. Need to set real time priority too. 
  /*int core_id = 0; //This sets the CPU core
  int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
  if (core_id < num_cores) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    pthread_t current_thread = pthread_self();    
    int return_val = pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
    if (return_val != 0) {
       asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s Error setting CPU affinity for data readout thread.\n", functionName);
    }
    }*/


  cout << "Data readout thread PID: " << getpid() << endl;
  cout << "Data readout thread TID syscall(SYS_gettid): " << syscall(SYS_gettid) << endl;

  //Code to set SCHED_FIFO for this thread. Not tested yet.
  /*struct sched_param param;
  int priority = 10;
  int policy = -999;

  pthread_getschedparam(pthread_self(),&policy,&param);
  printf("Thread policy before %d \n" ,policy);

  param.sched_priority = priority;
  policy = SCHED_FIFO;
  // scheduling parameters of target thread
  pthread_setschedparam(pthread_self(), policy, &param);
  pthread_getschedparam(pthread_self(),&policy,&param);
  printf("Thread policy after %d \n", policy);
   */


  while (1) {

    //Wait for a stop event, with a short timeout, to catch any that were done during last readout.
    eventStatus = epicsEventWaitWithTimeout(stopEvent_, timeout);          
    if (eventStatus == epicsEventWaitOK) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Got Stop Event Before Start Event.\n", functionName);
    }

    setIntegerParam(ADAcquire, 0);
    callParamCallbacks();

    eventStatus = epicsEventWait(startEvent_);          
    if (eventStatus == epicsEventWaitOK) {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Got Start Event.\n", functionName);
      acquire = 1;
      frameCounter = 0;
      aborted = false;
      completed = false;
      lock();
      setIntegerParam(NDArrayCounter, 0);
      //Need to clear local arrays here for each new acqusition.
      memset(pSCA, 0, (XSP3_SW_NUM_SCALERS*maxNumFrames*numChannels_)*sizeof(epicsFloat64));
      memset(pSCA_INT, 0, (XSP3_SW_NUM_SCALERS*maxNumFrames*numChannels_)*sizeof(epicsInt32));
      memset(pMCA, 0, (maxSpectra*numChannels_*maxNumFrames)*sizeof(epicsFloat64));
      memset(pMCA_INT, 0, (maxSpectra*numChannels_*maxNumFrames)*sizeof(epicsInt32));
      setIntegerParam(xsp3FrameCountParam, 0);
      setIntegerParam(NDArrayCounter, 0);
      setIntegerParam(ADStatus, ADStatusAcquire);
      setStringParam(ADStatusMessage, "Acquiring Data");
      callParamCallbacks();
    }

    dumpOffset = 0;
    lastFrameCount = 0;
    framesToReadOut = 0;
    stillBusy = false;

    //Get the number of channels actually in use.
    getIntegerParam(xsp3NumChannelsParam, &numChannels);
    //Read how many frames we want to read out before stopping.
    getIntegerParam(ADNumImages, &numFrames);
    //Do we want corrected or uncorrected data?
    getIntegerParam(xsp3DtcEnableParam, &dtcEnable);

    while (acquire) {
      setIntegerParam(ADStatus, ADStatusAcquire);
      unlock();

      //Wait for a stop event, with a short timeout.
      eventStatus = epicsEventWaitWithTimeout(stopEvent_, timeout);          
      if (eventStatus == epicsEventWaitOK) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Got Stop Event.\n", functionName);
        acquire = 0;
        aborted = true;
      }
      lock();

      //If we have stopped, wait until we are not busy on all channels.
      stillBusy = false;
      if (!simTest_) {
        if (!acquire) {
          if (checkHistBusy(maxCheckHistPolls_) == asynError) {
            stillBusy = true;
          }
          asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s stillBusy: %s.\n", functionName, (stillBusy?"true":"false"));
        }
      }

      //Read how many data frames have been transferred.
      if (acquire || aborted) {
        if (!simTest_) {
          getIntegerParam(xsp3NumCardsParam, &xsp3_num_cards);
          for (int card=0; card<xsp3_num_cards; card++) {
            xsp3_status = xsp3_scaler_check_desc(xsp3_handle_, card);
            if (xsp3_status < XSP3_OK) {
              checkStatus(xsp3_status, "xsp3_dma_check_desc", functionName);
              status = asynError;
            }
            frame_count = xsp3_status;
            setIntegerParam(xsp3FrameCountParam, frame_count-lastFrameCount);
            asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s frame_count: %d.\n", functionName, frame_count);
          }
        } else {
          //In sim mode we transfer 10 frames each time
          frame_count = lastFrameCount+10;
          setIntegerParam(xsp3FrameCountParam, 10);
        }
      } else {
        frame_count = 0;
      }

      if (frame_count > lastFrameCount) {
        framesToReadOut = frame_count - lastFrameCount;
        lastFrameCount = frame_count;

        getIntegerParam(NDArrayCounter, &frameCounter);
        frameCounter += framesToReadOut;
        int remainingFrames = framesToReadOut;
        //Check we are not overflowing or reading too many frames.
        if (frameCounter > maxNumFrames) {
          remainingFrames = maxNumFrames - (frameCounter - framesToReadOut);
          asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s ERROR: Stopping Acqusition. We Reached The Max Num Of Frames.\n", functionName);
          setStringParam(ADStatusMessage, "Stopped. Max Frames Reached.");
          setIntegerParam(ADAcquire, ADAcquireFalse_);
          if (!simTest_) {
            xsp3_status = xsp3_histogram_stop(xsp3_handle_, 0);
            if (xsp3_status != XSP3_OK) {
              checkStatus(xsp3_status, "xsp3_histogram_stop", functionName);
            }
          }
          acquire=0;
          setIntegerParam(ADStatus, ADStatusAborted);
        } else if (frameCounter >= numFrames) {
          completed = true;
          remainingFrames = numFrames - (frameCounter - framesToReadOut);
          if (!simTest_) {
            xsp3_status = xsp3_histogram_stop(xsp3_handle_, 0);
            if (xsp3_status != XSP3_OK) {
              checkStatus(xsp3_status, "xsp3_histogram_stop", functionName);
            }
          }
          acquire=0;
        }

        int frameOffset = frameCounter-framesToReadOut;
        if (!acquire) {
          frameCounter = frameOffset+remainingFrames;
        }

        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s frame_count: %d.\n", functionName, frame_count);
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s framesToReadOut: %d.\n", functionName, framesToReadOut);
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s frameCounter: %d.\n", functionName, frameCounter);
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s remainingFrames: %d.\n", functionName, remainingFrames);
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s frameOffset: %d.\n", functionName, frameOffset);

        if (acquire || aborted) {
          setIntegerParam(ADStatus, ADStatusReadout);
          callParamCallbacks();
        }

        epicsFloat64 *pData = NULL;
        //Readout multiple frames of data here into local arrays.
        if ((!stillBusy) && (remainingFrames != 0)) {
 
          if (!simTest_) {
            //Read out the MCA spectra and scaler data (dtc or non-corrected)
            if (dtcEnable == 1) {
              xsp3_status = xsp3_hist_dtc_read4d(xsp3_handle_, reinterpret_cast<double*>(pMCA), reinterpret_cast<double*>(pSCA), 0, 0, 0, frameOffset, maxSpectra, 1, numChannels, remainingFrames);
              if (xsp3_status != XSP3_OK) {
                checkStatus(xsp3_status, "xsp3_hist_dtc_read4d", functionName);
                status = asynError;
              }
            } else {
              xsp3_status = xsp3_histogram_read4d(xsp3_handle_, reinterpret_cast<u_int32_t*>(pMCA_INT), 0, 0, 0, frameOffset, maxSpectra, 1, numChannels, remainingFrames);
              if (xsp3_status != XSP3_OK) {
                checkStatus(xsp3_status, "xsp3_histogram_read4d", functionName);
                status = asynError;
              }
              xsp3_status = xsp3_scaler_read(xsp3_handle_, reinterpret_cast<u_int32_t*>(pSCA_INT), 0, 0, frameOffset, XSP3_SW_NUM_SCALERS, numChannels, remainingFrames);
              if (xsp3_status != XSP3_OK) {
                checkStatus(xsp3_status, "xsp3_scaler_read", functionName);
                status = asynError;
              }

              //If reading un-corrected data, need to convert to doubles for the rest of the IOC code.
    
              epicsFloat64 *pDATA = pMCA;
              epicsInt32 *pDATA_INT = pMCA_INT;
              epicsFloat64 *pSCA_DATA = pSCA;
              epicsInt32 *pSCA_DATA_INT = pSCA_INT;
              for (int frame=frameOffset; frame<(frameOffset+remainingFrames); ++frame) {
                for (int chan=0; chan<numChannels; ++chan) {
                  for (int bin=0; bin<maxSpectra; ++bin) {
                    *(pDATA++) = static_cast<epicsFloat64>(*(pDATA_INT++));
                  }
                  for (int sca=0; sca<XSP3_SW_NUM_SCALERS; ++sca) {
                    *(pSCA_DATA++) = static_cast<epicsFloat64>(*(pSCA_DATA_INT++));
                  }
                }
              }
            }
          } else {
            //If in simMode_, fill the MCA array with sim data
            epicsFloat64 *pMCA_DATA = pMCA;
            for (int frame=frameOffset; frame<(frameOffset+remainingFrames); ++frame) {
              for (int chan=0; chan<numChannels; ++chan) {
                for (int i=0; i<maxSpectra; ++i) {
                  //Generate a sine wave, with an element of randomness in it.
                  *(pMCA_DATA++) = sin(static_cast<epicsFloat64>(i)/90.0)*((rand()%20)+100);
                }
              }
            }

            //Fill the Scaler array with dummy data in simTest_ mode
            pData = pSCA;
            for (int i=0; i<(XSP3_SW_NUM_SCALERS*maxNumFrames*numChannels); ++i) {
              *(pData++) = i;
            }
          }

        }

        if (acquire) {
          setIntegerParam(ADStatus, ADStatusAcquire);
        }

        size_t dims[2] = {maxSpectra, numChannels};
        epicsFloat64 *pScaData = pSCA;

        //For each frame, do the ROI and pack into an NDArray object
        if (!stillBusy) {
          int currentFrameOffset = 0;
          for (int frame=frameOffset; frame<(frameOffset+remainingFrames); ++frame) {

            allocError = 0;
            setIntegerParam(NDArrayCounter, frame+1);

            asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s absolute frame: %d.\n", functionName, frame);
            asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s frame number for this readout currentFrameOffset: %d\n", functionName, currentFrameOffset);

            //NDArray to hold the all channels spectra and attributes for each frame.
            if ((pMCA_NDARRAY = this->pNDArrayPool->alloc(2, dims, NDFloat64, 0, NULL)) == NULL) {
              asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s: ERROR: pNDArrayPool->alloc failed.\n", functionName);
              setStringParam(ADStatusMessage, "Memory Error. Check IOC Log.");
              setIntegerParam(ADStatus, ADStatusError);
              setIntegerParam(ADAcquire, ADAcquireFalse_);
              acquire = 0;
              allocError = 1;
            } else {

              for (int chan=0; chan<numChannels; ++chan) {

                setDoubleParam(chan, xsp3ChanSca0Param, *(pScaData++));
                setDoubleParam(chan, xsp3ChanSca1Param, *(pScaData++));
                setDoubleParam(chan, xsp3ChanSca2Param, *(pScaData++));
                setDoubleParam(chan, xsp3ChanSca3Param, *(pScaData++));
                setDoubleParam(chan, xsp3ChanSca4Param, *(pScaData++));
                setDoubleParam(chan, xsp3ChanSca5Param, *(pScaData++));
                setDoubleParam(chan, xsp3ChanSca6Param, *(pScaData++));
                setDoubleParam(chan, xsp3ChanSca7Param, *(pScaData++));

                //Calculate MCA ROI here, if we have enabled it. Put the results into pMCA_ROI[chan][roi].
                getIntegerParam(xsp3RoiEnableParam, &roiEnabled);
                memset(&roiSum, 0, sizeof(epicsFloat64)*maxNumRoi_);
                if (roiEnabled) {

                  getIntegerParam(chan, xsp3ChanMcaRoi1LlmParam, &roiMin[0]);
                  getIntegerParam(chan, xsp3ChanMcaRoi1HlmParam, &roiMax[0]);
                  getIntegerParam(chan, xsp3ChanMcaRoi2LlmParam, &roiMin[1]);
                  getIntegerParam(chan, xsp3ChanMcaRoi2HlmParam, &roiMax[1]);
                  getIntegerParam(chan, xsp3ChanMcaRoi3LlmParam, &roiMin[2]);
                  getIntegerParam(chan, xsp3ChanMcaRoi3HlmParam, &roiMax[2]);
                  getIntegerParam(chan, xsp3ChanMcaRoi4LlmParam, &roiMin[3]);
                  getIntegerParam(chan, xsp3ChanMcaRoi4HlmParam, &roiMax[3]);
                  getIntegerParam(chan, xsp3ChanMcaRoi5LlmParam, &roiMin[4]);
                  getIntegerParam(chan, xsp3ChanMcaRoi5HlmParam, &roiMax[4]);
                  getIntegerParam(chan, xsp3ChanMcaRoi6LlmParam, &roiMin[5]);
                  getIntegerParam(chan, xsp3ChanMcaRoi6HlmParam, &roiMax[5]);
                  getIntegerParam(chan, xsp3ChanMcaRoi7LlmParam, &roiMin[6]);
                  getIntegerParam(chan, xsp3ChanMcaRoi7HlmParam, &roiMax[6]);
                  getIntegerParam(chan, xsp3ChanMcaRoi8LlmParam, &roiMin[7]);
                  getIntegerParam(chan, xsp3ChanMcaRoi8HlmParam, &roiMax[7]);
                  getIntegerParam(chan, xsp3ChanMcaRoi9LlmParam, &roiMin[8]);
                  getIntegerParam(chan, xsp3ChanMcaRoi9HlmParam, &roiMax[8]);
                  getIntegerParam(chan, xsp3ChanMcaRoi10LlmParam, &roiMin[9]);
                  getIntegerParam(chan, xsp3ChanMcaRoi10HlmParam, &roiMax[9]);
                  getIntegerParam(chan, xsp3ChanMcaRoi11LlmParam, &roiMin[10]);
                  getIntegerParam(chan, xsp3ChanMcaRoi11HlmParam, &roiMax[10]);
                  getIntegerParam(chan, xsp3ChanMcaRoi12LlmParam, &roiMin[11]);
                  getIntegerParam(chan, xsp3ChanMcaRoi12HlmParam, &roiMax[11]);
                  getIntegerParam(chan, xsp3ChanMcaRoi13LlmParam, &roiMin[12]);
                  getIntegerParam(chan, xsp3ChanMcaRoi13HlmParam, &roiMax[12]);
                  getIntegerParam(chan, xsp3ChanMcaRoi14LlmParam, &roiMin[13]);
                  getIntegerParam(chan, xsp3ChanMcaRoi14HlmParam, &roiMax[13]);
                  getIntegerParam(chan, xsp3ChanMcaRoi15LlmParam, &roiMin[14]);
                  getIntegerParam(chan, xsp3ChanMcaRoi15HlmParam, &roiMax[14]);
                  getIntegerParam(chan, xsp3ChanMcaRoi16LlmParam, &roiMin[15]);
                  getIntegerParam(chan, xsp3ChanMcaRoi16HlmParam, &roiMax[15]);

                  asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Calculating ROI Data.\n", functionName);

                  int roiOffset = ((maxSpectra*chan) + (maxSpectra*numChannels*currentFrameOffset));
                  epicsFloat64 *pMCA_DATA = pMCA+roiOffset;
                  for (int roi=0; roi<maxNumRoi_; ++roi) {
                    for (int energy=roiMin[roi]; energy<roiMax[roi]; ++energy) {
                      roiSum[roi] += *(pMCA_DATA+energy);
                    }
                  }
                }

                asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Setting ROI Param Data.\n", functionName);
                setDoubleParam(chan, xsp3ChanMcaRoi1Param, roiSum[0]);
                setDoubleParam(chan, xsp3ChanMcaRoi2Param, roiSum[1]);
                setDoubleParam(chan, xsp3ChanMcaRoi3Param, roiSum[2]);
                setDoubleParam(chan, xsp3ChanMcaRoi4Param, roiSum[3]);
                setDoubleParam(chan, xsp3ChanMcaRoi5Param, roiSum[4]);
                setDoubleParam(chan, xsp3ChanMcaRoi6Param, roiSum[5]);
                setDoubleParam(chan, xsp3ChanMcaRoi7Param, roiSum[6]);
                setDoubleParam(chan, xsp3ChanMcaRoi8Param, roiSum[7]);
                setDoubleParam(chan, xsp3ChanMcaRoi9Param, roiSum[8]);
                setDoubleParam(chan, xsp3ChanMcaRoi10Param, roiSum[9]);
                setDoubleParam(chan, xsp3ChanMcaRoi11Param, roiSum[10]);
                setDoubleParam(chan, xsp3ChanMcaRoi12Param, roiSum[11]);
                setDoubleParam(chan, xsp3ChanMcaRoi13Param, roiSum[12]);
                setDoubleParam(chan, xsp3ChanMcaRoi14Param, roiSum[13]);
                setDoubleParam(chan, xsp3ChanMcaRoi15Param, roiSum[14]);
                setDoubleParam(chan, xsp3ChanMcaRoi16Param, roiSum[15]);

              } //End of channel loop
            } //end of else, if the alloc worked.

            //Pack the MCA data into an NDArray for this frame.   Format is: [chan1 spectra][chan2 spectra][etc]
            if (allocError == 0) {
              int frame_offset = maxSpectra*numChannels*currentFrameOffset;
              for (int chan=0; chan<numChannels; ++chan) {
                int chan_offset = chan*maxSpectra;
                epicsFloat64 *pMCA_DATA = pMCA+frame_offset;
                memcpy(reinterpret_cast<epicsFloat64*>(pMCA_NDARRAY->pData)+chan_offset, pMCA_DATA+chan_offset, maxSpectra*sizeof(epicsFloat64));
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

            //Need to call callParamCallbacks for each list (ie. channel, indexed by Asyn address).
            for (int addr=0; addr<maxAddr; addr++) {
              asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s Calling callParamCallbacks for addr %d.\n", functionName, addr);
              callParamCallbacks(addr);
            }

            ++currentFrameOffset;

          } //end of frame loop
        } //end of if (!stillBusy)

      }  //end of if (frame_count > lastFrameCount)

      if (aborted) {
        setIntegerParam(ADAcquire, ADAcquireFalse_);
        setIntegerParam(ADStatus, ADStatusAborted);
        setStringParam(ADStatusMessage, "Stopped Acquiring");
        callParamCallbacks();
      }

      if (completed) {
        setStringParam(ADStatusMessage, "Completed Acqusition.");
        setIntegerParam(ADAcquire, ADAcquireFalse_);
        setIntegerParam(ADStatus, ADStatusIdle);
      }

      frame_count = 0;
      framesToReadOut = 0;

    } //end of while(acquire)

    unlock();

  } //end of while(1)

  asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s: ERROR: Exiting dataTask main loop.\n", functionName);
  free(pSCA);
  free(pMCA);
  free(pMCA_INT);

}


//Global C utility functions to tie in with EPICS
static void xsp3DataTaskC(void *drvPvt)
{
  Xspress3 *pPvt = (Xspress3 *)drvPvt;
  
  pPvt->dataTask();
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

