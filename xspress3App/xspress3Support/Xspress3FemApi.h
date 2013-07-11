/*
 * Xspress3FemApi.h
 *
 *  Created on: 7 Feb 2012
 *      Author: grm84
 */

/* This file defines the API of the library that is used to communicate with the xpsress3 FEM hardware */

#ifndef XSPRESS3FEMAPI_H_
#define XSPRESS3FEMAPI_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The functions provided by the library.
 */
void* xspress3FemInitialise(const char* hostIPAdress, int port);
void  xspress3FemClose(void* femHandle);
int xspress3FemSetInt(void* femHandle, int chan, int region, int offset, u_int32_t value);
int xspress3FemGetInt(void* femHandle, int chan, int region, int offset, u_int32_t *value);
int xspress3FemSetIntArray(void* femHandle, int chan, int region, int offset, size_t size, u_int32_t* value);
int xspress3FemGetIntArray(void* femHandle, int chan, int region, int offset, size_t size, u_int32_t* value);
int xspress3FemDRAMWrite(void* femHandle, u_int32_t address, size_t size, u_int32_t* value);
int xspress3FemPersonalityWrite(void* femHandle, u_int32_t sub_command, u_int32_t function_mask, size_t size, u_int32_t* value);
int xspress3FemConfigUDP(void* femHandle, char* fpgaMACaddress, char* fpgaIPaddress, int fpgaPort, char* hostIPaddress, int hostPort);
int xspress3FemSetHostPort(void* femHandle, int hostPort);
int xspress3FemSetPacketSize(void* femHandle, int sizeInBytes);
int xspress3FemResetFrameCounter(void* femHandle);
int xspress3FemGetDMAStatusBlock(void* femHandle, u_int32_t *value);
int xspress3FemRDMARead(void* femHandle, u_int32_t address, size_t size, u_int32_t *value);
int xspress3FemRDMAWrite(void* femHandle, u_int32_t address, size_t size, u_int32_t *value);
int xspress3FemSPIRead(void* femHandle, u_int32_t address, size_t size, u_int32_t *value);
int xspress3FemSPIWrite(void* femHandle, u_int32_t address, size_t size, u_int32_t *value);
int xspress3FemReadRawReg(void* femHandle, u_int32_t address, size_t size, u_int32_t* value);
int xspress3FemI2CWrite(void* femHandle, u_int32_t fem_i2c_bus, u_int32_t address, size_t size, u_int8_t *value);
int xspress3FemI2CRead(void* femHandle, u_int32_t fem_i2c_bus, u_int32_t address, size_t size, u_int8_t *value);
int xspress3FemWriteFanControl(void* femHandle, int offset, size_t size, u_int32_t* value);
int xspress3FemReadFanControl(void* femHandle, u_int32_t offset, size_t size, u_int32_t* value);

/* An identifier that indicates 'all chips'.
 */
#define FEM_CHIP_ALL 0

/* The return codes used by various functions.
 */
#define FEM_RTN_OK 0
#define FEM_RTN_UNKNOWNOPID 1
#define FEM_RTN_ILLEGALCHIP 2
#define FEM_RTN_BADSIZE 3

#ifdef __cplusplus
}  /* Closing brace for extern "C" */
#endif

#endif /* XSPRESS3FEMAPI_H_ */
