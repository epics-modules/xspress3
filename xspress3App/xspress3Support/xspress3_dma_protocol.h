/*
 * xspress3_dma_protocol.h
 *
 *  Created on: 19 Mar 2012
 *      Author: wih73
 */

#ifndef XSPRESS3_DMA_PROTOCOL_H_
#define XSPRESS3_DMA_PROTOCOL_H_

//! [XSP3_DMA_COMMANDS]
#define XSP3_DMA_CMD_RESET					1
#define XSP3_DMA_CMD_CONFIG_MEMORY			2
#define XSP3_DMA_CMD_BUILD_DESC				3
#define XSP3_DMA_CMD_START					4
#define XSP3_DMA_CMD_BUILD_AND_START		5
#define XSP3_DMA_CMD_BUILD_TEST_TEST_PAT	6
#define XSP3_DMA_CMD_PRINT_DATA				7
#define XSP3_DMA_CMD_PRINT_SCOPE			8
#define XSP3_DMA_CMD_RESEND					9
#define XSP3_DMA_CMD_READ_STATUS			10
#define XSP3_DMA_CMD_BUILD_DEBUG_DESC		11
#define XSP3_DMA_CMD_PRINT_DESC				12
#define XSP3_DMA_CMD_CHECK_RX_DESC			13
#define XSP3_DMA_CMD_SET_MSG_PPC1			14
#define XSP3_DMA_CMD_SET_MSG_PPC2			15
//! [XSP3_DMA_COMMANDS]

//! [XSP3_DMA_STREAM_NUMBER]
#define XSP3_DMA_STREAM_BNUM_DIRECT			0
#define XSP3_DMA_STREAM_BNUM_PLAYBACK		1
#define XSP3_DMA_STREAM_BNUM_SCOPE0			2
#define XSP3_DMA_STREAM_BNUM_SCOPE1			3
#define XSP3_DMA_STREAM_BNUM_SCALERS		4
#define XSP3_DMA_STREAM_BNUM_HIST_TO_DRAM	5
#define XSP3_DMA_STREAM_BNUM_DRAM_TO_10G	6
#define XSP3_DMA_STREAM_BNUM_10G_TO_DRAM	7
//! [XSP3_DMA_STREAM_NUMBER]
#define XSP3_DMA_STREAM_NUM					(XSP3_DMA_STREAM_BNUM_10G_TO_DRAM+1)

//! [XSP3_DMA_STREAM_MASK]
#define XSP3_DMA_STREAM_MASK_DIRECT			(1<<XSP3_DMA_STREAM_BNUM_DIRECT)
#define XSP3_DMA_STREAM_MASK_PLAYBACK		(1<<XSP3_DMA_STREAM_BNUM_PLAYBACK)
#define XSP3_DMA_STREAM_MASK_SCOPE0			(1<<XSP3_DMA_STREAM_BNUM_SCOPE0)
#define XSP3_DMA_STREAM_MASK_SCOPE1			(1<<XSP3_DMA_STREAM_BNUM_SCOPE1)
#define XSP3_DMA_STREAM_MASK_SCALERS		(1<<XSP3_DMA_STREAM_BNUM_SCALERS)
#define XSP3_DMA_STREAM_MASK_HIST_TO_DRAM	(1<<XSP3_DMA_STREAM_BNUM_HIST_TO_DRAM)
#define XSP3_DMA_STREAM_MASK_DRAM_TO_10G	(1<<XSP3_DMA_STREAM_BNUM_DRAM_TO_10G)
#define XSP3_DMA_STREAM_MASK_10G_TO_DRAM	(1<<XSP3_DMA_STREAM_BNUM_10G_TO_DRAM)
//! [XSP3_DMA_STREAM_MASK]

#define XSP3_DMA_STATE_DESC_CONF	(1<<0)
#define XSP3_DMA_STATE_BUFFER_CONF	(1<<1)
#define XSP3_DMA_STATE_DESC_BUILT	(1<<2)

//! [XSP3_DMA_DESCRIPTOR]
typedef struct _dma_desc
{
	void *next;
	u_int32_t  addr;
	u_int32_t len;
	u_int32_t app[5];
} DMADesc;
//! [XSP3_DMA_DESCRIPTOR]

//! [XSP3_DMA_STREAM]
typedef struct
{
	int32_t state;
	u_int32_t base_addr;
	int32_t is_tx;
	u_int32_t desc;
	u_int32_t num_desc;
	u_int32_t data_start;
	u_int32_t data_size;
	u_int32_t max_block_bytes;
	int32_t defined_desc;
	u_int32_t transfer_start;
	u_int32_t transfer_size;
	int32_t num_frames;
	enum {AllOneFrame, FramePerDesc, FrameByBytes } frame_rule;
} DMAStream;
//! [XSP3_DMA_STREAM]

//! [XSP3_DMA_MSG_BUILD_DESC]
typedef struct
{
	int src_stream;
	u_int32_t addr;
	u_int32_t size_bytes;
	u_int32_t block_size;
} XSP3_DMA_MsgBuildDesc;
//! [XSP3_DMA_MSG_BUILD_DESC]

//! [XSP3_DMA_MSG_BUILD_DEBUG_DESC]
typedef struct
{
	int src_stream;
	u_int32_t addr;
	u_int32_t size_bytes;
	u_int32_t flags;
	u_int32_t packet_size;
	u_int32_t frame_size;
} XSP3_DMA_MsgBuildDebugDesc;
//! [XSP3_DMA_MSG_BUILD_DEBUG_DESC]

#define XSP3_DMA_DEBUG_DESC_SMALL			1
#define XSP3_DMA_DEBUG_DESC_NEAR_PACKET		2
#define XSP3_DMA_DEBUG_DESC_INC_FRAME		4

#define XSP3_DMA_DEBUG_DESC_ALL_SMALL		0x10000

//! [XSP3_DMA_MSG_TEST_PAT]
typedef struct
{
	u_int32_t address;
	u_int32_t num_bytes;
	u_int32_t start_val;
	u_int32_t options;
} XSP3_DMA_MsgTestPat;
//! [XSP3_DMA_MSG_TEST_PAT]

//! [XSP3_DMA_MSG_PRINT]
typedef struct
{
	u_int32_t offset;
	u_int32_t num_bytes;
	u_int32_t options;
} XSP3_DMA_MsgPrint;
//! [XSP3_DMA_MSG_PRINT]

//! [XSP3_DMA_MSG_START]
typedef struct
{
	u_int32_t first_desc;
	u_int32_t num_desc;
	u_int32_t options;
} XSP3_DMA_MsgStart;
//! [XSP3_DMA_MSG_START]

//! [XSP3_DMA_MSG_RESEND]
typedef struct
{
	u_int32_t first_desc;
	u_int32_t num_desc;
	u_int32_t options;
} XSP3_DMA_MsgResend;
//! [XSP3_DMA_MSG_RESEND]

//! [XSP3_DMA_MSG_PRINT_DESC]
typedef struct
{
	u_int32_t first_desc;
	u_int32_t num_desc;
	u_int32_t options;
} XSP3_DMA_MsgPrintDesc;
//! [XSP3_DMA_MSG_PRINT_DESC]

//! [XSP3_DMA_MSG_CHECK_DESC]
typedef struct
{
	u_int32_t first_desc;
	u_int32_t num_desc;
	u_int32_t options;
} XSP3_DMA_MsgCheckDesc;
//! [XSP3_DMA_MSG_CHECK_DESC]

//! [XSP3_DMA_MSG_CHECK]
#define XSP3_DMA_MSG_CHECK_NONE		1			// Do not do any checks, disables default checks
#define XSP3_DMA_MSG_CHECK_LENGTH	2			// Check length from trailer in 10G Rx and Scalers
#define XSP3_DMA_MSG_CHECK_10GRX	4			// Check UDP Header flags and frame
#define XSP3_DMA_MSG_CHECK_FRAME_PER_DESC	8	// Check SOF and EOF present, 1 local link frame per descriptor
#define XSP3_DMA_MSG_CHECK_1_FRAME	0x10		// Check SOF in first desc and EOF in last, 1 large local link frame (scope)
//! [XSP3_DMA_MSG_CHECK]

#define XSP3_DMA_MSG_TP_INC32		0
#define XSP3_DMA_MSG_TP_INC8		1
#define XSP3_DMA_MSG_TP_SLIDE		2
#define XSP3_DMA_MSG_TP_PLAYBACK	3

#define XSP3_DMA_MSG_PRINT_1COL		(1<<0)
#define XSP3_DMA_MSG_PRINT_DEC		(1<<1)

typedef struct
{
	u_int32_t status[XSP3_DMA_STREAM_NUM];			// From RX or TX Register
	DMAStream stream_def[XSP3_DMA_STREAM_NUM];
} XSP3_DMA_StatusBlock;

// Magic for Mail Box is chosen to start F0 as there are no valid addresses at 0xF0000000
#define XSP3_MBOX_MAGIC	0xF0123456
#define XSP3_MBOX_MAX_MSG	10		// Hardware limit is 16 at the moment.
#define XSP3_DMA_MAX_POLL_PPC2	100000000

#define XSP3_DMA_ERROR_OK			0
#define XSP3_DMA_ERROR_UNKNOWN		1
#define XSP3_DMA_ERROR_BAD_COMMAND	2
#define XSP3_DMA_ERROR_BAD_STREAM	3
#define XSP3_DMA_ERROR_BAD_PAY_LOAD	4
#define XSP3_DMA_ERROR_UNCONF_DESC	5
#define XSP3_DMA_ERROR_UNCONF_BUFF	6
#define XSP3_DMA_ERROR_NOT_TX		7
#define XSP3_DMA_ERROR_NOT_RX		8
#define XSP3_DMA_ERROR_DESC_RANGE	9
#define XSP3_DMA_ERROR_DATA_RANGE	10
#define XSP3_DMA_ERROR_MSG_LEVEL	11

#endif /* XSPRESS3_DMA_PROTOCOL_H_ */
