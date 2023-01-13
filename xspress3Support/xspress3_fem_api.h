/*
 * Xspress3FemApi.h
 *
 *  Created on: 5 Nov 2013
 *      Author: grm84
 */

/* This file defines the API that is used to communicate with the xpsress3 FEM hardware */

#ifndef XSPRESS3FEMAPI_H_
#define XSPRESS3FEMAPI_H_

#include <stdlib.h>

typedef struct _FemHandle {
	int m_skt;
	int m_valid;
	enum ProtocolGeneration {ProtocolFEM1, ProtocolFEM2} protocol_generation;
	int max_tx_bytes;
	int max_rx_bytes;
	u_int32_t base_address;
	u_int32_t seq;
	pthread_mutex_t mutex;
	int max_posted_writes;
	int num_posted_writes;
} FemHandle;

#ifdef __cplusplus
extern "C" {
#endif

/* The functions provided by the library.
 */
void* xspress3FemInitialise(const char* hostIPAdress, int fem1_port, int fem2_port);
void  xspress3FemClose(void* femHandle);
int xspress3FemSetInt(void* femHandle, int chan, int region, int offset, u_int32_t value);
int xspress3FemGetInt(void* femHandle, int chan, int region, int offset, u_int32_t *value);
int xspress3FemSetIntArray(void* femHandle, int chan, int region, int offset, size_t size, u_int32_t* value);
int xspress3FemGetIntArray(void* femHandle, int chan, int region, int offset, size_t size, u_int32_t* value);
int xspress3FemDRAMWrite(void* femHandle, u_int32_t address, size_t size, u_int32_t* value);
int xspress3FemPersonalityWrite(void* femHandle, u_int32_t sub_command, u_int32_t function_mask, size_t size, u_int32_t* value, int num_details, u_int32_t *details);
int xspress3FemConfigUDP(void* femHandle, char* fpgaMACaddress, char* fpgaIPaddress, int fpgaPort, char* hostIPaddress, int hostPort);
int xspress3FemSetHostPort(void* femHandle, int hostPort);
int xspress3FemSetPacketSize(void* femHandle, int sizeInBytes);
int xspress3FemResetFrameCounter(void* femHandle);
int xspress3FemGetDMAStatusBlock(void* femHandle, size_t size, u_int32_t *value);
int xspress3FemRDMARead(void* femHandle, u_int32_t address, size_t size, u_int32_t *value);
int xspress3FemRDMAWrite(void* femHandle, u_int32_t address, size_t size, u_int32_t *value);
int xspress3FemSPIRead(void* femHandle, u_int32_t address, size_t size, u_int32_t *value);
int xspress3FemSPIWrite(void* femHandle, u_int32_t address, size_t size, u_int32_t *value);
int xspress3FemChanSPIWrite(void* femHandle, u_int32_t address, size_t num_writes, u_int16_t *value);
int xspress3FemChanSPIRead(void* femHandle, u_int32_t address, size_t num_reads, u_int16_t *value);


int xspress3FemReadRawReg(void* femHandle, u_int32_t address, size_t size, u_int32_t* value);
int xspress3FemI2CWrite(void* femHandle, u_int32_t fem_i2c_bus, u_int32_t address, size_t size, u_int8_t *value);
int xspress3FemI2CRead(void* femHandle, u_int32_t fem_i2c_bus, u_int32_t address, size_t size, u_int8_t *value);
int xspress3FemWriteFanControl(void* femHandle, int offset, size_t size, u_int32_t* value);
int xspress3FemReadFanControl(void* femHandle, u_int32_t offset, size_t size, u_int32_t* value);
int xspress3FemConfigWrite(void* femHandle, u_int32_t address, size_t size, u_int8_t *value);
int xspress3FemConfigRead(void* femHandle, u_int32_t address, size_t size, u_int8_t *value);

int32_t xspress3FemSetFarmLUT(void* femHandle, int index, unsigned char* hostMAC, char* hostIPaddress, int hostPort);
int xspress3FemRMW(void* femHandle, int chan, int region, int offset, u_int32_t and_mask, u_int32_t or_mask, u_int32_t *ret_value);
int xspress3FemRDMARMW(void* femHandle, int address, u_int32_t and_mask, u_int32_t or_mask, u_int32_t *ret_value);

int xspress3FemWriteDMABuff(void* femHandle, int dma_stream, u_int32_t address, size_t num_writes, u_int32_t* value); 
int xspress3FemReadDMABuff(void* femHandle, int dma_stream, u_int32_t address, size_t num_reads, u_int32_t* value); 
int xspress3FemZeroDMABuff(void* femHandle, int dma_stream, u_int32_t address, size_t num_writes); 
int xspress3FemReadDMABuffRoI(void* femHandle, int dma_stream, u_int32_t address, u_int32_t row_length_ops, u_int32_t num_rows, u_int32_t src_stride_ops, u_int32_t dst_stride_ops, u_int32_t* value);


int xspress3FemSetIntMGTDRP(void* femHandle, int link, int offset, u_int32_t value);
int xspress3FemSetIntArrayMGTDRP(void* femHandle, int link, int offset, size_t num_writes, u_int32_t* value); 
int xspress3FemGetIntMGTDRP(void* femHandle, int link, int offset, u_int32_t *value);
int xspress3FemGetIntArrayMGTDRP(void* femHandle, int link, int offset, size_t num_reads, u_int32_t* value);

int xspress3FemI2CWriteRegAddr(void* femHandle, u_int32_t fem_i2c_bus, u_int32_t address, u_int32_t reg_addr, size_t size, u_int8_t *value);
int xspress3FemI2CReadRegAddr(void* femHandle, u_int32_t fem_i2c_bus, u_int32_t address, u_int32_t reg_addr, size_t size, u_int8_t *value);
int xspress3FemXADCRead(void* femHandle, u_int32_t address, size_t size, u_int32_t *value);

int xspress3FemWriteDMAorClk(void* femHandle, int offset, size_t num_writes, u_int32_t* value);
int xspress3FemReadDMAorClk(void* femHandle, int offset, size_t num_reads, u_int32_t* value);

int xspress3FemMPSPIWrite(void* femHandle, int chip, int reg, u_int32_t *value);
int xspress3FemMPSPIRead(void* femHandle, int chip, int reg, u_int32_t *value);
int xspress3FemPersonalityRead(void* femHandle, u_int32_t sub_command, u_int32_t stream_mask, u_int32_t width_bytes, u_int32_t first, u_int32_t num_ops, void* value); 
int xspress3FemWriteDriverStatics(void* femHandle, int offset, size_t num_writes, u_int32_t* value);
int xspress3FemReadDriverStatics(void* femHandle, int offset, size_t num_reads, u_int32_t* value); 

#ifdef __cplusplus
}  /* Closing brace for extern "C" */
#endif

#endif /* XSPRESS3FEMAPI_H_ */
