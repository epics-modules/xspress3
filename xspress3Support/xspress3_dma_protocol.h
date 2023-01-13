/*
 * xspress3_dma_protocol.h
 *
 *  Created on: 19 Mar 2012
 *      Author: wih73
 */


	  
#ifndef XSPRESS3_DMA_PROTOCOL_H_
#define XSPRESS3_DMA_PROTOCOL_H_

/**
	@defgroup XSP3_DMA   Macros to control the DMA engines in the FPGA.
	@ingroup XSP3_MACROS
*/

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
#define XSP3_DMA_CMD_GET_DESC_STATUS		16
#define XSP3_DMA_CMD_REUSE					17
#define XSP3_DMA_CMD_CHECK_RX_DESC_CIRCULAR 18
#define XSP3_DMA_CMD_READ_TF_STATUS		 	19
#define XSP3_DMA_CMD_READ_TF_MARKERS		20
#define XSP3_DMA_CMD_READ_SEND_LIST			21

//! [XSP3_DMA_COMMANDS]

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
#define XSP3_DMA_ERROR_OUTSIDE_VA	12
#define XSP3_DMA_INSUFFICIENT_MEM	13

#define XSP3_DMA_ERROR_FIRMWARE_DETECTED 20
#define XSP3_DMA_ERROR_DESC_SOF_EOF 	21
#define XSP3_DMA_ERROR_DESC_LENGTH 		22
#define XSP3_DMA_ERROR_DESC_TIME_FRAME 	23

/* Note these match Xsp3ErrFlag_* where possible */ 
#define XSP3_DMA_ERROR_DESC_OVERRUN		0x0200
#define XSP3_DMA_ERROR_DESC_TF_MASK		0x0400
#define XSP3_DMA_ERROR_DESC_SOFEOF		0x0800
#define XSP3_DMA_ERROR_DESC_COUNT		0x1000
#define XSP3_DMA_ERROR_SOCKET			0x2000

#define XSP3_DMA_ERROR_MOD_OVERRUN		0x010000

#define XSP3_DMA_ERROR_DESC_OVERRUN_SCAL	0x020000
#define XSP3_DMA_ERROR_DESC_TF_MASK_SCAL	0x040000
#define XSP3_DMA_ERROR_DESC_SOFEOF_SCAL		0x080000
#define XSP3_DMA_ERROR_DESC_COUNT_SCAL		0x100000
#define XSP3_DMA_ERROR_SOCKET_SCAL			0x200000
#define XSP3_DMA_ERROR_MOD_OVERRUN_SCAL		0x1000000

#define XSP3_DMA_BD_STS_COMPLETE_MASK	0x80000000 /**< Completed */
#define XSP3_DMA_BD_STS_DEC_ERR_MASK	0x40000000 /**< Decode error */
#define XSP3_DMA_BD_STS_SLV_ERR_MASK	0x20000000 /**< Slave error */
#define XSP3_DMA_BD_STS_INT_ERR_MASK	0x10000000 /**< Internal err */
#define XSP3_DMA_BD_STS_ALL_ERR_MASK	0x70000000 /**< All errors */
#define XSP3_DMA_BD_STS_RXSOF_MASK	0x08000000 /**< First rx pkt */
#define XSP3_DMA_BD_STS_RXEOF_MASK	0x04000000 /**< Last rx pkt */
#define XSP3_DMA_BD_STS_ALL_MASK		0xFC000000 /**< All status bits */

/*! @defgroup XSP3_DMA_STREAM_NUMBER  Number used to identify DMA streams controlled by PPC1 in the Virtex-5 FPGA.
    @ingroup XSP3_DMA
  @{
*/
#define XSP3_DMA_STREAM_BNUM_DIRECT			0		//!< No DMA stream specified, use absolute addresses.
#define XSP3_DMA_STREAM_BNUM_PLAYBACK		1		//!< Use Playback DMA and associated descriptors and memory buffer.
#define XSP3_DMA_STREAM_BNUM_SCOPE0			2		//!< Use Scope0 DMA (for scope streams 0:2) and associated descriptors and memory buffer.
#define XSP3_DMA_STREAM_BNUM_SCOPE1			3		//!< Use Scope1 DMA (for scope streams 3:5) and associated descriptors and memory buffer.
#define XSP3_DMA_STREAM_BNUM_SCALERS		4		//!< Use scalers DMA and associated descriptors and memory buffer.
#define XSP3_DMA_STREAM_BNUM_HIST_TO_DRAM	5		//!< Use DMA for saving histogram addresses to DRAM and associated descriptors and memory buffer. Never used. Provided for debug only.
#define XSP3_DMA_STREAM_BNUM_DRAM_TO_10G	6		//!< Use DMA Output FEM DRAM over 10 G Ethernet.
#define XSP3_DMA_STREAM_BNUM_10G_TO_DRAM	7		//!< Use DMA filling FEM DRAM from 10 G Ethernet.
// Some reuse in xspress3, unless need access to XSP3M_DMA_STREAM_BNUM_SOFT_HIST
#define XSP3M_DMA_STREAM_BNUM_HIST_FRAMES   5		//!< Completed histogrammed frames in Xspress3 mini. Re use HIST to DRAM number as this is not used in any current builds
#define XSP3M_DMA_STREAM_BNUM_HIST_LIST     8		//!< List of events for software histogramming Xspress3 mini. 
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST0    9		//!< Special BNUM  used to identify the soft histogram buffer (rather than the event list) associated with XSP3M_DMA_STREAM_BNUM_HIST_LIST
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST1    (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+1)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST2    (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+2)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST3    (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+3)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST4    (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+4)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST5    (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+5)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST6    (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+6)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST7    (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+7)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST8    (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+8)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST9    (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+9)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST10   (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+10)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST11   (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+11)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST12   (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+12)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST13   (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+13)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST14   (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+14)
#define XSP3M_DMA_STREAM_BNUM_SOFT_HIST15   (XSP3M_DMA_STREAM_BNUM_SOFT_HIST0+15)


//! @}
#define XSP3_DMA_STREAM_NUM					(XSP3_DMA_STREAM_BNUM_10G_TO_DRAM+1)
#define XSP4_DMA_STREAM_NUM					(XSP3M_DMA_STREAM_BNUM_SOFT_HIST15+1)

/*! @defgroup XSP3_DMA_STREAM_MASK  Binary Mask used to identify multiple DMA streams controlled by PPC1 in the Virtex-5 FPGA.
    @ingroup XSP3_DMA
    @{
*/
#define XSP3_DMA_STREAM_MASK_DIRECT			(1<<XSP3_DMA_STREAM_BNUM_DIRECT)		//!< No DMA stream specified, use absolute addresses.
#define XSP3_DMA_STREAM_MASK_PLAYBACK		(1<<XSP3_DMA_STREAM_BNUM_PLAYBACK)		//!< Use Playback DMA and associated descriptors and memory buffer.
#define XSP3_DMA_STREAM_MASK_SCOPE0			(1<<XSP3_DMA_STREAM_BNUM_SCOPE0)		//!< Use Scope0 DMA (for scope streams 0:2) and associated descriptors and memory buffer.
#define XSP3_DMA_STREAM_MASK_SCOPE1			(1<<XSP3_DMA_STREAM_BNUM_SCOPE1)		//!< Use Scope1 DMA (for scope streams 3:5) and associated descriptors and memory buffer.
#define XSP3_DMA_STREAM_MASK_SCALERS		(1<<XSP3_DMA_STREAM_BNUM_SCALERS)		//!< Use scalers DMA and associated descriptors and memory buffer.
#define XSP3_DMA_STREAM_MASK_HIST_TO_DRAM	(1<<XSP3_DMA_STREAM_BNUM_HIST_TO_DRAM)  //!< Use DMA for saving histogram addresses to DRAM and associated descriptors and memory buffer. Never used. Provided for debug only.
#define XSP3_DMA_STREAM_MASK_DRAM_TO_10G	(1<<XSP3_DMA_STREAM_BNUM_DRAM_TO_10G)   //!< Use DMA Output FEM DRAM over 10 G Ethernet.
#define XSP3_DMA_STREAM_MASK_10G_TO_DRAM	(1<<XSP3_DMA_STREAM_BNUM_10G_TO_DRAM)   //!< Use DMA filling FEM DRAM from 10 G Ethernet.
#define XSP3M_DMA_STREAM_MASK_HIST_FRAMES   (1<<XSP3M_DMA_STREAM_BNUM_HIST_FRAMES)	//!< Completed histogrammed frames in Xspress3 mini. Re used SCOPE1 number in Xspress3 mini to allow backwards compatibility of status block size 
#define XSP3M_DMA_STREAM_MASK_HIST_LIST     (1<<XSP3M_DMA_STREAM_BNUM_HIST_LIST)	//!< List of events for software histogramming Xspress3 mini. 
//! @}

#define XSP3_DMA_STATE_DESC_CONF	(1<<0)
#define XSP3_DMA_STATE_BUFFER_CONF	(1<<1)
#define XSP3_DMA_STATE_DESC_BUILT	(1<<2)
#define XSP3_DMA_STATE_DESC_DEBUG	(1<<3)

#define XSP3_DMA_DEBUG_DESC_SMALL			1
#define XSP3_DMA_DEBUG_DESC_NEAR_PACKET		2
#define XSP3_DMA_DEBUG_DESC_INC_FRAME		4

#define XSP3_DMA_DEBUG_DESC_ALL_SMALL		0x10000
#define XSP3_DMA_DEBUG_DESC_SHORT_BURSTS	0x20000

#define XSP3_DMA_START_CIRCULAR				1		// Set Tail descriptor so DMA will try to cycle the buffers.

/*! @defgroup XSP3_DMA_LAYOUT MACROS to interpret the layout option when configuring memory, currently on XSPRESS3-mini only.
    @ingroup XSP3_DMA
    @{
*/
//! [XSP3_DMA_LAYOUT_CODE]
#define XSP3_CONF_MEM_OVERALL(x)			((x)&0xFF)
#define XSP3_CONF_MEM_GET_OVERALL(x)		((x)&0xFF)

#define XSP3_CONF_MEM_PLAYBACK_SCOPE(x)		(((x)&0x3F)<<8)
#define XSP3_CONF_MEM_GET_PLAYBACK_SCOPE(x)	(((x)>>8)&0x3F)

#define XSP3_CONF_MEM_HIST_OPTIONS(x)		(((x)&0x3F)<<14)
#define XSP3_CONF_MEM_GET_HIST_OPTIONS(x)	(((x)>>14)&0x3F)
#define XSP3_CONF_MEM_HIST_FRAME_SCALARS	0
#define XSP3_CONF_MEM_HIST_ALL_SCALARS		1
#define XSP3_CONF_MEM_HIST_10FRAME_DIAG		4
#define XSP3_CONF_MEM_HIST_ALL_DIAG			5
#define XSP3_CONF_MEM_HIST_10FRAME_LIST		6		/* For test purposes only */
#define XSP3_CONF_MEM_HIST_ALL_LIST			7


#define XSP3_CONF_MEM_NBITS_ENG(x)			(((x)&0xF)<<20)
#define XSP3_CONF_MEM_GET_NBITS_ENG(x)		(((x)>>20)&0xF)

#define XSP3_CONF_MEM_NUM_CHAN(x)			(((x)&0x3F)<<24)
#define XSP3_CONF_MEM_GET_NUM_CHAN(x)		(((x)>>24)&0x3F)
//! [XSP3_DMA_LAYOUT_CODE]

#define XSP3M_HIST_LIST_BURST_EVENTS		4096

/** @} */

//! [XSP3_DMA_DESCRIPTOR]
typedef struct _dma_desc
{
	void *next;
	u_int32_t  addr;
	u_int32_t len;
	u_int32_t app[5];
} DMADesc;
//! [XSP3_DMA_DESCRIPTOR]

//! [XSP4_DMA_DESCRIPTOR]
/* This is padded to 64 byte boundaries and the padding space is used to store the virtual addresses of the descriptor and optionally data for ARM/Linux code */
typedef struct _axidma_desc
{
	u_int32_t phys_next;
	u_int32_t reserved0;
	u_int32_t phys_addr;
	u_int32_t reserved1[3];
	u_int32_t control;
	u_int32_t status;
	u_int32_t app[5];
	void *virt_next;
	u_int32_t *virt_addr;
	u_int32_t *buff_virt_addr;
} AXIDMADesc;
//! [XSP4_DMA_DESCRIPTOR]

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


//! [XSP3_DMA_MSG_REUSE]
typedef struct
{
	u_int32_t first_desc;
	u_int32_t num_desc;
	u_int32_t options;
} XSP3_DMA_MsgReuse;
//! [XSP3_DMA_MSG_REUSE]

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
#define XSP3_DMA_MSG_CHECK_TIMEFRAME 0x20			// Check time frame is in the app0 field for Xspress3 mini hist and scalars

//! [XSP3_DMA_MSG_CHECK]

//! [XSP3_DMA_MSG_GET_DESC_STATUS]
typedef struct
{
	u_int32_t first_desc;
} XSP3_DMA_MsgGetDescStatus;
//! [XSP3_DMA_MSG_GET_DESC_STATUS]


#define XSP3_DMA_MSG_TP_INC32		0
#define XSP3_DMA_MSG_TP_INC8		1
#define XSP3_DMA_MSG_TP_SLIDE		2
#define XSP3_DMA_MSG_TP_PLAYBACK	3
#define XSP3_DMA_MSG_TP_DEADBEEF	4

#define XSP3_DMA_MSG_PRINT_1COL		(1<<0)
#define XSP3_DMA_MSG_PRINT_DEC		(1<<1)

typedef struct
{
	u_int32_t status[XSP3_DMA_STREAM_NUM];			// From RX or TX Register
	DMAStream stream_def[XSP3_DMA_STREAM_NUM];
} XSP3_DMA_StatusBlock;

// This lengthened structure allows space for XSPRESS3 mini hist frames DMA and Diganostic Hist List DMA 
typedef struct
{
	u_int32_t status[XSP3M_DMA_STREAM_BNUM_HIST_LIST+1];			// From RX or TX Register
	DMAStream stream_def[XSP4_DMA_STREAM_NUM];
	int mem_layout;
	int padding[4];		// For future expansion
} XSP4_DMA_StatusBlock;


// Magic for Mail Box is chosen to start F0 as there are no valid addresses at 0xF0000000

typedef struct {
	u_int64_t num_good;
	u_int64_t num_completed;
	u_int64_t last_tf;
	u_int32_t error_code;
	u_int32_t desc_num;
	u_int32_t dma_status;
	u_int32_t desc_status;
	u_int64_t desc_tf;
} XSP3_DMA_CheckCircular;

#endif /* XSPRESS3_DMA_PROTOCOL_H_ */
