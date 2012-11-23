/**
 * Asyn port driver for the Xspress3 system.
 *
 * @author Matthew Pearson
 * @date Sept 2012
 *
 * @version 0.2
 *
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

/* These are the drvInfo strings that are used to identify the parameters.
 * They are used by asyn clients, including standard asyn device support */
//System wide settings
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
#define xsp3NumCardsParamString           "XSP3_NUM_CARDS"
#define xsp3ConfigPathParamString           "XSP3_CONFIG_PATH"
#define xsp3ConfigSavePathParamString           "XSP3_CONFIG_SAVE_PATH"
#define xsp3ConnectParamString           "XSP3_CONNECT"
#define xsp3ConnectedParamString           "XSP3_CONNECTED"
#define xsp3DisconnectParamString           "XSP3_DISCONNECT"
#define xsp3SaveSettingsParamString           "XSP3_SAVE_SETTINGS"
#define xsp3RestoreSettingsParamString           "XSP3_RESTORE_SETTINGS"
#define xsp3RunFlagsParamString           "XSP3_RUN_FLAGS"
//Settings for a channel
#define xsp3ChanMcaParamString            "XSP3_CHAN_MCA"
#define xsp3ChanMcaCorrParamString        "XSP3_CHAN_MCA_CORR"
#define xsp3ChanMcaRoi1LlmParamString     "XSP3_CHAN_ROI1_LLM"
#define xsp3ChanMcaRoi2LlmParamString     "XSP3_CHAN_ROI2_LLM"
#define xsp3ChanMcaRoi3LlmParamString     "XSP3_CHAN_ROI3_LLM"
#define xsp3ChanMcaRoi4LlmParamString     "XSP3_CHAN_ROI4_LLM"
#define xsp3ChanMcaRoi1HlmParamString     "XSP3_CHAN_ROI1_HLM"
#define xsp3ChanMcaRoi2HlmParamString     "XSP3_CHAN_ROI2_HLM"
#define xsp3ChanMcaRoi3HlmParamString     "XSP3_CHAN_ROI3_HLM"
#define xsp3ChanMcaRoi4HlmParamString     "XSP3_CHAN_ROI4_HLM"
#define xsp3ChanMcaRoi1ParamString        "XSP3_CHAN_ROI1"
#define xsp3ChanMcaRoi2ParamString        "XSP3_CHAN_ROI2"
#define xsp3ChanMcaRoi3ParamString        "XSP3_CHAN_ROI3"
#define xsp3ChanMcaRoi4ParamString        "XSP3_CHAN_ROI4"
#define xsp3ChanMcaRoi1ArrayParamString        "XSP3_CHAN_ROI1_ARRAY"
#define xsp3ChanMcaRoi2ArrayParamString        "XSP3_CHAN_ROI2_ARRAY"
#define xsp3ChanMcaRoi3ArrayParamString        "XSP3_CHAN_ROI3_ARRAY"
#define xsp3ChanMcaRoi4ArrayParamString        "XSP3_CHAN_ROI4_ARRAY"
#define xsp3ChanSca0ParamString           "XSP3_CHAN_SCA0"
#define xsp3ChanSca1ParamString           "XSP3_CHAN_SCA1"
#define xsp3ChanSca2ParamString           "XSP3_CHAN_SCA2"
#define xsp3ChanSca3ParamString           "XSP3_CHAN_SCA3"
#define xsp3ChanSca4ParamString           "XSP3_CHAN_SCA4"
#define xsp3ChanSca5ParamString           "XSP3_CHAN_SCA5"
#define xsp3ChanSca6ParamString           "XSP3_CHAN_SCA6"
#define xsp3ChanSca7ParamString           "XSP3_CHAN_SCA7"
#define xsp3ChanSca5CorrParamString       "XSP3_CHAN_SCA5_CORR"
#define xsp3ChanSca6CorrParamString       "XSP3_CHAN_SCA6_CORR"
#define xsp3ChanSca0ArrayParamString           "XSP3_CHAN_SCA0_ARRAY"
#define xsp3ChanSca1ArrayParamString           "XSP3_CHAN_SCA1_ARRAY"
#define xsp3ChanSca2ArrayParamString           "XSP3_CHAN_SCA2_ARRAY"
#define xsp3ChanSca3ArrayParamString           "XSP3_CHAN_SCA3_ARRAY"
#define xsp3ChanSca4ArrayParamString           "XSP3_CHAN_SCA4_ARRAY"
#define xsp3ChanSca5ArrayParamString           "XSP3_CHAN_SCA5_ARRAY"
#define xsp3ChanSca6ArrayParamString           "XSP3_CHAN_SCA6_ARRAY"
#define xsp3ChanSca7ArrayParamString           "XSP3_CHAN_SCA7_ARRAY"
#define xsp3ChanSca5CorrArrayParamString       "XSP3_CHAN_SCA5_CORR_ARRAY"
#define xsp3ChanSca6CorrArrayParamString       "XSP3_CHAN_SCA6_CORR_ARRAY"
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
//Parameters to control single value and array update rates.
#define xsp3CtrlDataParamString              "XSP3_CTRL_DATA_UPDATE"
#define xsp3CtrlMcaParamString              "XSP3_CTRL_MCA_UPDATE"
#define xsp3CtrlScaParamString              "XSP3_CTRL_SCA_UPDATE"
#define xsp3CtrlDataPeriodParamString              "XSP3_CTRL_DATA_UPDATE_PERIOD"
#define xsp3CtrlMcaPeriodParamString              "XSP3_CTRL_MCA_UPDATE_PERIOD"
#define xsp3CtrlScaPeriodParamString              "XSP3_CTRL_SCA_UPDATE_PERIOD"
#define xsp3RoiEnableParamString        "XSP3_CTRL_MCA_ROI"



extern "C" {
  int xspress3Config(const char *portName, int numChannels, int numCards, const char *baseIP, int maxFrames, int maxSpectra, int maxBuffers, size_t maxMemory, int debug, int simTest);
}


class Xspress3 : public ADDriver {

 public:
  Xspress3(const char *portName, int numChannels, int numCards, const char *baseIP, int maxFrames, int maxSpectra, int maxBuffers, size_t maxMemory, int debug, int simTest);
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
  asynStatus checkHistBusy(int checkTimes);
  asynStatus mapTriggerMode(int mode, int *apiMode);

  //Put private static data members here
  static const epicsInt32 ctrlDisable_;
  static const epicsInt32 ctrlEnable_;
  static const epicsInt32 runFlag_MCA_SPECTRA_;
  static const epicsInt32 runFlag_PLAYB_MCA_SPECTRA_;
  static const epicsInt32 maxNumRoi_;
  
  //Put private dynamic here
  epicsUInt32 acquiring_; 
  epicsUInt32 running_; 
  int xsp3_handle_; 

  //Constructor parameters.
  const epicsUInt32 debug_; //debug parameter for API
  const epicsInt32 numChannels_; //The number of channels
  const epicsUInt32 simTest_; //Run in sim mode
  const std::string baseIP_; //Constructor param - IP address of host system

  epicsEventId statusEvent_;
  epicsEventId startEvent_;
  epicsEventId stopEvent_;

  //Values used for pasynUser->reason, and indexes into the parameter library.
  int xsp3ResetParam;
  #define FIRST_DRIVER_COMMAND xsp3ResetParam
  int xsp3EraseParam;
  int xsp3NumChannelsParam;
  int xsp3MaxNumChannelsParam;
  int xsp3MaxSpectraParam;
  int xsp3MaxFramesParam;
  int xsp3FrameCountParam;
  int xsp3TriggerModeParam;
  int xsp3FixedTimeParam;
  int xsp3NumFramesConfigParam;
  int xsp3NumCardsParam;
  int xsp3ConfigPathParam;
  int xsp3ConfigSavePathParam;
  int xsp3ConnectParam;
  int xsp3ConnectedParam;
  int xsp3DisconnectParam;
  int xsp3SaveSettingsParam;
  int xsp3RestoreSettingsParam;
  int xsp3RunFlagsParam;
  int xsp3ChanMcaParam;             
  int xsp3ChanMcaCorrParam; 
  int xsp3ChanMcaRoi1LlmParam;  
  int xsp3ChanMcaRoi2LlmParam;   
  int xsp3ChanMcaRoi3LlmParam;   
  int xsp3ChanMcaRoi4LlmParam;   
  int xsp3ChanMcaRoi1HlmParam;   
  int xsp3ChanMcaRoi2HlmParam;   
  int xsp3ChanMcaRoi3HlmParam;   
  int xsp3ChanMcaRoi4HlmParam;   
  int xsp3ChanMcaRoi1Param;      
  int xsp3ChanMcaRoi2Param;      
  int xsp3ChanMcaRoi3Param;      
  int xsp3ChanMcaRoi4Param;      
  int xsp3ChanMcaRoi1ArrayParam; 
  int xsp3ChanMcaRoi2ArrayParam; 
  int xsp3ChanMcaRoi3ArrayParam; 
  int xsp3ChanMcaRoi4ArrayParam; 
  int xsp3ChanSca0Param;
  int xsp3ChanSca1Param;             
  int xsp3ChanSca2Param;
  int xsp3ChanSca3Param;
  int xsp3ChanSca4Param;
  int xsp3ChanSca5Param;
  int xsp3ChanSca6Param;
  int xsp3ChanSca7Param;             
  int xsp3ChanSca5CorrParam;         
  int xsp3ChanSca6CorrParam; 
  int xsp3ChanSca0ArrayParam;        
  int xsp3ChanSca1ArrayParam;
  int xsp3ChanSca2ArrayParam;
  int xsp3ChanSca3ArrayParam;
  int xsp3ChanSca4ArrayParam;
  int xsp3ChanSca5ArrayParam;
  int xsp3ChanSca6ArrayParam;
  int xsp3ChanSca7ArrayParam;
  int xsp3ChanSca5CorrArrayParam;
  int xsp3ChanSca6CorrArrayParam;
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
  int xsp3CtrlDataParam;
  int xsp3CtrlMcaParam;
  int xsp3CtrlScaParam;
  int xsp3CtrlDataPeriodParam;
  int xsp3CtrlMcaPeriodParam;
  int xsp3CtrlScaPeriodParam;
  int xsp3RoiEnableParam;
  #define LAST_DRIVER_COMMAND xsp3RoiEnableParam

};

#define NUM_DRIVER_PARAMS (&LAST_DRIVER_COMMAND - &FIRST_DRIVER_COMMAND + 1)

static const char *driverName = "Xspress3";


#endif //ISC110BL_H
