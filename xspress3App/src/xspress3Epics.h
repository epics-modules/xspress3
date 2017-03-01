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

#ifndef XSPRESS3_H
#define XSPRESS3_H

#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsMutex.h>
#include <epicsString.h>
#include <epicsStdio.h>
#include <cantProceed.h>
#include <epicsTypes.h>

#include <asynOctetSyncIO.h>

#include "ADDriver.h"

#include "xsp3Detector.h"
#include "xsp3Simulator.h"

/* These are the drvInfo strings that are used to identify the parameters.
 * They are used by asyn clients, including standard asyn device support */
//System wide settings
#define xsp3FirstParamString              "XSP3_FIRST"
#define xsp3LastParamString              "XSP3_LAST"
#define xsp3ResetParamString              "XSP3_RESET"
#define xsp3EraseParamString              "XSP3_ERASE"
#define xsp3NumChannelsParamString        "XSP3_NUM_CHANNELS"
#define xsp3MaxNumChannelsParamString     "XSP3_MAX_NUM_CHANNELS"
#define xsp3MaxSpectraParamString     "XSP3_MAX_SPECTRA"
#define xsp3MaxFramesParamString     "XSP3_MAX_FRAMES"
#define xsp3FrameCountParamString     "XSP3_FRAME_COUNT"
#define xsp3TriggerModeParamString        "XSP3_TRIGGER_MODE"
#define xsp3FixedTimeParamString        "XSP3_FIXED_TIME"
#define xsp3NumFramesConfigParamString          "XSP3_NUM_FRAMES_CONFIG"
#define xsp3NumFramesDriverParamString          "XSP3_NUM_FRAMES_DRIVER"
#define xsp3NumCardsParamString           "XSP3_NUM_CARDS"
#define xsp3ConfigPathParamString           "XSP3_CONFIG_PATH"
#define xsp3ConfigSavePathParamString           "XSP3_CONFIG_SAVE_PATH"
#define xsp3ConnectParamString           "XSP3_CONNECT"
#define xsp3ConnectedParamString           "XSP3_CONNECTED"
#define xsp3DisconnectParamString           "XSP3_DISCONNECT"
#define xsp3SaveSettingsParamString           "XSP3_SAVE_SETTINGS"
#define xsp3RestoreSettingsParamString           "XSP3_RESTORE_SETTINGS"
#define xsp3RunFlagsParamString           "XSP3_RUN_FLAGS"
#define xsp3TriggerParamString           "XSP3_TRIGGER"
#define xsp3InvertF0ParamString           "XSP3_INVERT_F0"
#define xsp3InvertVetoParamString           "XSP3_INVERT_VETO"
#define xsp3DebounceParamString           "XSP3_DEBOUNCE"
#define xsp3PulsePerTriggerParamString           "XSP3_PPTRIGGER"
#define xsp3ITFGStartParamString           "XSP3_ITFG_START"
#define xsp3ITFGStopParamString           "XSP3_ITFG_STOP"
//Settings for a channel
#define xsp3ChanMcaRoi1LlmParamString     "XSP3_CHAN_ROI1_LLM"
#define xsp3ChanMcaRoi2LlmParamString     "XSP3_CHAN_ROI2_LLM"
#define xsp3ChanMcaRoi3LlmParamString     "XSP3_CHAN_ROI3_LLM"
#define xsp3ChanMcaRoi4LlmParamString     "XSP3_CHAN_ROI4_LLM"
#define xsp3ChanMcaRoi5LlmParamString     "XSP3_CHAN_ROI5_LLM"
#define xsp3ChanMcaRoi6LlmParamString     "XSP3_CHAN_ROI6_LLM"
#define xsp3ChanMcaRoi7LlmParamString     "XSP3_CHAN_ROI7_LLM"
#define xsp3ChanMcaRoi8LlmParamString     "XSP3_CHAN_ROI8_LLM"
#define xsp3ChanMcaRoi9LlmParamString     "XSP3_CHAN_ROI9_LLM"
#define xsp3ChanMcaRoi10LlmParamString     "XSP3_CHAN_ROI10_LLM"
#define xsp3ChanMcaRoi11LlmParamString     "XSP3_CHAN_ROI11_LLM"
#define xsp3ChanMcaRoi12LlmParamString     "XSP3_CHAN_ROI12_LLM"
#define xsp3ChanMcaRoi13LlmParamString     "XSP3_CHAN_ROI13_LLM"
#define xsp3ChanMcaRoi14LlmParamString     "XSP3_CHAN_ROI14_LLM"
#define xsp3ChanMcaRoi15LlmParamString     "XSP3_CHAN_ROI15_LLM"
#define xsp3ChanMcaRoi16LlmParamString     "XSP3_CHAN_ROI16_LLM"
#define xsp3ChanMcaRoi1HlmParamString     "XSP3_CHAN_ROI1_HLM"
#define xsp3ChanMcaRoi2HlmParamString     "XSP3_CHAN_ROI2_HLM"
#define xsp3ChanMcaRoi3HlmParamString     "XSP3_CHAN_ROI3_HLM"
#define xsp3ChanMcaRoi4HlmParamString     "XSP3_CHAN_ROI4_HLM"
#define xsp3ChanMcaRoi5HlmParamString     "XSP3_CHAN_ROI5_HLM"
#define xsp3ChanMcaRoi6HlmParamString     "XSP3_CHAN_ROI6_HLM"
#define xsp3ChanMcaRoi7HlmParamString     "XSP3_CHAN_ROI7_HLM"
#define xsp3ChanMcaRoi8HlmParamString     "XSP3_CHAN_ROI8_HLM"
#define xsp3ChanMcaRoi9HlmParamString     "XSP3_CHAN_ROI9_HLM"
#define xsp3ChanMcaRoi10HlmParamString     "XSP3_CHAN_ROI10_HLM"
#define xsp3ChanMcaRoi11HlmParamString     "XSP3_CHAN_ROI11_HLM"
#define xsp3ChanMcaRoi12HlmParamString     "XSP3_CHAN_ROI12_HLM"
#define xsp3ChanMcaRoi13HlmParamString     "XSP3_CHAN_ROI13_HLM"
#define xsp3ChanMcaRoi14HlmParamString     "XSP3_CHAN_ROI14_HLM"
#define xsp3ChanMcaRoi15HlmParamString     "XSP3_CHAN_ROI15_HLM"
#define xsp3ChanMcaRoi16HlmParamString     "XSP3_CHAN_ROI16_HLM"
#define xsp3ChanMcaRoi1ParamString        "XSP3_CHAN_ROI1"
#define xsp3ChanMcaRoi2ParamString        "XSP3_CHAN_ROI2"
#define xsp3ChanMcaRoi3ParamString        "XSP3_CHAN_ROI3"
#define xsp3ChanMcaRoi4ParamString        "XSP3_CHAN_ROI4"
#define xsp3ChanMcaRoi5ParamString        "XSP3_CHAN_ROI5"
#define xsp3ChanMcaRoi6ParamString        "XSP3_CHAN_ROI6"
#define xsp3ChanMcaRoi7ParamString        "XSP3_CHAN_ROI7"
#define xsp3ChanMcaRoi8ParamString        "XSP3_CHAN_ROI8"
#define xsp3ChanMcaRoi9ParamString        "XSP3_CHAN_ROI9"
#define xsp3ChanMcaRoi10ParamString        "XSP3_CHAN_ROI10"
#define xsp3ChanMcaRoi11ParamString        "XSP3_CHAN_ROI11"
#define xsp3ChanMcaRoi12ParamString        "XSP3_CHAN_ROI12"
#define xsp3ChanMcaRoi13ParamString        "XSP3_CHAN_ROI13"
#define xsp3ChanMcaRoi14ParamString        "XSP3_CHAN_ROI14"
#define xsp3ChanMcaRoi15ParamString        "XSP3_CHAN_ROI15"
#define xsp3ChanMcaRoi16ParamString        "XSP3_CHAN_ROI16"
#define xsp3ChanSca0ParamString           "XSP3_CHAN_SCA0"
#define xsp3ChanSca1ParamString           "XSP3_CHAN_SCA1"
#define xsp3ChanSca2ParamString           "XSP3_CHAN_SCA2"
#define xsp3ChanSca3ParamString           "XSP3_CHAN_SCA3"
#define xsp3ChanSca4ParamString           "XSP3_CHAN_SCA4"
#define xsp3ChanSca5ParamString           "XSP3_CHAN_SCA5"
#define xsp3ChanSca6ParamString           "XSP3_CHAN_SCA6"
#define xsp3ChanSca7ParamString           "XSP3_CHAN_SCA7"
#define xsp3ChanSca4ThresholdParamString        "XSP3_CHAN_SCA4_THRESHOLD"
#define xsp3ChanSca5HlmParamString        "XSP3_CHAN_SCA5_HLM"
#define xsp3ChanSca6HlmParamString        "XSP3_CHAN_SCA6_HLM"
#define xsp3ChanSca5LlmParamString        "XSP3_CHAN_SCA5_LLM"
#define xsp3ChanSca6LlmParamString        "XSP3_CHAN_SCA6_LLM"
#define xsp3ChanDtcFlagsParamString            "XSP3_CHAN_DTC_FLAGS"
#define xsp3ChanDtcAegParamString            "XSP3_CHAN_DTC_AEG"
#define xsp3ChanDtcAeoParamString            "XSP3_CHAN_DTC_AEO"
#define xsp3ChanDtcIwgParamString            "XSP3_CHAN_DTC_IWG"
#define xsp3ChanDtcIwoParamString            "XSP3_CHAN_DTC_IWO"
//Params to enable or disable calculations
#define xsp3RoiEnableParamString        "XSP3_CTRL_MCA_ROI"
#define xsp3DtcEnableParamString        "XSP3_CTRL_DTC"
#define xsp3EventWidthParamString        "XSP3_EVENT_WIDTH"

#define xsp3ChanDTPercentParamString     "XSP3_CHAN_DTPERCENT"
#define xsp3ChanDTFactorParamString      "XSP3_CHAN_DTFACTOR"



extern "C" {
  int xspress3Config(const char *portName, int numChannels, int numCards, const char *baseIP, int maxFrames, int maxDriverFrames, int maxSpectra, int maxBuffers, size_t maxMemory, int debug, int simTest);
}


class Xspress3 : public ADDriver {

 public:
  Xspress3(const char *portName, int numChannels, int numCards, const char *baseIP, int maxFrames, int maxDriverFrames, int maxSpectra, int maxBuffers, size_t maxMemory, int debug, int simTest);
  virtual ~Xspress3();

  /* These are the methods that we override from asynPortDriver */
  virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
  virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
  virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, 
                                    size_t nChars, size_t *nActual);
  virtual void report(FILE *fp, int details);

  void dataTask(void);

 private:

  //Put private functions here
  void checkStatus(int status, const char *function, const char *parentFunction);
  asynStatus connect(void);
  asynStatus disconnect(void);
  asynStatus saveSettings(void);
  asynStatus restoreSettings(void);
  asynStatus checkConnected(void);
  asynStatus setWindow(int channel, int sca, int llm, int hlm);
  asynStatus checkRoi(int channel, int roi, int llm, int hlm);
  asynStatus erase(void);
  asynStatus eraseSCAMCAROI(void);
  asynStatus checkSaveDir(const char *dirName);
  asynStatus readSCAParams(void);
  asynStatus readDTCParams(void); 
  asynStatus readTrigB(void);
  asynStatus checkHistBusy(int checkTimes);
  asynStatus setupITFG(void); 
  asynStatus mapTriggerMode(int mode, int invert_f0, int invert_veto, int debounce, int *apiMode);
  asynStatus setTriggerMode(int mode, int invert_f0, int invert_veto, int debounce );

  //Put private static data members here
  static const epicsInt32 ctrlDisable_;
  static const epicsInt32 ctrlEnable_;
  static const epicsInt32 runFlag_MCA_SPECTRA_;
  static const epicsInt32 runFlag_PLAYB_MCA_SPECTRA_;
  static const epicsInt32 maxNumRoi_;
  static const epicsInt32 maxStringSize_;
  static const epicsInt32 maxCheckHistPolls_;
  static const epicsInt32 mbboTriggerFIXED_;
  static const epicsInt32 mbboTriggerINTERNAL_;
  static const epicsInt32 mbboTriggerIDC_;
  static const epicsInt32 mbboTriggerTTLVETO_;
  static const epicsInt32 mbboTriggerTTLBOTH_;
  static const epicsInt32 mbboTriggerLVDSVETO_;
  static const epicsInt32 mbboTriggerLVDSBOTH_;
  static const epicsInt32 ADAcquireFalse_;
  static const epicsInt32 ADAcquireTrue_;
  
  //Put private dynamic here
  epicsUInt32 acquiring_; 
  epicsUInt32 running_; 
  int xsp3_handle_; 

  xsp3Api* xsp3;

  //Constructor parameters.
  const epicsUInt32 debug_; //debug parameter for API
  const epicsInt32 numChannels_; //The number of channels
  const epicsUInt32 simTest_; //Run in sim mode
  const std::string baseIP_; //Constructor param - IP address of host system

  epicsEventId statusEvent_;
  epicsEventId startEvent_;
  epicsEventId stopEvent_;

  //Values used for pasynUser->reason, and indexes into the parameter library.
  int xsp3FirstParam;
  #define XSP3_FIRST_DRIVER_COMMAND xsp3FirstParam
  int xsp3ResetParam;
  int xsp3EraseParam;
  int xsp3NumChannelsParam;
  int xsp3MaxNumChannelsParam;
  int xsp3MaxSpectraParam;
  int xsp3MaxFramesParam;
  int xsp3FrameCountParam;
  int xsp3TriggerModeParam;
  int xsp3FixedTimeParam;
  int xsp3NumFramesConfigParam;
  int xsp3NumFramesDriverParam;
  int xsp3NumCardsParam;
  int xsp3ConfigPathParam;
  int xsp3ConfigSavePathParam;
  int xsp3ConnectParam;
  int xsp3ConnectedParam;
  int xsp3DisconnectParam;
  int xsp3SaveSettingsParam;
  int xsp3RestoreSettingsParam;
  int xsp3RunFlagsParam;             
  int xsp3TriggerParam;             
  int xsp3InvertF0Param;             
  int xsp3InvertVetoParam;             
  int xsp3DebounceParam;             
  int xsp3ChanMcaRoi1LlmParam;  
  int xsp3ChanMcaRoi2LlmParam;   
  int xsp3ChanMcaRoi3LlmParam;   
  int xsp3ChanMcaRoi4LlmParam;  
  int xsp3ChanMcaRoi5LlmParam;  
  int xsp3ChanMcaRoi6LlmParam;  
  int xsp3ChanMcaRoi7LlmParam;
  int xsp3ChanMcaRoi8LlmParam;
  int xsp3ChanMcaRoi9LlmParam;
  int xsp3ChanMcaRoi10LlmParam;
  int xsp3ChanMcaRoi11LlmParam;
  int xsp3ChanMcaRoi12LlmParam;
  int xsp3ChanMcaRoi13LlmParam;
  int xsp3ChanMcaRoi14LlmParam;
  int xsp3ChanMcaRoi15LlmParam;
  int xsp3ChanMcaRoi16LlmParam;
  int xsp3ChanMcaRoi1HlmParam;   
  int xsp3ChanMcaRoi2HlmParam;   
  int xsp3ChanMcaRoi3HlmParam;   
  int xsp3ChanMcaRoi4HlmParam;   
  int xsp3ChanMcaRoi5HlmParam;   
  int xsp3ChanMcaRoi6HlmParam;   
  int xsp3ChanMcaRoi7HlmParam;   
  int xsp3ChanMcaRoi8HlmParam;   
  int xsp3ChanMcaRoi9HlmParam;   
  int xsp3ChanMcaRoi10HlmParam;   
  int xsp3ChanMcaRoi11HlmParam;   
  int xsp3ChanMcaRoi12HlmParam;   
  int xsp3ChanMcaRoi13HlmParam;   
  int xsp3ChanMcaRoi14HlmParam;   
  int xsp3ChanMcaRoi15HlmParam;   
  int xsp3ChanMcaRoi16HlmParam;   
  int xsp3ChanMcaRoi1Param;      
  int xsp3ChanMcaRoi2Param;      
  int xsp3ChanMcaRoi3Param;      
  int xsp3ChanMcaRoi4Param;      
  int xsp3ChanMcaRoi5Param;      
  int xsp3ChanMcaRoi6Param;      
  int xsp3ChanMcaRoi7Param;      
  int xsp3ChanMcaRoi8Param;      
  int xsp3ChanMcaRoi9Param;      
  int xsp3ChanMcaRoi10Param;      
  int xsp3ChanMcaRoi11Param;      
  int xsp3ChanMcaRoi12Param;      
  int xsp3ChanMcaRoi13Param;      
  int xsp3ChanMcaRoi14Param;      
  int xsp3ChanMcaRoi15Param;      
  int xsp3ChanMcaRoi16Param;      
  int xsp3ChanSca0Param;
  int xsp3ChanSca1Param;             
  int xsp3ChanSca2Param;
  int xsp3ChanSca3Param;
  int xsp3ChanSca4Param;
  int xsp3ChanSca5Param;
  int xsp3ChanSca6Param;
  int xsp3ChanSca7Param;             
  int xsp3ChanSca4ThresholdParam;          
  int xsp3ChanSca5HlmParam;          
  int xsp3ChanSca6HlmParam;          
  int xsp3ChanSca5LlmParam;          
  int xsp3ChanSca6LlmParam;  
  int xsp3ChanDtcFlagsParam;  
  int xsp3ChanDtcAegParam;  
  int xsp3ChanDtcAeoParam;  
  int xsp3ChanDtcIwgParam;  
  int xsp3ChanDtcIwoParam;  
  int xsp3RoiEnableParam;
  int xsp3DtcEnableParam;
  int xsp3EventWidthParam;
  int xsp3ChanDTPercentParam;
  int xsp3ChanDTFactorParam;
  int xsp3PulsePerTriggerParam;
  int xsp3ITFGStartParam;
  int xsp3ITFGStopParam;
  int xsp3LastParam;
  #define XSP3_LAST_DRIVER_COMMAND xsp3LastParam

};

#define NUM_DRIVER_PARAMS (&XSP3_LAST_DRIVER_COMMAND - &XSP3_FIRST_DRIVER_COMMAND + 1)

//static const char *driverName = "Xspress3";


#endif //ISC110BL_H
