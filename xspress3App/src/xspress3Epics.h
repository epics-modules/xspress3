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
#define xsp3StatusParamString             "STATUS"
#define xsp3NumChannelsParamString        "NUM_CHANNELS"
#define xsp3MaxNumChannelsParamString     "MAX_NUM_CHANNELS"
#define xsp3TriggerModeParamString        "TRIGGER_MODE"
#define xsp3NumFramesParamString          "NUM_FRAMES"
#define xsp3NumCardsParamString           "NUM_CARDS"
#define xsp3ConfigPathParamString           "CONFIG_PATH"
#define xsp3ConnectParamString           "CONNECT"
#define xsp3ConnectedParamString           "CONNECTED"
#define xsp3DisconnectParamString           "DISCONNECT"
//Settings for a channel
#define xsp3ChanMcaParamString            "CHAN_MCA"
#define xsp3ChanMcaCorrParamString        "CHAN_MCA_CORR"
#define xsp3ChanSca1ParamString           "CHAN_SCA1"
#define xsp3ChanSca2ParamString           "CHAN_SCA2"
#define xsp3ChanSca3ParamString           "CHAN_SCA3"
#define xsp3ChanSca4ParamString           "CHAN_SCA4"
#define xsp3ChanSca1CorrParamString       "CHAN_SCA1_CORR"
#define xsp3ChanSca2CorrParamString       "CHAN_SCA2_CORR"
#define xsp3ChanSca3CorrParamString       "CHAN_SCA3_CORR"
#define xsp3ChanSca4CorrParamString       "CHAN_SCA4_CORR"
#define xsp3ChanSca1ArrayParamString           "CHAN_SCA1_ARRAY"
#define xsp3ChanSca2ArrayParamString           "CHAN_SCA2_ARRAY"
#define xsp3ChanSca3ArrayParamString           "CHAN_SCA3_ARRAY"
#define xsp3ChanSca4ArrayParamString           "CHAN_SCA4_ARRAY"
#define xsp3ChanSca1CorrArrayParamString       "CHAN_SCA1_CORR_ARRAY"
#define xsp3ChanSca2CorrArrayParamString       "CHAN_SCA2_CORR_ARRAY"
#define xsp3ChanSca3CorrArrayParamString       "CHAN_SCA3_CORR_ARRAY"
#define xsp3ChanSca4CorrArrayParamString       "CHAN_SCA4_CORR_ARRAY"
#define xsp3ChanSca1HlmParamString        "CHAN_SCA1_HLM"
#define xsp3ChanSca2HlmParamString        "CHAN_SCA2_HLM"
#define xsp3ChanSca3HlmParamString        "CHAN_SCA3_HLM"
#define xsp3ChanSca4HlmParamString        "CHAN_SCA4_HLM"
#define xsp3ChanSca1LlmParamString        "CHAN_SCA1_LLM"
#define xsp3ChanSca2LlmParamString        "CHAN_SCA2_LLM"
#define xsp3ChanSca3LlmParamString        "CHAN_SCA3_LLM"
#define xsp3ChanSca4LlmParamString        "CHAN_SCA4_LLM"
#define xsp3ChanTotalParamString          "CHAN_TOTAL"
#define xsp3ChanTotalCorrParamString      "CHAN_TOTAL_CORR"
#define xsp3ChanTotalArrayParamString     "CHAN_TOTAL_ARRAY"
#define xsp3ChanTotalCorrArrayParamString "CHAN_TOTAL_CORR_ARRAY"
//Parameters to control single value and array update rates.
#define xsp3CtrlDataParamString              "CTRL_DATA_UPDATE"
#define xsp3CtrlMcaParamString              "CTRL_MCA_UPDATE"
#define xsp3CtrlScaParamString              "CTRL_SCA_UPDATE"
#define xsp3CtrlTotalParamString              "CTRL_TOTAL_UPDATE"
#define xsp3CtrlDataPeriodParamString              "CTRL_DATA_UPDATE_PERIOD"
#define xsp3CtrlMcaPeriodParamString              "CTRL_MCA_UPDATE_PERIOD"
#define xsp3CtrlScaPeriodParamString              "CTRL_SCA_UPDATE_PERIOD"
#define xsp3CtrlTotalPeriodParamString              "CTRL_TOTAL_UPDATE_PERIOD"



extern "C" {
  int xspress3Config(const char *portName, int numChannels, const char *baseIP, int maxBuffers, size_t maxMemory, int debug);
}


class Xspress3 : public asynNDArrayDriver {

 public:
  Xspress3(const char *portName, int numChannels, const char *baseIP, int maxBuffers, size_t maxMemory, int debug);
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
  void log(epicsUInt32 mask, const char *msg, const char *function);
  void checkStatus(int status, const char *function, const char *parentFunction);
  asynStatus connect(void);
  asynStatus disconnect(void);

  //Put private static data members here
  static const epicsUInt32 logFlow_;
  static const epicsUInt32 logError_;
  static const epicsInt32 ctrlDisable_;
  static const epicsInt32 ctrlEnable_;

  //Put private dynamic here
  epicsUInt32 acquiring_; //Data acquisition in progress
  epicsUInt32 running_; //driver is in good state and we can read data from device
  epicsUInt32 debug_;
  int xsp3_handle_;

  const epicsInt32 numChannels_; //The number of channels (this is a constructor param).
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
  int xsp3StatusParam;
  int xsp3NumChannelsParam;
  int xsp3MaxNumChannelsParam;
  int xsp3TriggerModeParam;
  int xsp3NumFramesParam;
  int xsp3NumCardsParam;
  int xsp3ConfigPathParam;
  int xsp3ConnectParam;
  int xsp3ConnectedParam;
  int xsp3DisconnectParam;
  int xsp3ChanMcaParam;             
  int xsp3ChanMcaCorrParam;         
  int xsp3ChanSca1Param;             
  int xsp3ChanSca2Param;             
  int xsp3ChanSca3Param;           
  int xsp3ChanSca4Param;             
  int xsp3ChanSca1CorrParam;         
  int xsp3ChanSca2CorrParam;         
  int xsp3ChanSca3CorrParam;         
  int xsp3ChanSca4CorrParam;  
  int xsp3ChanSca1ArrayParam;
  int xsp3ChanSca2ArrayParam;
  int xsp3ChanSca3ArrayParam;
  int xsp3ChanSca4ArrayParam;
  int xsp3ChanSca1CorrArrayParam;
  int xsp3ChanSca2CorrArrayParam;
  int xsp3ChanSca3CorrArrayParam;
  int xsp3ChanSca4CorrArrayParam;
  int xsp3ChanSca1HlmParam;          
  int xsp3ChanSca2HlmParam;          
  int xsp3ChanSca3HlmParam;          
  int xsp3ChanSca4HlmParam;          
  int xsp3ChanSca1LlmParam;          
  int xsp3ChanSca2LlmParam;          
  int xsp3ChanSca3LlmParam;          
  int xsp3ChanSca4LlmParam;          
  int xsp3ChanTotalParam;            
  int xsp3ChanTotalCorrParam;  
  int xsp3ChanTotalArrayParam;            
  int xsp3ChanTotalCorrArrayParam;  
  int xsp3CtrlDataParam;
  int xsp3CtrlMcaParam;
  int xsp3CtrlScaParam;
  int xsp3CtrlTotalParam;
  int xsp3CtrlDataPeriodParam;
  int xsp3CtrlMcaPeriodParam;
  int xsp3CtrlScaPeriodParam;
  int xsp3CtrlTotalPeriodParam;
  #define LAST_DRIVER_COMMAND xsp3CtrlTotalPeriodParam

};

#define NUM_DRIVER_PARAMS (&LAST_DRIVER_COMMAND - &FIRST_DRIVER_COMMAND + 1)

static const char *driverName = "Xspress3";


#endif //ISC110BL_H
