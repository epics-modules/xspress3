/**
 * Asyn port driver for the Xspress3 system.
 *
 * @author Matthew Pearson
 * @date Sept 2012
 *
 * @version 0.1
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

#include "asynNDArrayDriver.h"

/* These are the drvInfo strings that are used to identify the parameters.
 * They are used by asyn clients, including standard asyn device support */
//System wide settings
#define xsp3ResetParamString              "RESET"
#define xsp3EraseParamString              "ERASE"
#define xsp3StartParamString              "START"
#define xsp3StopParamString               "STOP"
#define xsp3BusyParamString               "BUSY"
#define xsp3StatusParamString             "STATUS"
#define xsp3StatParamString             "STAT"
#define xsp3NumChannelsParamString        "NUM_CHANNELS"
#define xsp3MaxNumChannelsParamString     "MAX_NUM_CHANNELS"
#define xsp3MaxSpectraParamString     "MAX_SPECTRA"
#define xsp3MaxFramesParamString     "MAX_FRAMES"
#define xsp3FrameCountParamString     "FRAME_COUNT"
#define xsp3FrameCountTotalParamString     "FRAME_COUNT_TOTAL"
#define xsp3TriggerModeParamString        "TRIGGER_MODE"
#define xsp3FixedTimeParamString        "FIXED_TIME"
#define xsp3NumFramesParamString          "NUM_FRAMES"
#define xsp3NumFramesConfigParamString          "NUM_FRAMES_CONFIG"
#define xsp3NumCardsParamString           "NUM_CARDS"
#define xsp3ConfigPathParamString           "CONFIG_PATH"
#define xsp3ConfigSavePathParamString           "CONFIG_SAVE_PATH"
#define xsp3ConnectParamString           "CONNECT"
#define xsp3ConnectedParamString           "CONNECTED"
#define xsp3DisconnectParamString           "DISCONNECT"
#define xsp3SaveSettingsParamString           "SAVE_SETTINGS"
#define xsp3RestoreSettingsParamString           "RESTORE_SETTINGS"
#define xsp3RunFlagsParamString           "RUN_FLAGS"
//Settings for a channel
#define xsp3ChanMcaParamString            "CHAN_MCA"
#define xsp3ChanMcaCorrParamString        "CHAN_MCA_CORR"
#define xsp3ChanMcaRoi1LlmParamString     "CHAN_ROI1_LLM"
#define xsp3ChanMcaRoi2LlmParamString     "CHAN_ROI2_LLM"
#define xsp3ChanMcaRoi3LlmParamString     "CHAN_ROI3_LLM"
#define xsp3ChanMcaRoi4LlmParamString     "CHAN_ROI4_LLM"
#define xsp3ChanMcaRoi1HlmParamString     "CHAN_ROI1_HLM"
#define xsp3ChanMcaRoi2HlmParamString     "CHAN_ROI2_HLM"
#define xsp3ChanMcaRoi3HlmParamString     "CHAN_ROI3_HLM"
#define xsp3ChanMcaRoi4HlmParamString     "CHAN_ROI4_HLM"
#define xsp3ChanMcaRoi1ParamString        "CHAN_ROI1"
#define xsp3ChanMcaRoi2ParamString        "CHAN_ROI2"
#define xsp3ChanMcaRoi3ParamString        "CHAN_ROI3"
#define xsp3ChanMcaRoi4ParamString        "CHAN_ROI4"
#define xsp3ChanMcaRoi1ArrayParamString        "CHAN_ROI1_ARRAY"
#define xsp3ChanMcaRoi2ArrayParamString        "CHAN_ROI2_ARRAY"
#define xsp3ChanMcaRoi3ArrayParamString        "CHAN_ROI3_ARRAY"
#define xsp3ChanMcaRoi4ArrayParamString        "CHAN_ROI4_ARRAY"
#define xsp3ChanSca0ParamString           "CHAN_SCA0"
#define xsp3ChanSca1ParamString           "CHAN_SCA1"
#define xsp3ChanSca2ParamString           "CHAN_SCA2"
#define xsp3ChanSca3ParamString           "CHAN_SCA3"
#define xsp3ChanSca4ParamString           "CHAN_SCA4"
#define xsp3ChanSca5ParamString           "CHAN_SCA5"
#define xsp3ChanSca6ParamString           "CHAN_SCA6"
#define xsp3ChanSca7ParamString           "CHAN_SCA7"
#define xsp3ChanSca5CorrParamString       "CHAN_SCA5_CORR"
#define xsp3ChanSca6CorrParamString       "CHAN_SCA6_CORR"
#define xsp3ChanSca0ArrayParamString           "CHAN_SCA0_ARRAY"
#define xsp3ChanSca1ArrayParamString           "CHAN_SCA1_ARRAY"
#define xsp3ChanSca2ArrayParamString           "CHAN_SCA2_ARRAY"
#define xsp3ChanSca3ArrayParamString           "CHAN_SCA3_ARRAY"
#define xsp3ChanSca4ArrayParamString           "CHAN_SCA4_ARRAY"
#define xsp3ChanSca5ArrayParamString           "CHAN_SCA5_ARRAY"
#define xsp3ChanSca6ArrayParamString           "CHAN_SCA6_ARRAY"
#define xsp3ChanSca7ArrayParamString           "CHAN_SCA7_ARRAY"
#define xsp3ChanSca5CorrArrayParamString       "CHAN_SCA5_CORR_ARRAY"
#define xsp3ChanSca6CorrArrayParamString       "CHAN_SCA6_CORR_ARRAY"
#define xsp3ChanSca4ThresholdParamString        "CHAN_SCA4_THRESHOLD"
#define xsp3ChanSca5HlmParamString        "CHAN_SCA5_HLM"
#define xsp3ChanSca6HlmParamString        "CHAN_SCA6_HLM"
#define xsp3ChanSca5LlmParamString        "CHAN_SCA5_LLM"
#define xsp3ChanSca6LlmParamString        "CHAN_SCA6_LLM"
#define xsp3ChanDtcFlagsParamString            "CHAN_DTC_FLAGS"
#define xsp3ChanDtcAegParamString            "CHAN_DTC_AEG"
#define xsp3ChanDtcAeoParamString            "CHAN_DTC_AEO"
#define xsp3ChanDtcIwgParamString            "CHAN_DTC_IWG"
#define xsp3ChanDtcIwoParamString            "CHAN_DTC_IWO"
//Parameters to control single value and array update rates.
#define xsp3CtrlDataParamString              "CTRL_DATA_UPDATE"
#define xsp3CtrlMcaParamString              "CTRL_MCA_UPDATE"
#define xsp3CtrlScaParamString              "CTRL_SCA_UPDATE"
#define xsp3CtrlDataPeriodParamString              "CTRL_DATA_UPDATE_PERIOD"
#define xsp3CtrlMcaPeriodParamString              "CTRL_MCA_UPDATE_PERIOD"
#define xsp3CtrlScaPeriodParamString              "CTRL_SCA_UPDATE_PERIOD"
#define xsp3RoiEnableParamString        "CTRL_MCA_ROI"



extern "C" {
  int xspress3Config(const char *portName, int numChannels, int numCards, const char *baseIP, int maxFrames, int maxSpectra, int maxBuffers, size_t maxMemory, int debug, int simTest);
}


class Xspress3 : public asynNDArrayDriver {

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
  void statusTask(void);

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

  //Put private static data members here
  static const epicsInt32 ctrlDisable_;
  static const epicsInt32 ctrlEnable_;
  static const epicsInt32 runFlag_MCA_SPECTRA_;
  static const epicsInt32 runFlag_PLAYB_MCA_SPECTRA_;
  static const epicsInt32 statIdle_;
  static const epicsInt32 statAcquire_;
  static const epicsInt32 statReadout_;
  static const epicsInt32 statAborted_;
  static const epicsInt32 statError_;
  static const epicsInt32 statDisconnected_;
  static const epicsInt32 maxNumRoi_;
  
  //Put private dynamic here
  epicsUInt32 acquiring_; //Data acquisition in progress
  epicsUInt32 running_; //driver is in good state and we can read data from device
  epicsUInt32 debug_;
  int xsp3_handle_;

  const epicsInt32 numChannels_; //The number of channels (this is a constructor param).
  epicsUInt32 simTest_;
  char baseIP_[24]; //Constructor param - IP address of host system

  epicsEventId statusEvent_;
  epicsEventId startEvent_;
  epicsEventId stopEvent_;
  float pollingPeriod_;
  float fastPollingPeriod_;

  //Values used for pasynUser->reason, and indexes into the parameter library.
  int xsp3ResetParam;
  #define FIRST_DRIVER_COMMAND xsp3ResetParam
  int xsp3EraseParam;
  int xsp3StartParam;
  int xsp3StopParam;
  int xsp3BusyParam;
  int xsp3StatusParam;
  int xsp3StatParam;
  int xsp3NumChannelsParam;
  int xsp3MaxNumChannelsParam;
  int xsp3MaxSpectraParam;
  int xsp3MaxFramesParam;
  int xsp3FrameCountParam;
  int xsp3FrameCountTotalParam;
  int xsp3TriggerModeParam;
  int xsp3FixedTimeParam;
  int xsp3NumFramesParam;
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
