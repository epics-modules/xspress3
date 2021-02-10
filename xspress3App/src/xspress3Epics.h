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
  Xspress3(const char *portName, int numChannels);
  virtual ~Xspress3();

  /* These are the methods that we override from asynPortDriver */
  virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
  virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
  virtual asynStatus writeOctet(asynUser *pasynUser, const char *value,
                                    size_t nChars, size_t *nActual);
  virtual void report(FILE *fp, int details);

  const int checkForStopEvent(double timeout, const char *message);
  const int waitForStartEvent(const char *message);
  void adReportError(const char* message);
  bool createMCAArray(size_t dims[2], NDArray *&pMCA, NDDataType_t dataType);
  bool createSCAArray(void *&pSCA);
  bool readFrame(double* pSCA, double* pMCAData, int frameNumber, int maxSpectra);
  bool readFrame(u_int32_t* pSCA, u_int32_t* pMCAData, int frameNumber, int maxSpectra);
  void writeOutScas(void *&pSCA, int numChannels, NDDataType_t dataType);
  void setStartingParameters();
  const NDDataType_t getDataType();
  void getDims(size_t (&dims)[2]);
  asynStatus checkHistBusy(int checkTimes);
  const int getXsp3Handle() { return this->xsp3_handle_; }
  xsp3Api *getXsp3() { return this->xsp3; }
  void setNDArrayAttributes(NDArray *&pMCA, int frameNumber);
  void setAcqStopParameters(bool aborted);
  int getNumFramesToAcquire();
  void doNDCallbacksIfRequired(NDArray *pMCA);
  int getNumFramesRead();
  void xspAsynPrint(int asynPrintType, const char *format, ...);

 private:

  //Put private functions here
  void checkStatus(int status, const char *function, const char *parentFunction);
  asynStatus connect(void);
  asynStatus disconnect(void);
  asynStatus saveSettings(void);
  asynStatus restoreSettings(void);
  asynStatus checkConnected(void);
  asynStatus setWindow(int channel, int sca, int llm, int hlm);
  asynStatus erase(void);
  asynStatus eraseSCA(void);
  asynStatus checkSaveDir(const char *dirName);
  asynStatus readSCAParams(void);
  asynStatus readDTCParams(void);
  asynStatus readTrigB(void);
  asynStatus setupITFG(void);
  asynStatus mapTriggerMode(int mode, int invert_f0, int invert_veto, int debounce, int *apiMode);
  asynStatus setTriggerMode(int mode, int invert_f0, int invert_veto, int debounce );
  void createInitialParameters();
  bool setInitialParameters(int maxFrames, int maxDriverFrames, int numCards, int maxSpectra);

  //Put private static data members here
  static const epicsInt32 ctrlDisable_;
  static const epicsInt32 ctrlEnable_;
  static const epicsInt32 runFlag_MCA_SPECTRA_;
  static const epicsInt32 runFlag_PLAYB_MCA_SPECTRA_;
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
