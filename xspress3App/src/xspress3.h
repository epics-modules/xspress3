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

#include "asynPortDriver.h"

/* These are the drvInfo strings that are used to identify the parameters.
 * They are used by asyn clients, including standard asyn device support */
#define xsp3ResetParamString              "RESET"
#define xsp3NumChannelsParamString        "NUM_CHANNELS"
#define xsp3Chan1ArrayParamString         "CHAN1_ARRAY"

extern "C" {
int xspress3Config(const char *portName, int numChannels);
}


class Xspress3 : public asynPortDriver {

 public:
  Xspress3(const char *portName, int numChannels);
  virtual ~Xspress3();

  /* These are the methods that we override from asynPortDriver */
  virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
  virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);

  void dataTask(void);
  void statusTask(void);

 private:

  //Put private functions here
  void log(epicsUInt32 mask, const char *msg, const char *function);
  

  //Put private static data members here
  static const epicsUInt32 logFlow_;
  static const epicsUInt32 logError_;

  //Put private dynamic here
  epicsUInt32 acquiring_; //Data acquisition in progress
  epicsUInt32 running_; //driver is in good state and we can read data from device
  epicsUInt32 debug_;

  const epicsInt32 numChannels_; //The number of channels (this is a constructor param).

  epicsEventId statusEvent_;
  epicsEventId dataEvent_;
  float pollingPeriod_;
  float fastPollingPeriod_;

  //Values used for pasynUser->reason, and indexes into the parameter library.
  int xsp3ResetParam;
  #define FIRST_DRIVER_COMMAND xsp3ResetParam
  int xsp3NumChannelsParam;
  int xsp3Chan1ArrayParam;
  #define LAST_DRIVER_COMMAND xsp3Chan1ArrayParam

};

#define NUM_DRIVER_PARAMS (&LAST_DRIVER_COMMAND - &FIRST_DRIVER_COMMAND + 1)

static const char *driverName = "Xspress3";


#endif //ISC110BL_H
