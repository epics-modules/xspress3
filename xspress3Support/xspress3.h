/*
 * xspress3.h
 *
 *  Created on: 19 Mar 2012
 *      Author: wih73
 */
#ifndef XSPRESS3_H_
#define XSPRESS3_H_
 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sched.h>
#include "xspress3_dma_protocol.h"
#include "xspress3_fem_api.h"
#include "xspress3_data_mod.h"

/**
	@defgroup XSP3_MACROS   Macros for XSPRESS3
*/



#define XSP3_CONF_SOFTWARE_ROI_LUT  1	//< Enable Region of interest in software event list processing code
#define XSP3_CONF_ACCUMULATE_RESET_TICKS  0	//< Enable accumulation of reset ticks from each reset width, rather than using reset ticks at end.
#define XSP3_CONF_ALL_GOOD_FROM_MCA  0	//< Enable All Good scaler by summing MCA, but note then pileup reject will remove events from AllGood
#define XSP3_SGX_SOFTWARE			0   //!< Enable SGX processing in software (currently firmware option)

#define FEM_SINGLE 0
#define FEM_COMPOSITE 1

#define XSP3_BINS_PER_MCA 4096

#define XSP3_SW_NUM_SCALERS 9

#define XSP_SW_SCALER_LIVE_TICKS	0			//!< Total exposure time, may show lower than programmed time if data packets are dropped. Can be used to scale for dropped packets 
#define XSP_SW_SCALER_RESET_TICKS 	1			//!< Time in Reset or reset crostalk glitch padding.
#define XSP_SW_SCALER_NUM_RESETS 	2			//!< Number of Resets.
#define XSP_SW_SCALER_ALL_EVENT   	3			//!< Number of all event triggers.
#define XSP_SW_SCALER_ALL_GOOD   	4			//!< Number of events with positivie energy > Good threshold.
#define XSP_SW_SCALER_IN_WINDOW0   	5			//!< Number of events in window 0
#define XSP_SW_SCALER_IN_WINDOW1   	6			//!< Number of events in window 1
#define XSP_SW_SCALER_PILEUP   		7			//!< Number of events detected as pileup.
#define XSP_SW_SCALER_TOTAL_TICKS	8 			//!< Total time in 80MHz ticks of exposure, even if some packets are dropped.

#define XSP3_SOFT_SCALER_NUM_WINDOWS  2

#if XSP3_CONF_ALL_GOOD_FROM_MCA
// This code is not supported at the moment and would nedd fixing for use
#	define XSP3_SOFT_SCALER_TOTAL_TICKS		5 		//!< Total integration time ticks
#	define XSP3_NUM_SOFT_SCALERS 6
#else
#	define XSP3_SOFT_SCALER_TOTAL_TICKS		8 		//!< Total integration time ticks
#	define XSP3_SOFT_SCALER_WINDOW0			9 		//!< First of the In window scalars, used only when Roi Is in use
#	define XSP3_NUM_SOFT_SCALERS (9+XSP3_SOFT_SCALER_NUM_WINDOWS)
#endif
#define XSP3_SOFT_SCALER_LIVE_TICKS		0 		//!< Total integration time in received packets, may may less than total ticks due to dropped packets.
#define XSP3_SOFT_SCALER_RESET_TICKS	1
#define XSP3_SOFT_SCALER_NUM_RESETS 	2
#define XSP3_SOFT_SCALER_ALL_EVENT   	3
#define XSP3_SOFT_SCALER_PILEUP      	4
#define XSP3_SOFT_SCALER_ALL_GOOD      	5
#define XSP3_SOFT_SCALER_ALL_GOOD_GG   	6		//!< All Good with good grade
#define XSP3_SOFT_SCALER_ALL_EVENT_GG   7		//!< All event with good Grade

 
#define XSP3_HW_USED_SCALERS 7
#define XSP3_HW_DEFINED_SCALERS 8

#define XSP3_ENERGIES 4096
#define XSP3_MAX_CARDS 18			//!< Maximum number of cards in a logical system
#define XSP3_MAX_CARD_INDEX 62		//!< Maximum card index across all systems
#define XSP3_MAX_PATH 20
#define XSP3_MAX_IP_CHARS 16
#define XSP3_MAX_CHANS_PER_CARD_ORG 9	//!< Maximum channels per card using FEM-1 and 9 channel ADC card
#define XSP3_MAX_CHANS_PER_CARD 16		//!< Includes XSPRESS4 in VC709 16 channel demonstration.
#define XSP3_MAX_MSG_LEN 1024
#define XSP3_MAX_CHANS (XSP3_MAX_CARDS*XSP3_MAX_CHANS_PER_CARD)
#define XSP3_MAX_ROI 8
#define XSP3_ADC_RANGE 65536
#define XSP3_MAX_TOP_SAMPLES 1024
#define XSP3_CSHR_MAX_NUM_NEB		6		//!< Maxiumum number of charge sharing neighbours

#define XSP3_CALIB_NBITS 10 						// Allows +/- 512 bins around zero 
#define XSP3_CALIB_NBINS (1<<XSP3_CALIB_NBITS)	// Allows +/- 512 bins around zero 

#define XSP3_TRAILER_LWORDS 2
#define XSP3_10GTX_SOF 0x80000000
#define XSP3_10GTX_EOF 0x40000000
#define XSP3_10GTX_PAD 0x20000000
#define XSP3_10GTX_PACKET_MASK 0x0FFFFFFF
#define XSP3_10GTX_TIMEOUT 30

//! [XSP3_ERROR_CODES]
#define XSP3_OK 				0
#define XSP3_ERROR				-1
#define XSP3_INVALID_PATH		-2
#define XSP3_ILLEGAL_CARD		-3
#define XSP3_ILLEGAL_SUBPATH	-4
#define XSP3_INVALID_DMA_STREAM	-5
#define XSP3_RANGE_CHECK		-6
#define XSP3_INVALID_SCOPE_MOD	-7
#define XSP3_OUT_OF_MEMORY		-8
#define XSP3_ERR_DEV_NOT_FOUND	-9
#define XSP3_CANNOT_OPEN_FILE	-10
#define XSP3_FILE_READ_FAILED	-11
#define XSP3_FILE_WRITE_FAILED	-12
#define XSP3_FILE_RENAME_FAILED	-13
#define XSP3_LOG_FILE_MISSING	-14

#define XSP3_WOULD_BLOCK		-20	

//! [XSP3_ERROR_CODES]

#ifndef XSP3_MAX_MODNAME
#define XSP3_MAX_MODNAME 100
#endif
#define XSP_MAX_MAC_ADDR 50
#define XSP_MAX_IP_ADDR  19
#define XSP3_MAX_PLAYBACK_FILES 10				//!< Maximum number of files to be used in one call to load playback data

//! The 3 off 32 bit feature registers store 3 x 8 x 4 bit fields. Unpacked in struct using chars
#define XSP3_FEATURE_ACK_EOF_NONE 0				//!< No End of Frame Acknowledge packets.
#define XSP3_FEATURE_ACK_EOF_YES  1				//!< Acknowledge packets with frame number only
#define XSP3_FEATURE_ACK_EOF_WITH_TIME  2		//!< Acknowledge packets also tell total exposure time of frame.

typedef enum {XspressGenError= -1, XspressGen3, XspressGen3V7, XspressGen3Mini, XspressGen4} XspressGeneration;

typedef struct _xsp3_feature
{
	char test_data_source, real_data_source, data_mux, inl_corr, reset_detector, reset_corr, glitch_detect, glitch_pad;
	char trigger_b, trigger_c, trigger_extra, calibrator, neighbour_events, servo_base, servo_details, run_ave;
	char lead_tail_corr, output_format, format_details_a, format_details_b, global_reset, timing_source, timing_generator, scope_mode;
	char farm_mode, soft_scalers, ack_eof;
	XspressGeneration generation;
	int num_scope_dma, num_playback_streams;
	int max_real_dma_stream;
	int min_nbits_eng;
	int nbits_frac_be;
} Xspress3_features;

typedef struct _tf_status
{
	int state;		//!< State to detect overruns in circular buffer mode. Should be 0 after call to {@link xsp3_histogram_circ_ack()} on that channel and frame.
	int64_t time_frame;	//!< Extended time frame in circular buffer mode
	int markers;		// Marker inputs, circular buffer and normal mode.
} Xsp3TFStatus;

typedef enum  {Xsp3GDPFixed, Xsp3GDPLinear, Xsp3GDPLinearRate, Xsp3GDPTriangleRate} Xsp3GenDataPeriodType;
typedef enum  {Xsp3GDHFixed, Xsp3GDHLinear, Xsp3GDHKAlphaBeta} Xsp3GenDataHeightType;
typedef struct 
{
	Xsp3GenDataPeriodType period_type;
	Xsp3GenDataHeightType height_type;
	double ave_hgt;
	int min_sep;
	int max_sep;
	int stream_sep;
	int num_t;					//!< Set by {@link xsp3_playback_generate}. Also set by  {@link xsp3_playback_generate_est_counts()} if it is <= 0, otherwise it is used to save time
} Xsp3GenDataType;

typedef enum
{	Xsp3ScopeOpt_DelayStart 		= 1,		//!< Delay starting scope mode until rising edge of CountEnable signal.
	Xsp3ScopeOpt_ForceExtraDelay 	= 2,		//!< Apply Extra pipeline delay to CountEnable signal.
	Xsp3ScopeOpt_ExtraDelayOn0 		= 4,		//!< Apply Extra pipeline delay to card 0 only in multi-card system.
	Xsp3ScopeOpt_RadialStart		= 8,		//!< Radially wored delay start on XSPRESS4 rack version only
	Xsp3ScopeOpt_Synchronised		= 0x100		//!< Cause xsp3_set_scope_options to select synchronisation mode based on sync_mode (xsp3_set_sync_mode())
} Xsp3ScopeOptionFlags;
typedef struct
{	Xsp3ScopeOptionFlags flags;					//!< Scope options flags {@link Xsp3ScopeOptionFlags};
	int nstreams;								//!< Number of scope streams to use where programmable (xspress3V7 onwards)
} Xsp3ScopeOptions;

typedef struct _xspress3_saved_config{
	int ncards;
	int num_tf;
	char baseIPaddress[XSP_MAX_IP_ADDR+2];
	int basePort;
	char baseMACaddress[XSP_MAX_MAC_ADDR+2];
	int nchan;
	int createModule;
	char modname[XSP3_MAX_MODNAME+2];
	int debug;
	int card_index;
	int path;
} Xspress3SavedConfig;

typedef struct _UDPconnection
{
    int socket;                         // File descriptor for socket
    int hostPort;                       // PC Port number
    int femPort;						// FEM port number
    int tos;                            // IP TOS (Type of service) setting
    int recvBuf;                        // Receiving buffer size
    int packetSize;                     // Packet (datagram) size
    int dataSize;                       // Data size for each sending/receiving
    int timeout;						// Time to wait
    int sentPackets;                    // Total UDP packets sent
    int recvPackets;                    // Total UDP packets received
    int sentLoss;                       // Loss packets (send)
    int recvLoss;                       // Loss packets (recv)
    u_int64_t sentBytes;                // Total UDP data sent in bytes
    u_int64_t recvBytes;                // Total UPD data received in bytes
    u_int64_t elapsedTime;              // Elapse time in microsecond
} UDPconnection;

typedef struct _XSPROI
{
		int lhs, rhs, out_bins;
} XSP3Roi;

#define XSP3_ORIGINAL_DTC_BYTES (5*sizeof(double))
typedef struct _ChannelDTC
{
	int flags;
	double processDeadTimeAllEventGradient;
	double processDeadTimeAllEventOffset;
	double processDeadTimeInWindowOffset;
	double processDeadTimeInWindowGradient;
	double processDeadTimeAllEventRateGradient;
	double processDeadTimeAllEventRateOffset;
	double processDeadTimeInWindowRateOffset;
	double processDeadTimeInWindowRateGradient;
} ChannelDTC;

typedef struct _Histogram {
	int path;
	int card;
	int chan_of_card;
	int chan_of_system;
	MOD_IMAGE *module;
	u_int32_t *buffer;
	u_int32_t bufsiz;
	MOD_IMAGE *calib_module;
	u_int32_t *calib_buffer;
	u_int32_t calib_bufsiz;
	UDPconnection udpsock;
	int nbits_eng, nbits_aux1, nbits_aux2;
	int nbins_eng, nbins_aux1, nbins_aux2;
	int aux1_mode, aux2_mode;
#if XSP3_CONF_SOFTWARE_ROI_LUT
	u_int16_t *roi_lut;
#endif
	u_int8_t *pileup_time_lut;
	u_int32_t expected_frame, expected_packet;
	int dropped_frames;
	char busy, reset_frame;
	int good_thres;
	int good_grade_mode;
	struct window_limits
	{
		int low, high;
	} window[XSP3_SOFT_SCALER_NUM_WINDOWS];
	u_int32_t format_reg;
	int cur_tf;
	int prev_hw_time_frame;			//!< Used to kepp track of hardware time frame and detect wrap rounf at 24 bits
	int64_t cur_tf_ext;				//!< Extended current time from when using the recirulating buffer.
	int64_t first_circ_overrun; 	//!< First frame to suffer overrun in circular buffer mode. 
	int64_t num_circ_overrun;		//!< Number of over run frames in circular buffer mode.
	int64_t tf_top;				//!< Counts number of wrap rounds of firmware Time Frame
	u_int16_t *diffs_ptr;	// Used for saving differences mode data into modifed scope mode module.
	u_int16_t *tf_ptr;		// Used for saving time frame part of differences mode data into modifed scope mode module.
	u_int16_t *dig_ptr;		// Used for saving Digital part of differences mode data into modifed scope mode module.
	int debug_burst;		// Count down a burst of debug messages from withing histogram thread and then stop.
	pthread_mutex_t mutex;		// Mutex between receive threads and control threads.
	cpu_set_t cpu_set;			// CPU set to run scope mode on if specified
	int num_sub_frames, ts_divide;
	int list_mode_fd;
	double gain_scaling;	// Copy of gain scaling implemented in quotient BRAM. Note may not be restired by restore settings. For use in da.server only.
} Histogram;

typedef struct clock_setup_struct
{
	int clk_src;		//!< Clk source 
	int flags;			//!< Clk and ADC flags interpreted to suit differetn Xspress Generations
	int tp_type;		// Test pattern type, used only in xspress3.
	int adc_clk_delay;	//!< ADC clock delay in LMK03200 for Xspress3 only.
	int fpga_clk_delay;	//!< FPGA clock delay in LMK03200 for Xspress3 only.
} ClockSetup; 

typedef enum {
Xsp3SysLog_StartNewFile=1,
Xsp3SysLog_KeepFiles=2,
Xsp3SysLog_DisableADCTemp=0x10
} Xsp3SysLogFlags;

typedef struct xsp3_sys_log_struct 
{
	int active;
	Xsp3SysLogFlags flags;
	char *fname;
	int debug;
	FILE *ofp;
	int period;
	pthread_t tid;
	int count;
	int max_count;
	int max_file_num;
	int error_code;
	struct timeval start_time;
} Xsp3SysLogger;


typedef struct _XSP3Path {
	void* femHandle;				//!< Pointer to data structure managing TCP communication with the FEM/ZYNQ.
	int type;						//!< FEM_COMPOSITE => Parent node of multi-card system. FEM_SINGLE => leaf node of multi card system or singe node for 1 card system.
	int valid;
	int debug;						//!< print out debug information
	int sub_path[XSP3_MAX_CARDS+1];
	int chans_per_card;				//!< Number of channels per card = max_num_chans, from leaves in multi card system.
	int num_cards;					//!< Number of cards in the system
	int num_chan;					//!< Number of channels requested on the system (top) or card (leaf)
	int max_num_chan;				//!< Maximum total number of channels available in system (on top) or on card for leaf
	int revision;					//!< Card Revision
	int total_lwords_per_card;
	UDPconnection udpsock;
	struct xsp3_scope_data_module *scope_mod;				//!< Pointer to scope mode data module.
	pthread_t thread[XSP3_MAX_CHANS_PER_CARD];				//!< Thread numbers for the UDP data RX threads.
	char thread_created[XSP3_MAX_CHANS_PER_CARD];	
	volatile Histogram histogram[XSP3_MAX_CHANS_PER_CARD];	//!< Per channel histogamming data for communication with histogramming threads.
	ChannelDTC dtc[XSP3_MAX_CHANS_PER_CARD];				//!< Dead time correction parameters per channel for this card.
	double deadtimeEnergy; 									//!< Current nominal beamline energy in keV NOT eV!
	int run_flags;											//!< Overall system run flags.
	u_int32_t hist_frames_num_tf;							//!< Xspress3 MINI BRAM histogrammed frames must be the same across all channel on a box, practicualy across the system
	u_int32_t scaler_start;  								//!< Start address offset of Scaler data within DRAM on FEM (FEM-1 Addr32 version), per card.
	u_int32_t scaler_size; 									//!< Size in bytes of Scaler data with DRAM on FEM, per card.
	u_int32_t total_scaler_num_tf;							//!< Total number of time frames of scaler data that will fit with channels set to num_chan, with no sub-frames
	u_int32_t scaler_num_tf;								//!< Number of time frames of scaler data that will fit with channels set to num_chan and current number of sub-frames.
	int scaler_num_sub_frames;								//!< Number of time frames of scaler data that will fit with channels set to num_chan and current number of sub-frames.
	int mdio_flags;											//!< Various options for MDIO access.
	int startingCard;										//!< Card index of first card, remembered to enable us to make default module names correctly
	Xspress3_features features;								//!< Features register set read and unpacked for this card
	MOD_IMAGE3D *scalers_mod;								//!< Pointer to /dev/shm data module for software scalars, used when features.soft_scalers != 0
	mh_com *scalers_mod_head;								//!< Pointer to module header for software scalars.
	MOD_IMAGE3D *tf_status_mod;								
	mh_com *tf_status_head;
	int disable_multi_thread;								//!< Disable Thread per card activity speed ups on scope mode, start etc. See {@link XSP3_MT_FLAGS}
	int chan_of_system;										//!< Used to initialise chan_of_system in histogram (only)
	char soft_lead_tail;									//!< Enable alternate event list processing for SGX detector.
	u_int32_t explicit_chan_cont[XSP3_MAX_CHANS_PER_CARD];	//!< Channel Control register value as set by xsp3_set_chan_cont
	int mem_layout;
	ClockSetup clock_setup;									//!< Clock setup information to allow xsp3_save_settings to save clock setup.
	double clock_period;									//!< Clock period in seconds, 12.5E-9 for xspress3, but can be checked by {@link xsp3_measure_clock_frequency()} in XSPRESS3Mini and XSPRESS4.
	cpu_set_t scope_cpu_set;								//!< CPU set to run leaf card scope mode and callibration code on.
	int sync_mode;											//!< Synchronisation cabling used for multibox systems. top path only. See XSP3_SYNC_NONE etc.
	int sf_warnings_sent;									//!< Allows moderate number of sub-frame warning to be sent
	int timing_src;											//!< Timing src for estimation of live_ticks in sub-frames mode
	u_int32_t itfg_col_time; 								//!< ITFG setup for calculatio/estimation of live_ticks in sub-frames mode, stored on top level (or only) path
	int itfg_trig_mode;										//!< ITFG setup for calculatio/estimation of live_ticks in sub-frames mode, stored on top level (or only) path
	int itfg_gap_mode;										//!< ITFG setup for calculatio/estimation of live_ticks in sub-frames mode, stored on top level (or only) path
	int itfg_acq_in_pause;									//!< ITFG setup for calculatio/estimation of live_ticks in sub-frames mode, stored on top level (or only) path
	Xsp3SysLogger sys_log;									//!< System looger information, used in top (or single) path only.
	} XSP3Path; 

typedef struct trigger_b_setttings
{
	int avemode;
	int two_over_mode, enable, enb_otd_split;
	int disable_split, combined;
	int scaled_thres_mode;
	int arm_thres, end_thres, rearm_thres;
	int sep1, sep2, data_delay, event_time;
	int over_thres_delay, over_thres_stretch, over_thres_trim;
	int enb_fast_otd, fast_avemode, fast_sep1, fast_align, fast_arm_thres, fast_end_thres;
	int enb_variable_width, variable_width_delay, enb_split_touching, enb_fast_diff2;
} Xspress3_TriggerB;

typedef struct trigger_c_setttings
{
	int arm_thres, end_thres;
	int sep1;
	int avemode;
	int two_over_mode, enb_negative, ignore_near_glitch;
	int over_thres_servo_delay, over_thres_servo_stretch, over_thres_servo_trim;
	int use_det_reset_b, use_det_reset_c;
	int min_width;
} Xspress3_TriggerC;

typedef enum {Xsp3TP_Inc, Xsp3TP_IncPlusPeaks} Xsp3TestPattern;

typedef	struct pileup_times_struct
{
	int eng, width; 
}  XSP3_PileupTimes;
typedef enum {
	Xsp3ErrFlag_Playback=1, //!< Under run of data from Playback DMA
	Xsp3ErrFlag_Scope=2,    //!< Over run of data into Scope DMA(s)
	Xsp3ErrFlag_HistFramesOverRun=4,    //!< Firmware detected Over run of counts and Frames into BRAM histogrammer (Xspress3 Mini Only)
	Xsp3ErrFlag_HistFramesMissing=8,    //!< Software detected bad or missing frames  from BRAM histogrammer into DRAM (Xspress3 Mini Only)
	Xsp3ErrFlag_ScalarOverRun=0x10,    	//!< Firmware detected Over run Scalar transfer into DRAM (Xspress3 Mini Only)
	Xsp3ErrFlag_ScalarMissing=0x20,     //!< Software detected bad or missing frames Scalars into DRAM (Xspress3 Mini Only)
	Xsp3ErrFlag_DiagHistOverRun=0x40,   //!< Firmware detected dropped events on Diag Hist (Xspress3 Mini Only) (Not severe error)
	Xsp3ErrFlag_CircBuffOverRun=0x10000 //!< Software detected overrun of circulr buffer.
} Xsp3ErrFlag;

typedef struct {
	u_int8_t addr, data;
} Xsp3I2CAddrData;

typedef struct {
 	int t_src;				//!< Timing source as defined by * The time frame source bits are defined as:  @snippet xspress3.h XSP3_GLOBAL_TIMEA_SOURCE
	u_int32_t fixed;		//!< Fixed time frame used for the starting frame (usually 0)
	int inv_f0;				//!< Enable inversion of the Frame 0 to make it active low.
	int inv_veto;			//!< Enable inversion of Count enable input so the ssytem counts when the input is low.
	int loop_io;			//!< Enable test mode which loop TTL inputs to outputs
	int debounce;			//!< Debounce time for card 0 trigger inputs in ADC clock cycles.
	int override_debounce_other;	//!< Optional override of the debounce time for the trigger inputs on subsequent cards.
	u_int32_t card0_a_extra;	//!< Extra bits to mask into card 0 timeA (usually 0)
	u_int32_t cardn_a_extra;	//!< Extra bits to mask into other card timeA (usually 0)
	int padding[2];			//!< For future expansion
} Xsp3Timing;

typedef struct 
{
	int enable;				//!< Enable Charge sharing functions (currently permanentley enabled at firmware level)
	int coinc_mode;			//!< Coincence mode, see XSP3_CSHR_COINC_ANY, XSP3_CSHR_COINC_PM2 etc.
	int enb_neb_trig;		//!< Enable generation of a dummy trigger on neighbour channels which have some shared charge, to exclude this charge from the preceding and folowwing events on that channel.
	int enb_mask_large;		//!< Assume that any "large" events are not charge sharing and ignore.
	int override_discard;	//!< -1 to use default discard setting when FIFOs overfill, 0 to disable, > 0 to enable selected.
	int unused[8];			//!< For future expansion
} Xsp3CShrControl;


#ifdef __cplusplus
extern "C" {
#endif

/* XSPRESS3 forward declaration*/
int 	xsp3_config(int ncards, int num_tf, char* baseIPaddress, int basePort, char* baseMACaddress, int nchan, int createmodule, char* modname, int debug, int card_index);
int     xsp3_config_init(int ncards, int num_tf, char* baseIPaddress, int basePort, char* baseMACaddress, int num_chan,	int create_module, char* modname, int debug, int card_index, int do_init);
int     xsp3_set_debug(int path, int level, int burst_len);
int 	xsp3_do_config(int ncards, int num_tf, char* baseIPaddress, int basePort, char* baseMACaddress, int num_chan, int create_module, char* modname, int debug, int card_index, int do_init);
int		xsp3_config_tcp(char femHostName[][XSP3_MAX_IP_CHARS], int femPort, int card, int chan, int debug);
int 	xsp3_close(int path);
char* 	xsp3_get_error_message();
int		xsp3_get_revision(int path);
int 	xsp3_get_features(int path, int card, Xspress3_features * features);
int 	xsp3_get_num_chan(int path);
int		xsp3_set_num_chan(int path, int num);
int 	xsp3_get_num_cards(int path);
int 	xsp3_get_chans_per_card(int path);
int 	xsp3_get_num_chan_used(int path, int card);
int 	xsp3_resolve_path_chan_card(int path, int chan, int *thisPath, int *chanIdx, int *cardP);
int 	xsp3_resolve_path(int path, int chan, int *thisPath, int *chanIdx);
int 	xsp3_set_trigger_b(int path, int detector, Xspress3_TriggerB *trig_b);
int 	xsp3_get_trigger_b(int path, int chan, Xspress3_TriggerB *trig_b);
int 	xsp3_set_trigger_c(int path, int detector, Xspress3_TriggerC *trig_c);
int 	xsp3_get_trigger_c(int path, int chan, Xspress3_TriggerC *trig_c);
int 	xsp3_set_trigger_b_ringing(int path, int chan, double scale_a, int delay_a, double scale_b, int delay_b);
int 	xsp3_get_trigger_b_ringing(int path, int chan, double *scale_a, int *delay_a, double *scale_b, int *delay_b);
int 	xsp3_trigger_b_get_ringing_params(int path, int *min_delay, int *max_delay);
int		xsp3_get_max_num_chan(int path);
int 	xsp3_set_window(int path, int chan, int win, int low, int high);
int 	xsp3_set_good_thres(int path, int chan, u_int32_t good_thres);
int xsp3_set_trigger_regs_b(int path, int chan, u_int32_t trigb_thres, u_int32_t trigb_timea, u_int32_t trigb_timeb, u_int32_t trigb_fast, u_int32_t trigb_timec); 
int 	xsp3_set_trigger_regs_c(int path, int chan, u_int32_t trigc_otd_servo, u_int32_t trigc_thres);
int 	xsp3_set_chan_cont(int path, int chan, u_int32_t chan_cont);
int 	xsp3_set_format_reg(int path, int chan, u_int32_t format);
int 	xsp3_set_reset(int path, int chan, u_int32_t resetA, u_int32_t resetB, u_int32_t resetC);
int 	xsp3_set_glitch(int path, int chan, u_int32_t glitchA, u_int32_t glitchB);
int 	xsp3_set_servo(int path, int chan, u_int32_t servoA, u_int32_t servoB, u_int32_t servoC);
int 	xsp3_get_window(int path, int chan, int win, u_int32_t *low, u_int32_t *high);
int 	xsp3_get_good_thres(int path, int chan, u_int32_t *good_thres);
int 	xsp3_get_trigger_regs_b(int path, int chan, u_int32_t *trigb_thres, u_int32_t *trigb_timea, u_int32_t *trigb_timeb, u_int32_t *trigb_fast, u_int32_t *trigb_timec);
int 	xsp3_get_trigger_regs_c(int path, int chan, u_int32_t *trigc_otd_servo, u_int32_t *trigc_thres);
int 	xsp3_set_cal_events(int path, int chan,int enable, int period, int avoid);
int 	xsp3_get_glitch(int path, int chan, u_int32_t *glitchA, u_int32_t *glitchB);
int 	xsp3_write_reg(int path, int chan, int region, int offset, int size, u_int32_t *value);
int 	xsp3_read_reg(int path, int chan, int region, int offset, int size, u_int32_t *value);
int 	xsp3_get_servo(int path, int chan, u_int32_t *servoA, u_int32_t *servoB, u_int32_t *servoC);
int 	xsp3_get_chan_cont(int path, int chan, u_int32_t *chan_cont);
int 	xsp3_get_format_reg(int path, int chan, u_int32_t *format);
int 	xsp3_get_format(int path, int chan, int *nbins_eng, int *nbins_aux1, int *nbins_aux2, int * nbins_tf);
int 	xsp3_get_format_diag(int path, int chan, int diag, int *nbins_eng, int *nbins_aux1, int *nbins_aux2, int * nbins_tf);
int 	xsp3_get_reset(int path, int chan, u_int32_t *resetA, u_int32_t *resetB, u_int32_t * resetC);
int 	xsp3_write_glob_reg(int path, int card, int offset, int size, u_int32_t *value);
int 	xsp3_read_glob_reg(int path, int card, int offset, int size, u_int32_t *value);
int 	xsp3_read_raw_reg(int path, int card, u_int32_t address, int size, u_int32_t *value);
int		xsp3_set_clock_control(int path, int card, u_int32_t clock);
int		xsp3_get_clock_control(int path, int card, u_int32_t *clock);
int		xsp3_set_scope(int path, int card, u_int32_t scope_chn, u_int32_t scope_src, u_int32_t scope_nwd, u_int32_t scope_alt);
int 	xsp3_get_scope(int path, int card, u_int32_t *scope_chn, u_int32_t *scope_src, u_int32_t *scope_nwd, u_int32_t *scope_alt);
int 	xsp3_scope_settings_from_mod(int path);
int 	xsp3_scope_settings_to_mod(int path);
int 	xsp3_set_scope_options(int path, int card, Xsp3ScopeOptions *options);
int 	xsp3_set_scope_stream(int path, int card, int stream, u_int32_t chan, u_int32_t src, u_int32_t alt);
int 	xsp3_set_glob_timeA(int path, int card, u_int32_t time);
int 	xsp3_set_glob_timeFixed(int path, int card, u_int32_t time);
int 	xsp3_get_glob_timeA(int path, int card, u_int32_t *time);
int 	xsp3_get_glob_timeFixed(int path, int card, u_int32_t *time);
int		xsp3_dma_reset(int path, int card, u_int32_t function_mask);
int 	xsp3_dma_config_memory(int path, int card, int layout);
int 	xsp3_dma_get_memory_config(int path, int card);
int 	xsp3_dma_build_desc(int path, int card, u_int32_t func, XSP3_DMA_MsgBuildDesc *msg);
int 	xsp3_dma_build_debug_desc(int path, int card, u_int32_t stream, XSP3_DMA_MsgBuildDebugDesc *msg);
int 	xsp3_dma_start(int path, int card, u_int32_t stream, XSP3_DMA_MsgStart *msgStart);
int 	xsp3_dma_build_test_pat(int path, int card, u_int32_t func, XSP3_DMA_MsgTestPat *msg);
int 	xsp3_dma_print_data(int path, int card, u_int32_t func, XSP3_DMA_MsgPrint *msg);
int 	xsp3_dma_print_scope_data(int path, int card, XSP3_DMA_MsgPrint *msg);
int 	xsp3_dma_print_desc(int path, int card, u_int32_t stream, XSP3_DMA_MsgPrintDesc *msg);
int 	xsp3_get_dma_status_block(int path, int card, XSP3_DMA_StatusBlock *statusBlock);
int 	xsp3_dma_check_desc(int path, int card, u_int32_t stream, XSP3_DMA_MsgCheckDesc *msg, u_int32_t *completed_desc, u_int32_t *last_frame, u_int32_t *status);
int 	xsp3_dma_resend(int path, int card, u_int32_t stream, u_int32_t first, u_int32_t num);
int 	xsp3_dma_reuse(int path, int card, u_int32_t stream, u_int32_t first, u_int32_t num);
int 	xsp3_dma_read_status(int path, int card, u_int32_t stream_mask);
int 	xsp3_dma_read_buffer(int path, int stream, int offset, int size, u_int32_t *value);

int 	xsp3_scope_wait(int path, int card);
int		xsp3_config_udp(int path, int card, char *femMACaddress, char*femIPaddress, int femPort, char* hostIPaddress, int hostPort);
int 	xsp3_config_histogram_udp(int path, int card, char *hostIPaddress, int hostPort, char *femIPaddress, int femPort);
int 	xsp3_config_histogram_threads(int path, int card);
int 	xsp3_udp_set_1_buf_size(UDPconnection *udpsock, int buf_size);
int		xsp3_set_udp_port(int path, int card, int hostPort);
int 	xsp3_set_udp_packet_size(int path, int card, int size_bytes);
int 	xsp3_set_udp_buffer_size(int path, int card, int hist_ports, int scope_ports);
int 	xsp3_get_udp_buffer_size(int path, int card, int *hist_ports, int *scope_ports);
int		xsp3_read_scope_data(int path, int card);
int		xsp3_read_scope_data_int(int path, int card, int swap, int enb_retry);
int 	xsp3_read_scope_data_debug(int path, int card, int enb_retry); 
int 	xsp3_create_data_module(int path, char* modname, int layout);
int		xsp3_read_rdma_reg(int path, int card, int address, int size, u_int32_t *value);
int		xsp3_write_rdma_reg(int path, int card, int address, int size, u_int32_t *value);
int		xsp3_read_spi_reg(int path, int card, int address, int size, u_int32_t *value);
int		xsp3_write_spi_reg(int path, int card, int address, int size, u_int32_t *value);
int		xsp3_write_dram(int path, int card, int address, int size, u_int32_t* value);
int 	xsp3_reset_10g_frame_counter(int path, int card);
int	 	xsp3_read_data_10g(int path, int card, int stream, int offset_bytes, unsigned size_bytes, unsigned char *buff, int enb_retry);
int 	xsp3_read_data_10g_receive(int path, int card, XSP3_DMA_StatusBlock * statusBlock, unsigned char *buff_base, u_int32_t  first_frame, u_int32_t num_frames, u_int32_t *frame_flags);
int		xsp3_write_playback_data(int path, int card, u_int32_t* buffer, size_t nbytes, int no_retry);
int 	xsp3_write_data_10g(int path, int card, u_int32_t* buffer, int dst_stream, int offset_bytes, size_t nbytes, int no_retry);
int 	xsp3_histogram_mkmod(int path, int chan, char *root_name, int num_tf, int do_init);
int 	xsp3_calib_histogram_mkmod(int path, int chan, char *root_name, int num_tf);
int		xsp3_histogram_start(int path, int card);
int		xsp3_histogram_arm(int path, int card);
int 	xsp3_histogram_start_count_enb(int path, int card, int count_enb);
int		xsp3_histogram_stop(int path, int card);
int 	xsp3_histogram_clear(int path, int first_chan, int num_chan, int first_frame, int num_frames);
int 	xsp3_histogram_read4d(int path, u_int32_t *buffer, unsigned eng, unsigned aux, unsigned chan, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf);
int 	xsp3_histogram_read3d(int path, u_int32_t *buffer,	unsigned x, unsigned y, unsigned t, unsigned dx, unsigned dy, unsigned dt);
int 	xsp3_histogram_read_chan(int path, u_int32_t *buffer, unsigned chan, unsigned eng, unsigned aux, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_tf);
int 	xsp3_histogram_write_test_pat(int path, Xsp3TestPattern type);
int 	xsp3_histogram_get_dropped(int path, int chan);
int 	xsp3_histogram_is_busy(int path, int chan);
int 	xsp3_histogram_is_any_busy(int path);
int 	xsp3_scaler_check_progress(int path);
int 	xsp3_scaler_get_num_tf(int path);
int 	xsp3_scaler_check_desc(int path, int card);
int 	xsp3_scaler_read(int path, u_int32_t *dest, unsigned scaler, unsigned chan, unsigned t, unsigned n_scalers, unsigned n_chan, unsigned dt);
int 	xsp3_config_scaler(int path);
int 	xsp3_scaler_dtc_read(int path, double *dest, unsigned scaler, unsigned chan, unsigned t, unsigned n_scalers,
			unsigned n_chan, unsigned dt);
int 	xsp3_hist_dtc_read4d(int path, double *hist_buff, double *scal_buff, unsigned eng, unsigned aux, unsigned chan, unsigned tf,
			unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf);
int 	xsp3_hist_dtc_read3d(int path, double *hist_buff, unsigned eng, unsigned y, unsigned tf, unsigned num_eng, unsigned dy, unsigned num_tf);
int		xsp3_setDeadtimeCalculationEnergy(int path, double energy);
double 	xsp3_getDeadtimeCalculationEnergy(int path);
int 	xsp3_setDeadtimeCorrectionParameters(int path, int chan, int flags, double processDeadTimeAllEventGradient,
			double processDeadTimeAllEventOffset, double processDeadTimeInWindowOffset, double processDeadTimeInWindowGradient);
int		xsp3_getDeadtimeCorrectionParameters(int path, int chan, int *flags, double *processDeadTimeAllEventGradient,
			double *processDeadTimeAllEventOffset, double *processDeadTimeInWindowOffset, double *processDeadTimeInWindowGradient);
int 	xsp3_getDeadtimeCorrectionFlags(int path, int chan, int *flags);
int 	xsp3_calculateDeadtimeCorrectionFactors(int path, u_int32_t* hardwareScalerReadings, double* dtcFactors, double *inpEst, int num_tf, int first_chan, int num_chan);

int 	xsp3_set_roi(int path, int chan, int num_roi, XSP3Roi *roi);
int 	xsp3_init_roi(int path, int chan);

MOD_IMAGE *xsp3_mkmod(char *name, int num_x, int num_y, char *x_lab, char *y_lab, int data_type, mh_com **mod_head);
MOD_IMAGE3D * xsp3_mkmod3d ( char *name, int num_x, int num_y, int num_t, char *x_lab, char *y_lab, char *t_lab, char ** labels, int data_type, mh_com **mod_head);
u_int32_t *xsp3_mod_get_ptr(void *p, int x, int y, int t);

struct	xsp3_scope_data_module * xsp3_scope_get_module(int path); 

int 	xsp3_system_start(int path, int card);
int 	xsp3_system_arm(int path, int card);
int 	xsp3_histogram_continue(int path, int card);
int 	xsp3_histogram_pause(int path, int card);

int 	xsp3_system_start_count_enb(int path, int card, int count_enb, int pb_num_t, int scope_num_t);
int     xsp3_set_run_flags(int path, int flags);
int     xsp3_get_run_flags(int path);
int		xsp3_get_bins_per_mca(int path);
int 	xsp3_bram_init(int path, int chan, int region, double scaling);
int 	xsp3_register_init(int path, int chan);

int 	xsp3_mdio_display(int path, int card);
int 	xsp3_mdio_set_connection(int path, int card, int conn);
int 	xsp3_mdio_write(int path, int card, int port, int device, int addr, int data);
int 	xsp3_mdio_read(int path, int card, int port, int device, int addr, u_int32_t *data);
int 	xsp3_mdio_read_inc(int path, int card, int port, int device, u_int32_t *data);

int 	xsp3_clocks_setup(int path, int card, int clk_src, int flags, int tp_type);
int 	xsp3_clocks_setup_int(int path, int card, int clk_src, int flags, int tp_type, int adc_clk_delay, int fpga_clk_delay );
int		xsp3_set_ppc_debuglevel(int path, int card, int ppc1, int ppc2, int level);

//int 	xsp3_get_resmode(int path, int chan, int *res_mode, int *res_thres);
int xsp3_get_aux1_mode(int path, int chan, int *aux1_mode, int *aux1_thres);

int 	xsp3_nbits_aux1(int path, int res_mode);
int 	xsp3_nbits_adc(int path, int adc_cont);
int 	xsp3_get_max_ave(int path, int chan);
int 	xsp3_trigger_b_get_diff_params(int path, int *sep_offset, int *sep_max);
int 	xsp3_format_run(int path, int chan, int aux1_mode, int res_thres, int aux2_cont, int disables, int aux2_mode, int nbits_eng);
int 	xsp3_format_sub_frames(int path, int chan, int just_good, int just_good_thres, int flags, int nbits_eng, int num_sub_frames, int ts_divide);
int 	xsp3_format_run_int (int path, int chan, int aux1_mode, int aux1_thres, int aux2_cont, int flags, int aux2_mode, int nbits_eng, int num_sub_frames, int ts_divide);
int 	xsp3_set_data_buffer(int path, int chan, u_int32_t*buffer, u_int32_t bufsiz);
int 	xsp3_set_data_module(int path, int chan, MOD_IMAGE* module, u_int32_t* buffer, u_int32_t bufsiz);
int 	xsp3_get_num_tf(int path);
int 	xsp3_set_num_tf(int path, int num_tf);

int 	xsp3_set_scope_search(int path, int chan, u_int32_t scope_search);
int 	xsp3_get_scope_search(int path, int chan, u_int32_t *scope_search);

int 	xsp3_save_settings(int path, char *dir_name);
int 	xsp3_restore_settings(int path, char *dir_name, int force_mismatch);
int 	xsp3_restore_settings_and_clock(int path, char *dir_name, int force_mismatch);
int 	xsp3_get_glob_time_statA(int path, int card, u_int32_t *time);

int 	xsp3_set_data_mux_cont(int path, int card, u_int32_t mux_cont);
int 	xsp3_get_data_mux_cont(int path, int card, u_int32_t *mux_cont);

MOD_IMAGE* xsp3_get_data_module(int path, int chan);

int 	xsp3_i2c_read_adc_temp(int path, int card, float *temp);
int 	xsp3_i2c_set_adc_temp_limit(int path, int card, int critTemp);
int 	xsp3_write_fan_cont(int path, int card, int offset, int size, u_int32_t* value);
int 	xsp3_read_fan_cont(int path, int card, int offset, int size, u_int32_t *value);

int     xsp3_playback_load_x2(int path, int card, char *filename0, char *filename1, char *filename2, char *filename3, char *filename4, char *filename5, char *filename6, char *filename7, 
                              int do_test, int do_scale, int do_swap, int reverse, int smooth_join, int glob_reset, int enb16chan, int no_retry); 
int 	xsp3_playback_load_x3(int path, int card, char *filename, int src[16],  int file_streams, int str0dig, int smooth_join, int enb_higher_chan, int no_retry, int xspress4_dig, int glob_reset); 
int 	xsp3_playback_load_x3_multi(int path, int card, char ** file_list, int src[16],  int file_streams_list[XSP3_MAX_PLAYBACK_FILES], int str0dig, int smooth_join, int enb_higher_chan, int no_retry, int xspress4_dig, int glob_reset);
int 	xsp3_read_fem_config(int path, int card, int offset, int size, u_int8_t *value);
int 	xsp3_write_fem_config(int path, int card, int offset, int size, u_int8_t* value);

int 	xsp3_features_unpack(int path, int card); 
int 	xsp3_has_soft_lut(int path, int chan, int region_num);
int 	xsp3_has_bram(int path, int chan, int region_num);
int 	xsp3_write_soft_lut(int path, int chan, int region_num, int nwords, u_int32_t * data);
int 	xsp3_read_soft_lut(int path, int chan, int region_num, int nwords, u_int32_t * data);

void * 	read_and_histogram_hgt64(void* args);
void *  read_and_save_diffs(void* args);

int	 	xsp3_soft_scaler_read(int path, u_int32_t *dest, unsigned first_scaler, unsigned first_chan, unsigned first_t, unsigned n_scalers, unsigned n_chan, unsigned dt);
int 	xsp3_soft_scaler_clear(int path, int first_chan, int first_frame, int num_chan, int num_frames);
int 	xsp3_config_soft_scaler(int path, char * mod_name, int num_tf);
int 	xsp3_get_has_soft_scalers(int path);
int 	xsp3_build_pileup_time(int path, int chan, int num_pairs, XSP3_PileupTimes *pileup_time, char *fname);

int 	xsp3_itfg_setup(int path, int card, int num_tf, u_int32_t col_time, int trig_mode, int gap_mode);
int 	xsp3_itfg_setup2(int path, int card, int num_tf, u_int32_t col_time, int trig_mode, int gap_mode, int acq_in_pause, int marker_period, int marker_frame);
int 	xsp3_itfg_get_setup(int path, int card, int *num_tf, u_int32_t *col_time, int *trig_mode, int *gap_mode);
int 	xsp3_itfg_stop(int path, int card);
int 	xsp3_itfg_start(int path, int card);
int 	xsp3_has_itfg(int path, int card);
int 	xsp3_has_reset_det(int path, int chan);
int 	xsp3_has_glitch_det(int path, int card, int *min_thres, int *with_rst_xtk);
int 	xsp3_has_scope_dig_alt0(int path, int card);
int 	xsp3_bram_size(int path, int chan, int region_num);
int 	xsp3_has_lead_tail_corr_width(int path, int chan, int region_num, int *num_t, int *num_wid);
int 	xsp3_has_lead_corr(int path, int card);
int 	xsp3_has_servo_bi_linear_time(int path, int chan);
int 	xsp3_has_servo_reset_pre_time(int path, int chan);
XspressGeneration xsp3_get_generation(int path, int card);
int 	xsp3_get_playback_nstreams(int path, int card);
int 	xsp3_get_ringing_sub(int path, int chan);
int 	xsp3_has_dual_diff_trig(int path, int chan);
int 	xsp3_has_varaiable_width_cfd(int path, int chan);
int 	xsp3_has_fine_time(int path, int chan);
int     xsp3_get_xtk_corr(int path, int card);
int     xsp3_get_num_scope_dma(int path, int card);

int 	xsp3_set_global_reset_gen(int path, int card, int enable, int sync_mode, int det_reset_width, int hold_off_time, int gr_active_del, int gr_active_wid, int circ_offset);

int 	xsp3_get_disable_threading(int path);
int 	xsp3_set_disable_threading(int path, int flags);
int 	xsp3_rmw_glob_reg(int path, int card, int offset, u_int32_t and_mask, u_int32_t or_mask, u_int32_t* value);
int 	xsp4_rmw_glob_reg(int path, int card, int offset, u_int32_t and_mask, u_int32_t or_mask, u_int32_t* value);
int 	xsp3_mdio_display_10baser(int path, int card);

int 	xsp3_check_settings(int path, int chan);

int xsp3_set_xtk_corr(int path, int chan, int len, int pre_samples, int min_eng, int max_delete, int delete_mode, int enb_servo_delete_trig_b, int servo_max_delete, int servo_delete, int del_min_agg, int disable_split, int servo_pre_time, int servo_stretch, int discard_flags);

int 	xsp3m_write_dma_buff(int path, int card, int stream, int offset, int size, u_int32_t* value);
int     xsp3m_read_dma_buff(int path, int card, int stream, int offset, int size, u_int32_t *value);
int 	xsp3m_histogram_read_frames(int path, u_int32_t *buffer, unsigned eng, unsigned chan, unsigned tf, unsigned num_eng, unsigned num_chan, unsigned num_tf);
int 	xsp3m_zero_dma_buff(int path, int card, int stream, int offset, int size);
int 	xsp3_read_format(int path, int chan, int diag_hist, int *num_used_chanP, int *nbins_eng, int *nbins_aux1, int *nbins_aux2, int * nbins_tf );
int  	xsp3m_scaler_read(int path, u_int32_t *dest, unsigned scaler, unsigned chan, unsigned t, unsigned n_scalers, unsigned n_chan, unsigned dt) ;
int 	xsp3_dma_config_memory_int(int path, int card, int layout, int just_read, int force);
int 	xsp4_get_dma_status_block(int path, int card, XSP4_DMA_StatusBlock *statusBlock);

int 	xsp3_diag_histogram_read4d(int path, u_int32_t *buffer, unsigned eng, unsigned aux, unsigned chan, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf);
int 	xsp3_diag_histogram_read3d(int path, u_int32_t *buffer, unsigned x, unsigned y, unsigned t, unsigned dx, unsigned dy, unsigned dt);
int 	xsp3_diag_histogram_read_chan(int path, u_int32_t *buffer, unsigned chan, unsigned eng, unsigned aux, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_tf);
int 	xsp3_diag_histogram_write_test_pat(int path, int chan);
int 	xsp3m_read_dma_buff_roi(int path, int card, int stream, int offset, int row_len, int num_rows, int src_stride, int dst_stride, u_int32_t *value);
int 	xsp3_get_glob_status_a(int path, int card, u_int32_t *status) ;
int64_t xsp3_scaler_check_progress_details(int path, Xsp3ErrFlag *flagsP, int quiet, int64_t *furthest_frame);

int 	xsp4_get_adc_brd_status(int path, int card, u_int32_t *status); 
int 	xsp4_get_aurora_status(int path, int card, u_int16_t *aurora_status);
int 	xsp4_set_aurora_cont(int path, int card, u_int32_t control);
int 	xsp4_set_adc_cont(int path, int card, u_int32_t control);
int 	xsp4_get_aurora_cont(int path, int card, u_int32_t *control);
int 	xsp4_get_adc_cont(int path, int card, u_int32_t *control);
int 	xsp3_bram_init_xtk(int path, int chan, int b2b_stream, int enable);

int 	xsp4_write_mgt_drp(int path, int card, int link, int offset, int size, u_int32_t *value);
int 	xsp4_read_mgt_drp(int path, int card, int link, int offset, int size, u_int32_t *value);


int 	xsp3_write_chan_spi_reg(int path, int chan, int region, int size, u_int16_t* value);
int 	xsp3_read_chan_spi_reg(int path, int chan, int region, int n, u_int16_t* value);

int 	xsp3_set_gain(int path, int first, int num, int stage1, int stage2);
int 	xsp3_set_offset(int path, int first, int num, int offset);
int 	xsp3_set_ignore_over_temp(int path, int card, int value) ;
int 	xsp3_get_gain(int path, int first, int num, int *stage1, int *stage2);

int 	xsp3_set_trig_in_term(int path, int card, int flags);
int 	xsp3_set_trig_out_term(int path, int card, int flags);

int 	xsp3_write_i2c_reg(int path, int card, int bus, int addr, int size, u_int8_t* value);
int 	xsp3_read_i2c_reg(int path, int card, int bus, int addr, int size, u_int8_t* value);

int 	xsp3_read_i2c_reg_addr(int path, int card, int bus, int addr, int reg_addr, int size, u_int8_t* value);
int 	xsp3_write_i2c_reg_addr(int path, int card, int bus, int addr, int reg_addr, int size, u_int8_t* value);

int 	xsp3_i2c_read_fem_temp(int path, int card, float *temp);

int 	xsp3_lmk61e2_write_from_file(int path, int card, int mid_plane, char *fname);
int 	xsp3_lmk61e2_write(int path, int card, int mid_plane, int num, Xsp3I2CAddrData *reg);
int 	xsp3_lmk61e2_save_to_eeprom(int path, int card, int mid_plane);
int 	xsp3_lmk61e2_read(int path, int card, int mid_plane, int reg_addr, int num, u_int8_t *reg);

int 	xsp3m_write_fan_speed(int path, int card, int speed);
int 	xsp3m_read_fan_speed(int path, int card, int *speed);

int 	xsp3_read_xadc(int path, int card, int first, int num, u_int32_t *data);

int 	xsp3_i2c_read_adc_temp_int(int path, int card, int *temp); 
int 	xsp3_histogram_circ_ack(int path, unsigned chan, unsigned tf, unsigned num_chan, unsigned num_tf) ;
int 	xsp3_set_rst_xtk(int path, int chan, u_int32_t rst_xtk);
int 	xsp3_get_rst_xtk(int path, int chan, u_int32_t *rst_xtk);
int 	xsp3_config_tf_status(int path, int num_tf);
int64_t xsp3_histogram_get_circ_overrun(int path, int chan, int64_t *firstP);
int 	xsp3_histogram_get_tf_status(int path, int chan, unsigned tf, Xsp3TFStatus *tf_status);
int 	xsp3_histogram_get_tf_markers(int path, int chan, unsigned tf, unsigned num_tf, int *markers);
int 	xsp3_i2c_print_lm82_regs(int path, int card);
int 	xsp3_read_adc_revision(int path, int card, int *revision);
int 	xsp3_read_psu_cont(int path, int card, int *regP) ;

int 	xsp3_write_dma_or_clk_reg(int path, int card, int offset, int size, u_int32_t* value);
int 	xsp3_read_dma_or_clk_reg(int path, int card, int offset, int size, u_int32_t *value);
int 	xsp3_adc_mmcm_setup(int path, int card, int divide, double phase, double phase_del, double phase_radial);
int 	xsp3_adc_mmcm_reload(int path, int card);
int 	xsp3_has_roi(int path, int chan);
int 	xsp3_measure_clock_frequency(int path, int card);
double 	xsp3_get_clock_period(int path, int card);
int 	xsp3_get_first_card(int path);
int 	xsp3_scope_cpu_set(int path, int card, cpu_set_t *cpu_set);
int 	xsp3_hist_cpu_set(int path, int chan, cpu_set_t *cpu_set);
int 	xsp3_hist_cpu_set_update(int path, int chan);
int 	xsp3_adc_mmcm_reset(int path, int card);
int 	xsp3_glob_register_init(int path, int card);
int 	xsp3_i2c_read_mid_plane_temp(int path, float *temp);
int 	xsp3_set_sync_mode(int path, int sync_mode, int enb_global_reset, int gr_card);
int 	xsp3_get_sync_mode(int path, int *sync_mode, int *enb_global_reset, int *gr_card);
int 	xsp3_dma_get_desc_status(int path, int card, u_int32_t stream, XSP3_DMA_MsgGetDescStatus *msg, u_int32_t *status);
int 	xsp4_write_spi_par_exp(int path, int card, int chip, int reg, int val);
int 	xsp4_read_spi_par_exp(int path, int card, int chip, int reg, int *val);
int 	xsp3_write_data_10g_int(int path, int card, u_int32_t* buffer, int dst_stream, int offset_bytes, size_t nbytes, int no_retry, int test_mode);
int 	xsp3_get_nbits_frac_be(int path, int chan);
int 	xsp3_set_alt_ttl_out(int path, int card, int alt_ttl, int force);
int 	xsp3_has_trigger_c(int path, int chan);
int 	xsp3_set_timing(int path, Xsp3Timing *setup);
int 	xsp3_write_unused_reg(int path, int card, int chan_of_card, int region, int offset, int size, u_int32_t* value);
int 	xsp3_get_trig_in_term(int path, int card, int *flags);
int 	xsp3_get_trig_out_term(int path, int card, int *flags);
int 	xsp3_set_num_sub_frames_reg(int path, int chan, u_int32_t sub_frames);
int 	xsp3_get_num_sub_frames_reg(int path, int chan, u_int32_t *sub_frames);
int 	xsp3_update_scalar_num_sub_frames(int path);
int 	xsp3_rmw_chan_reg(int path, int chan, int offset, u_int32_t and_mask, u_int32_t or_mask, u_int32_t* value);
int 	xsp3_set_user_ts_sync_mode(int path, int card, int mode);
int 	xsp3_scaler_read_sf(int path, u_int32_t *dest, unsigned scaler, unsigned first_sf, unsigned chan, unsigned t, unsigned n_scalers, unsigned n_sf, unsigned n_chan, unsigned dt);
int 	xsp3_soft_scaler_read_sf(int path, u_int32_t *dest, unsigned first_scaler, unsigned first_sf, unsigned first_chan, unsigned first_t, unsigned n_scalers, unsigned n_sf, unsigned n_chan, unsigned dt);
int 	xsp3_scaler_get_num_sub_frames(int path);
int 	xsp3_calculateDeadtimeCorrectionFactors_sf(int path, u_int32_t* hardwareScalerReadings, double* dtcFactors, double *inpEst, int num_tf, int first_chan, int num_chan, int num_sub_frames);
int 	xsp3_scaler_dtc_read_sf(int path, double *dest, unsigned scaler, unsigned first_sf, unsigned chan, unsigned t, unsigned n_scalers, unsigned n_sf, unsigned n_chan, unsigned dt);
int 	xsp3_hist_dtc_read4d_sf(int path, double *hist_buff, double *scal_buff, unsigned eng, unsigned aux, unsigned chan, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf);
int 	xsp3_set_calib_data_module(int path, int chan, MOD_IMAGE* module, u_int32_t* buffer, u_int32_t bufsiz);
int 	xsp3_calib_histogram_read3d(int path, u_int32_t *buffer, unsigned x, unsigned y, unsigned t, unsigned dx, unsigned dy, unsigned dt, int calib);
int xsp3_setDeadtimeCorrectionParameters2(int path, int chan, int flags, double processDeadTimeAllEventOffset, double processDeadTimeAllEventGradient,
	double processDeadTimeAllEventRateOffset, double processDeadTimeAllEventRateGradient, 
	double processDeadTimeInWindowOffset, double processDeadTimeInWindowGradient, double processDeadTimeInWindowRateOffset, double processDeadTimeInWindowRateGradient);
int xsp3_getDeadtimeCorrectionParameters2(int path, int chan, int *flags, double *processDeadTimeAllEventOffset, double *processDeadTimeAllEventGradient,
	double *processDeadTimeAllEventRateOffset, double *processDeadTimeAllEventRateGradient,
	double *processDeadTimeInWindowOffset, double *processDeadTimeInWindowGradient, double *processDeadTimeInWindowRateOffset, double *processDeadTimeInWindowRateGradient) ;

int xsp3_histogram_start_list_mode(int path, int chan, char *root_name);
int xsp3_histogram_stop_list_mode(int path, int chan);
int64_t xsp3m_dma_check_desc(int path, int card, u_int32_t stream, u_int64_t *completed_desc, u_int64_t *last_frame, u_int32_t *dma_status, u_int32_t *desc_status, u_int64_t *desc_frame); 
int xsp3_has_64bit_time_frame(int path);
int xsp3m_scaler_read_board(int path, int thisPath, u_int32_t *dest, unsigned scaler, int chanIdx, unsigned t, unsigned n_scalers, unsigned n_chan, unsigned this_n_chan, unsigned dt);
int xsp3_histogram_get_tf_status_block(int path, int chan, unsigned tf, unsigned ntf, Xsp3TFStatus *tf_status);
int xsp3m_histogram_get_tf_status_block(int path, int chan, unsigned tf, unsigned ntf, Xsp3TFStatus *tf_status);
int xsp3m_histogram_get_tf_markers(int path, int chan, unsigned tf, unsigned ntf, int *markers);
int xsp3m_histogram_circ_ack(int path, unsigned chan, unsigned tf, unsigned num_chan, unsigned num_tf); 

int xsp3_playback_generate(int path, int card, Xsp3GenDataType *gd_type);
int xsp3_playback_generate_est_counts(int path, int chan, unsigned int tf, Xsp3GenDataType *gd_type, int64_t ts1, int64_t ts2, int *est_minP, double * max_rateP);
int xsp3_playback_get_num_t(int path, int card); 
double xsp3_get_scaling(int path, int chan);
int xsp3_write_cshare_min_eng_mark(int path, int chan, int num_neb, int *value);
int xsp3_write_cshare_min_eng_trig(int path, int chan, int num_neb, int *value);
int xsp3_read_cshare_min_eng_mark(int path, int chan, int num_neb, int *value);
int xsp3_read_cshare_min_eng_trig(int path, int chan, int num_neb, int *value);
int xsp3_write_cshare_control(int path, int chan, Xsp3CShrControl *set);
int xsp3_read_cshare_control(int path, int chan, Xsp3CShrControl *set);
int xsp3_write_cshare_mapping(int path, int chan, int num_neb, int *rel_board, int *chan_of_card);
int xsp3_read_cshare_mapping(int path, int chan, int num_neb, int *rel_board, int *chan_of_card);


int xsp3_sys_log_start(int path, char *fname, int period, int max_count, Xsp3SysLogFlags flags);
int xsp3_sys_log_continue(int path);
int xsp3_sys_log_stop(int path);
int xsp3_sys_log_pause(int path);
int xsp3_sys_log_stop_or_pause(int path, int pause);
int xsp3_sys_log_roll_files(int path);
int xsp3_sys_log_open(int path, int truncate);

#ifdef __cplusplus
}
#endif


/* XSPRESS3 code */
/**
	@defgroup XSP3_CHANNEL_REGS_OFFSETS   Address offsets of per channel registers
	@ingroup XSP3_MACROS
	@{
*/
#define XSP3_REVISION		0   //!< Revision Number D[31..16] = major revision, D[15..0] = minor revision	
#define XSP3_REG_PRESENT   	1   //!< Read/Writeable register present Mask bit 0 = Register offset 0		
#define XSP3_CHAN_CONT		2   //!< General Chan Control functions										
#define XSP3_TP_CYCLES   	3   //!< Test pattern generator cycles register when present					
#define XSP3_RESETA			4	//!< Reset Control A														
#define XSP3_RESETB			5	//!< Reset Control B														
#define XSP3_RESETC			6	//!< Reset Control C														
#define XSP3_GLITCHA 		7   //!< Glitch control A														
#define XSP3_GLITCHB 		8   //!< Glitch control B														
#define XSP3_XTK_CORR_A 	9   //!< Crosstalk correction control A.
#define XSP3_TRIGB_THRES	10	//!< Individual Trigger B Threshold control								
#define XSP3_TRIGB_TIMEA 	11	//!< Individual Trigger B TimesA 											
#define XSP3_TRIGB_TIMEB	12	//!< Individual Trigger B TimesB 											
#define XSP3_TRIGBC_TIME 	13	//!< Individual Trigger B or C OTD servo  Times							
#define XSP3_TRIGC_THRES 	14	//!< Individual Trigger C Threshold and averaging							
#define XSP3_TRIGB_RINGING	15	//!< Ringing removal for Trigger B 1st differential 						

#define XSP3_CAL_CONT		16    //!< Cal Cont															

#define XSP3_SERVO_CONT_A	17	//!< Servo Control A 														
#define XSP3_SERVO_CONT_B	18	//!< Servo Control B 														
#define XSP3_SERVO_CONT_C	19	//!< Servo Control C 														

#define XSP3_TRIGB_FAST		21	//!< Trigger B fast half when present.
#define XSP3_TRIGB_TIMEC	22	//!< Individual Trigger B TimeC, used in variable Width CFD mode 											
#define XSP3_XTK_CORR_C 	23  //!< Crosstalk correction control C.

#define XSP3_WINDOW0_THRES	24   	//!< Window values for Window 0 Scaler	 							
#define XSP3_WINDOW1_THRES  25   	//!< Window values for Window 1 Scaler								
#define XSP3_GOOD_THRES 	26   	//!< Window values for Good Event Scaler								

#define XSP3_XTK_CORR_B 	27   //!< Crosstalk correction control B.
#define XSP3_FORMAT			28	//!< Format Control 															
#define XSP3_SCOPE_SEARCH  	29	//!< Special scope search aid, when configured         					

#define XSP3_RST_XTK  	    30	//!< Direct reset crosstalk padding from on chip neighbouring resets (For SGX)

#define XSP_MAX_NUM_CHAN_REG	32			//!< Maximum number of writeable channel registers use for initialise

#define XSP3_LIVE_TIME_SCAL		(32+0) 	   	//!< Total Time Scaler direct readback			
#define XSP3_RESET_TICKS_SCAL	(32+1) 	 	//!< Reset Ticks Scaler direct readback			
#define XSP3_RESET_COUNT_SCAL	(32+2) 	 	//!< Reset Count Scaler direct readback			
#define XSP3_ALL_EVENT_SCAL 	(32+3)		//!< All event Scaler direct readback			
#define XSP3_GOOD_EVENT_SCAL 	(32+4)		//!< All event Scaler direct readback			
#define XSP3_IN_WIDNOW0_SCAL 	(32+5)		//!< All event Scaler direct readback			
#define XSP3_IN_WIDNOW1_SCAL 	(32+6)		//!< All event Scaler direct readback			

#define XSP3_MAX_NUM_READ_CHAN_REG 39
#define XSP3_NUM_SUB_FRAMES		48			//!< Register to store number of sub frames in sub-frames mode, otherwise 0

#define XSP3_CSHR_CONT			49
#define XSP3_CSHR_MIN_ENG_MARK_A 50			//!< Minimum Energy on neighbouring channel to mark "Central" as charge shared (neighbours 3..0)
#define XSP3_CSHR_MIN_ENG_MARK_B 51			//!< Minimum Energy on neighbouring channel to mark "Central" as charge shared (neighbours 7(5)..4)
#define XSP3_CSHR_MIN_ENG_TRIG_A 52			//!< Minimum Energy on neighbouring channel to cause trigger on that channel to exclude shared charge (neighbours 3..0)
#define XSP3_CSHR_MIN_ENG_TRIG_B 53			//!< Minimum Energy on neighbouring channel to cause trigger on that channel to exclude shared charge (neighbours 7..4)
#define XSP3_CSHR_DEST_MAPPING_A 54			//!< Destination mapping for measured charge sharing back to "central/requesting channel"
#define XSP3_CSHR_DEST_MAPPING_B 55			//!< Destination mapping for measured charge sharing back to "central/requesting channel higher bits"
/** @} 
*/
#define XSP3_REVISION_GET_DETECTOR(x) (((x)>>24)&0xFF)
#define XSP3_REVISION_GET_MAJOR(x) (((x)>>12)&0xFFF)
#define XSP3_REVISION_GET_MINOR(x) ((x)&0xFFF)

#define XSP3_REVISION_MAJOR_X3_MIN 0
#define XSP3_REVISION_MAJOR_X3_MAX 15
#define XSP3_REVISION_MAJOR_X3V7_MIN 16
#define XSP3_REVISION_MAJOR_X3V7_MAX 31
#define XSP3_REVISION_MAJOR_X3MINI_MIN 32
#define XSP3_REVISION_MAJOR_X3MINI_MAX 47
#define XSP3_REVISION_MAJOR_X4_MIN 48
#define XSP3_REVISION_MAJOR_X4_MAX 63

#define XSP3_SYNC_MODE(x)			((x)&0xFF)	//!< Build sync_mode value. Coded on bits 7..0, other spare to code e.g. long cables, termination enable etc.
#define XSP3_SYNC_GET_MODE(x)		((x)&0xFF)	//!< get sync_mode value
#define XSP3_SYNC_NONE				0			//!< No box to box synchronisation (or unknown) Only software.
#define XSP3_SYNC_LEMO_RADIAL		1			//!< Syncronisation using the count enable signal on LEMO cables from BOX 0 radially to all other boxes.
#define XSP3_SYNC_LEMO_DAISYCHAIN	2			//!< Syncronisation using the count enable signal on LEMO cables from BOX 0 to all other boxes including daisy-chaining through Boxes (using TTL Oout).
#define XSP3_SYNC_IDC				3			//!< Syncronisation using the XSPRESS3 or XSPRESS3mini IDC cables.
#define XSP3_SYNC_MIDPLANE			4			//!< Syncronisation using the XSPRESS4 midplane radial LVDS signals.
#define XSP3_SYNC_LEMO_DAISYCHAIN_TEE	5		//!< Syncronisation using the count enable signal on LEMO cables from BOX 0 to all other boxes using a daisy-chaining of LEMO Tee pieces.

#define XSP3_SYNC_GR_CARD(x)		(((x)&0x3F)<<6)	//!< Build sync mode value to store which board (usually 0) is used to generate Global Reset
#define XSP3_SYNC_GET_GR_CARD(x)	(((x)>>6)&0x3F)	//!< Get which board (usually 0) is used to generate Global Reset
#define XSP3_SYNC_GLOBAL_RESET		0x8000			//!< Specify that Global reset is used and output from XSP3_SYNC_GR_CARD
 
//! [XSP3_CC_REGISTER]
#define XSP3_CC_SEL_DATA(x)			((x)&7)
#define XSP3_CC_SEL_DATA_NORMAL			0
#define XSP3_CC_SEL_DATA_ALTERNATE		1
#define XSP3_CC_SEL_DATA_MUX_DATA		2
#define XSP3_CC_SEL_DATA_EXT0			4
#define XSP3_CC_SEL_DATA_EXT1			5
#define XSP3_CC_SEL_DATA_PB_CHAN		6
#define XSP3_CC_SEL_DATA_GLOB_RST		7

#define XSP3_CC_DATA_INV				(1<<3)				//!< 1's complement the data when adc data ramps from high to low
#define XSP3_CC_DET_RESET_INV			(1<<4)
#define XSP3_CC_USE_TEST_PAT			(1<<5)
#define XSP3_CC_TP_CONTINUOUS			(1<<6)
#define XSP3_CC_USE_RESET_FROM_DIFF		(1<<7)
#define XSP3_CC_GR_MASKS_RESET_TICKS	(1<<8)
#define XSP3_CC_GR_FROM_PLAYBACK		(1<<9)
#define XSP3_CC_ENB_INL_CORR			(1<<10)
#define XSP3_CC_AVE_RINGING_REMOVED		(1<<11)
#define XSP3_CC_LIVE_TICKS_MODE(x)		(((x)&7)<<12)
#define XSP3_CC_GOOD_GRADE_MODE(x)		(((x)&3)<<15)
#define XSP3_CC_NO_EXTEND_RESET_TICKS	(1<<17)			//!< Do not extend the reset ticks until any event overlapping the end of reset finishes.
#define XSP3_CC_RUN_AVE_BY_WIDTH		(1<<18)			//!< Enable event lead/tail correction by OTD width
#define XSP3_CC_NEB_EVENT_MODE(x)		(((x)&7)<<20)
#define XSP3_CC_MAX_FILT_LEN(x)			(((x)&7)<<24)
#define XSP3_CC_GET_MAX_FILT_LEN(x)		(((x)>>24)&7)
#define XSP3_CC_FINE_END				(1<<27)			//!< Select FineStart(3 downto 2) & FineEnd(3 downto 2) instead of FineStart(3 downto 0)
#define XSP3_CC_SEL_ENERGY(x)			(((x)&0xF)<<28)

#define XSP3_CC_GET_GOOD_GRADE_MODE(x)		(((x)>>15)&3)

/**
@defgroup XSP3_CC_NEB_EVENT Neighbour event mode (Currently I20 64 el specific)
@ingroup XSP3_MACROS
@{
*/
#define XSP3_CC_NEB_EVENT_NONE			0	//!< Disabled
#define XSP3_CC_NEB_EVENT_TRIGGER		1	//!< Trigger channel on neighbour keep all events
#define XSP3_CC_NEB_EVENT_REJECT_LONE 	2	//!< Reject lone neighbour events
#define XSP3_CC_NEB_EVENT_REJECT_ALL  	3 	//!< Reject all neighbour events

/**
@}
*/

#define XSP3_CC_SEND_RESET_WIDTHS		(1<<12)			//!< Send reset widths in event list modes 


#define XSP3_CC_LT_OFF						0		//!< Live ticks are not counter on scalers.
#define XSP3_CC_LT_RESET_TICKS				1		//!< Live ticks are counted on Reset Ticks Counter (cannot compare with standard DTC, but suitable for run modes
#define XSP3_CC_LT_IN_WINDOW1				2 		//!< Live ticks are counted in the in-window1 counter. Both DTC techniques can be tried.
#define XSP3_CC_LT_PILEUP					3 		//!< Live ticks are counted in the pile-up counter.    Both DTC techniques can be tried.

#define XSP3_CC_GOOD_GRADE_MCA_ONLY			0		//!< Good grade masks MCA only. Scalers unchanged
#define XSP3_CC_GOOD_GRADE_MCA_WIN			1		//!< Good grade masks MCA and inWin scalers. Correct with increased dead time
#define XSP3_CC_GOOD_GRADE_ALL_EVENT		2		//!< Good grade masks MCA and Inwin 0, 1. AllEvent and all good become AllEvent and AllEvent_GoodGrade.
#define XSP3_CC_GOOD_GRADE_ALL_GOOD 		3		//!< Good grade masks MCA and Inwin 0, 1. AllEvent and all good become AllGood and AllGood_GoodGrade.
//! [XSP3_CC_REGISTER]

//! [XSP3_RESET_A_REGISTER]
#define XSP3_RESET_TIME(x)				(((x)&0x3ff)<<0)
#define XSP3_RESET_OUTPUT_RESET			(1<<10)
#define XSP3_RESET_OVER_RANGE_EXTEND	(1<<11)
#define XSP3_RESET_DELAY(x)				(((x)&0xf)<<12)
#define XSP3_RESET_THRES(x)				(((x)&0xFFFF)<<16)

#define XSP3_RESET_GET_TIME(x)			(((x)>>0)&0x3ff)
#define XSP3_RESET_GET_DELAY(x)			(((x)>>12)&0xf)
#define XSP3_RESET_GET_THRES(x)			(((x)>>16)&0xFFFF)
//! [XSP3_RESET_A_REGISTER]

#define XSP3_RESET_MAX_RESET_TIME 0x3FF
#define XSP3_RESET_MAX_RESET_THRES 0xFFFF

#define XSP3_RESET_MAX_DIFF_DEL 0xF
#define XSP3_RESET_MAX_DATA_DEL 0x3F
#define XSP3_RESET_MIN_DIFF_THRES (-2048*16)
#define XSP3_RESET_MAX_RAW_TIME 0x3F

//! [XSP3_RESET_B_REGISTER]
#define XSP3_RESET_B_DIFF_DEL(x)		(((x)&0xF)<<0)	
#define XSP3_RESET_B_DATA_DEL(x)		(((x)&0x3F)<<4)
#define XSP3_RESET_B_AVE_CONT(x)		(((x)&0x3)<<10)
#define XSP3_RESET_B_DIFF_THRES(x)		((((x)>>4)&0x7FF)<<12)
#define XSP3_RESET_B_RAW_TIME(x)		(((x)&0x3F)<<23)
#define XSP3_RESET_B_SET_MODE(x)		(((x)&0x3)<<30)

//! [XSP3_RESET_B_REGISTER]

#define XSP3_RESET_B_MODE_OFF			0
#define XSP3_RESET_B_MODE_FROM_START	1
#define XSP3_RESET_B_MODE_END_GRAD		2
#define XSP3_RESET_B_MODE_END_LEVEL		3

#define XSP3_RESET_B_SUM1		0
#define XSP3_RESET_B_SUM2		1
#define XSP3_RESET_B_SUM4		2

//! [XSP3_RESET_C_REGISTER]
#define XSP3_RESET_C_THRES_FALL(x)		(((x)&0xFFFF)<<16)
#define XSP3_RESET_C_THRES_RISE(x)		(((x)&0xFFFF)<<0)
#define XSP3_RESET_C_GET_THRES_FALL(x)		(((x)>>16)&0xFFFF)
#define XSP3_RESET_C_GET_THRES_RISE(x)		((x)&0xFFFF)
#define XSP3_RESET_C_THRES_FALL_V2(x)		(((x)&0xFFF0)<<16)
#define XSP3_RESET_C_LONG_HOLD_OFF_V2(x)	(((x)&0xF)<<16)

#define XSP3_RESET_MAX_LONG_HOLDOFF 15
//! [XSP3_RESET_C_REGISTER]

//! [XSP3_GLITCH_A_REGISTER]
#define XSP3_GLITCH_A_ENABLE					(1<<0)
#define XSP3_GLITCH_A_GLITCH_TIME(x)			(((x)&0x1ff)<<1)
#define XSP3_GLITCH_A_GLITCH_THRES8(x)			(((x)&0xff)<<10)
#define XSP3_GLITCH_A_GLITCH_THRES10(x)			(((x)&0x3ff)<<10)
#define XSP3_GLITCH_A_DIFF_TIME(x)				(((x)&0x1)<<18)	// Not used with 10 bit threshold versions
//#define XSP3_GLITCH_A_PAD_MODE(x)				(((x)&0x1)<<19)  // Appears not to be supported in HW
#define XSP3_GLITCH_A_PRE_TIME(x)				(((x)&0x1f)<<20)
#define XSP3_GLITCH_A_FROM_GLOBAL_RESET_LEVEL	(1<<25)
#define XSP3_GLITCH_A_FROM_GLOBAL_RESET_EDGE	(1<<26)
#define XSP3_GLITCH_A_RETRIGGER					(1<<27)
#define XSP3_GLITCH_A_MASK_GLITCH_EVENT			(1<<28)
#define XSP3_GLITCH_A_COUNT_GLICTH_TIME			(1<<29)
#define XSP3_GLITCH_A_EVDET(x)					(((x)&0x3)<<30)		//!< From XSPRESS2 trigger-A (2 gradients over threshold). No longer used for this, now in XSP3_GLITCH_A_PRE_TIME_LONG.
#define XSP3_GLITCH_A_PRE_TIME_LONG(x)			(((x)&0x1f)<<20|((x)&0x60)<<25)	//!< Pusedo Log coded Pre time. 0...63 => 0..63 , 64..127 => 0, 8, 16..504.

//! [XSP3_GLITCH_A_REGISTER]

#define XSP3_GLITCH_A_GET_GLITCH_TIME(x)  (((x)>>1)&0x1ff)
#define XSP3_GLITCH_A_GET_GLITCH_THRES(x) (((x)>>10)&0xff)

#define XSP3_GLITCH_A_MAX_GLITCH_TIME 0x1FF
/* Max glitch time was 0x3F in early releases, but discouraging such operation and not supported in newer firmware */
#define XSP3_GLITCH_A_MAX_PRE_TIME    		0x1F		//!< Maximum pre-time with normal Full Gdet build
#define XSP3_GLITCH_A_MAX_PRE_TIME_LONG   	0x1FF		//!< Maximum pre-time with BRAM based dealy GDet, see {@link XSP3_FEATURE_GDET_LONG}.

#define XSP3_GLITCH_A_MIN_THRES       -255

//! [XSP3_GLITCH_B_REGISTER]
#define XSP3_GLITCH_B_HOLDOFF(x)			((x)&0x1FF)
#define XSP3_GLITCH_B_SUMMODE(x)			(((x)&3)<<9) // XSP3_GLITCH_B_SUM1 | XSP3_GLITCH_B_SUM2 | XSP3_GLITCH_B_SUM4
#define XSP3_GLITCH_B_GET_SUMMODE(x)		(((x)>>9)&3)
#define XSP3_GLITCH_B_MASK_BOG_EVENT		(1<<11)
#define XSP3_GLITCH_B_GLITCH_STRETCH(x)		(((x)&0x3F)<<12)
#define XSP3_GLITCH_B_GET_GLITCH_STRETCH(x)	(((x)>>12)&0x3F)
#define XSP3_GLITCH_B_BGE_STRETCH(x)		(((x)&0x1F)<<18)
#define XSP3_GLITCH_B_GET_BGE_STRETCH(x)	(((x)>>18)&0x1F))
#define XSP3_GLITCH_B_DIFF_SEP(x)			(((x)&0xF)<<24)
#define XSP3_GLITCH_B_GET_DIFF_SEP(x)		(((x)>>24)&0xF)
#define XSP3_GLITCH_B_FORCE_EVENT			(1<<23)
#define XSP3_GLITCH_B_BGE_EXTEND			(1<<28)
#define XSP3_GLITCH_B_SHORT_PADDING			(1<<29)
#define XSP3_GLITCH_B_BGE_LOOKAHEAD			(1<<30)
//! [XSP3_GLITCH_B_REGISTER]

#define XSP3_GLITCH_B_SUM1		0
#define XSP3_GLITCH_B_SUM2		1
#define XSP3_GLITCH_B_SUM4		2

#define XSP3_GLITCH_B_MAX_HOLDOFF 0x1FF
#define XSP3_GLITCH_B_MAX_STRETCH 0x3f
#define XSP3_GLITCH_B_MAX_BGE_STRETCH 0x1f
#define XSP3_GLITCH_B_MAX_DIFF_SEP    15

//! [XSP3_RST_XTK_REGISTER]
#define XSP3_RST_XTK_SET_CHAN_MASK(x)    	((x)&0xFFFF)
#define XSP3_RST_XTK_ENABLE					(1<<16)
#define XSP3_RST_XTK_EDGE_MODE				(1<<17)
#define XSP3_RST_XTK_SET_STRETCH_TIME(x)	(((x)&0x1FF)<<18)

#define XSP3_RST_XTK_GET_CHAN_MASK(x)    	((x)&0xFFFF)
#define XSP3_RST_XTK_GET_STRETCH_TIME(x)	(((x)>>18)&0x1FF)
#define XSP3_RST_XTK_MAX_STRETCH_TIME       0x1FF

//! [XSP3_RST_XTK_REGISTER]

//! [XSP3_XTK_CORR_REGISTERS]
#define XSP3_XTKA_CORR_ENB_SUBTRACT			1
#define XSP3_XTKA_CORR_SHAPE_LEN(x)			(((x)&0x7F)<<1)
#define XSP3_XTKA_CORR_PRE_SAMPLES(x)		(((x)&0x3F)<<8)
#define XSP3_XTKA_CORR_MIN_AGG(x)			(((x)&0x3FF)<<14)
#define XSP3_XTKA_CORR_MAX_NOISE(x)			(((x)&0xFF)<<24)

#define XSP3_XTKB_CORR_ENB_DELETE(x)		(((x)&7)<<0)
#define XSP3_XTKB_CORR_ENB_SERVO_DELETE_TRIG_B	(1<<3)
#define XSP3_XTKB_CORR_ENB_SERVO_DELETE(x)	(((x)&7)<<4)
#define XSP3_XTKB_CORR_SERVO_MAX_NOISE(x)	(((x)&0xFF)<<8)
#define XSP3_XTKB_CORR_DEL_MIN_AGG(x)		(((x)&0x3FF)<<16)

#define XSP3_XTKB_CORR_SERVO_PRE_TIME(x)	(((x)&0x1F)<<26)

#define XSP3_XTKB_CORR_DISABLE_SPLIT		(1<<31)
#define XSP3_XTKB_CORR_MAX_SERVO_PRE_TIME 	31

#define XSP3_XTKC_FIFO1_RESET_BAD		(1<<0)
#define XSP3_XTKC_FIFO2_RESET_BAD		(1<<1)
#define XSP3_XTKC_SUB_DISCARD_BAD		(1<<2)
#define XSP3_XTKC_SUB_SKIP				(1<<3)
#define XSP3_XTKC_MID_DISCARD_BAD		(1<<4)
#define XSP3_XTKC_NOISE_DISCARD_BAD		(1<<5)
#define XSP3_XTKC_CHECK_ENG_TOP			(1<<6)
#define XSP3_XTKC_ORDER_CHAN_OUT		(1<<7)
#define XSP3_XTKC_MID_DISCARD_LATE		(1<<8)
#define XSP3_XTKC_SUB_DISCARD_LATE		(1<<9)
#define XSP3_XTKC_NOISE_DEL_CORRECTED	(1<<10)

#define XSP3_XTKC_DEFAULT_DISCARD       (XSP3_XTKC_SUB_DISCARD_BAD | XSP3_XTKC_SUB_SKIP | XSP3_XTKC_MID_DISCARD_BAD	 | XSP3_XTKC_NOISE_DISCARD_BAD | XSP3_XTKC_CHECK_ENG_TOP | XSP3_XTKC_ORDER_CHAN_OUT | XSP3_XTKC_MID_DISCARD_LATE | XSP3_XTKC_SUB_DISCARD_LATE)

#define XSP3_XTKC_SERVO_MASK_STRETCH(x)	(((x)&0x3F)<<11)
#define XSP3_XTKC_GET_SERVO_MASK_STRETCH(x)	(((x)>>1)&0x3F)
#define XSP3_XTKC_MAX_SERVO_MASK_STRETCH 0x3F

#define XSP3_XTK_DELETE_OFF				0
#define XSP3_XTK_DELETE_NO_COINC		1
#define XSP3_XTK_DELETE_COINC_WID4OFF2	2
#define XSP3_XTK_DELETE_COINC_WID6OFF2	3
#define XSP3_XTK_DELETE_COINC_WID8OFF2	4
#define XSP3_XTK_DELETE_COINC_MAX		4

#define XSP3_XTK_CORR_MAX_MIN_AGG		0x3FF
#define XSP3_XTK_CORR_MAX_DEL_MIN_AGG	0x3FF
#define XSP3_XTK_CORR_MAX_MAX_NOISE		0xFF
//! [XSP3_XTK_CORR_REGISTERS]

//! [XSP3_TRIGGERB_THRESHOLD_REGISTER]
#define X3TRIG_B_THRES_SET_ARM(x)		(((x)&0x3FF))
#define X3TRIG_B_THRES_SET_END(x)		(((x)&0x3FF)<<10)
#define X3TRIG_B_THRES_SET_REARM(x)		(((x)&0x3FF)<<20)
#define X3TRIG_B_THRES_TWO_OVER			(1<<30)
#define X3TRIG_B_THRES_ENABLE_CFD		(1<<31)
#define X3TRIG_B_THRES_GET_ARM(x)		(((x)&0x3FF))
#define X3TRIG_B_THRES_GET_END(x)		(((x)>>10)&0x3FF)
#define X3TRIG_B_THRES_GET_REARM(x)		(((x)>>20)&0x3FF)
//! [XSP3_TRIGGERB_THRESHOLD_REGISTER]

#define X3TRIG_B_ENABLE_CFD     1		/* used as argument to setup-trigger_b function */
#define X3TRIG_B_ENABLE_OVERTHR 2		/* used as argument to setup-trigger_b function */

//! [XSP3_TRIGGERB_TIMEA_REGISTER]
#define X3TRIG_B_TIME_SET_DIFF1(x)		(((x)&0x7f))		// First differential delay. Value 0 creates delay of 3
#define X3TRIG_B_TIME_SET_DIFF2(x)		(((x)&0x7f)<<7) 	// Second differential delay. Value 0 creates delay of 3
#define X3TRIG_B_TIME_SET_DATA(x)		(((x)&0x7f)<<14)	// Data delay. Value 0 creates delay of 3
#define X3TRIG_B_TIME_SET_EVENT(x)		(((x)&0x7f)<<21)	// Event width. Value 1 creates width of 1
#define X3TRIG_B_TIME_SET_AVEMODE(x)	(((x)&3)<<28)		// Average mode
#define X3TRIG_B_TIME_DISABLE_SPLIT		(1<<30)
#define X3TRIG_B_TIME_ENABLE_OVERTHR	(1<<31)

#define X3TRIG_B_TIME_GET_DIFF1(x)		(((x)&0x7f))		/* First differential delay. Value 0 creates delay of 3 */
#define X3TRIG_B_TIME_GET_DIFF2(x)		(((x)>>7)&0x7f) 	/* Second differential delay. Value 0 creates delay of 3 */
#define X3TRIG_B_TIME_GET_DATA(x)		(((x)>>14)&0x7f)	/* Data delay. Value 0 creates delay of 3 */
#define X3TRIG_B_TIME_GET_EVENT(x)		(((x)>>21)&0x7f)	/* Event width. Value 1 creates width of 1 */
#define X3TRIG_B_TIME_GET_AVEMODE(x)	(((x)>>28)&3)		/* Average mode */

#define X3TRIG_B_TIME_DIFF_OFFSET_BRAM 	3					// Actual delay for value 0 when using BRAM delay
#define X3TRIG_B_TIME_DIFF_OFFSET_SRL 	1					// Actual delay for value 0 when using SRL32 delay
#define X3TRIG_B_TIME_DIFF_MAX_BRAM 	0x7F				// Maximum delay value when using BRAM Delay
#define X3TRIG_B_TIME_DIFF_MAX_SRL 		0x1F				// Maximum delay value when using SRL32 Delay
#define X3TRIG_B_TIME_DATA_DELAY_OFFSET	3					// Offset to apply to Data delay (using BRAM delay)
#define X3TRIG_B_MAX_DATA_DEL		0x7F

//! [XSP3_TRIGGERB_TIMEA_REGISTER]

//! [XSP3_TRIGGERB_OVER_THRESHOLD_REGISTER]
#define X3TRIG_B_SET_OVERTHR_DELAY(x)	(((x)&0x1F))
#define X3TRIG_B_SET_OVERTHR_STRETCH(x)	(((x)&0x3F)<<8)
#define X3TRIG_B_SET_OVERTHR_TRIM(x)	(((x)&0x1F)<<16)
#define X3TRIG_B_GET_OVERTHR_DELAY(x)	(((x)&0x1F))
#define X3TRIG_B_GET_OVERTHR_STRETCH(x)	(((x)>>8)&0x3F)
#define X3TRIG_B_GET_OVERTHR_TRIM(x)	(((x)>>16)&0x1F)
//! [XSP3_TRIGGERB_OVER_THRESHOLD_REGISTER]

//! [XSP3_TRIGGERB_TIMEB_REGISTER]
#define X3TRIG_B_TIMEB_ENB_SPLIT_OTD		(1<<29)
#define X3TRIG_B_TIMEB_SET_THRES_SCALE(x)	(((x)&1)<<30)
#define X3TRIG_B_TIMEB_SCALED_THRES_MODE	(1<<31)
#define X3TRIG_B_TIMEB_GET_THRES_SCALE(x) (((x)>>30)&1)
//! [XSP3_TRIGGERB_TIMEB_REGISTER]

#define X3TRIB_FAST_SET_AVEMODE(x)			(((x)&3)<<0)
#define X3TRIB_FAST_SET_DIFF1_DELAY(x)		(((x)&0x1F)<<2)
#define X3TRIB_FAST_SET_ALIGN_DELAY(x)		(((x)&0xF)<<7)
#define X3TRIB_FAST_SET_ARM_THRES(x)		(((x)&0x3ff)<<11)	
#define X3TRIB_FAST_SET_END_THRES(x)		(((x)&0x3ff)<<21)
#define X3TRIB_FAST_SET_ENABLE				(1<<31)

#define X3TRIB_FAST_GET_AVEMODE(x)			(((x)>>0)&3)
#define X3TRIB_FAST_GET_DIFF1_DELAY(x)		(((x)>>2)&0x1F)
#define X3TRIB_FAST_GET_ALIGN_DELAY(x)		(((x)>>7)&0xF)
#define X3TRIB_FAST_GET_ARM_THRES(x)		(((x)>>11)&0x3ff)	
#define X3TRIB_FAST_GET_END_THRES(x)		(((x)>>21)&0x3ff)

#define X3TRIG_B_FAST_MAX_DIFF_DELAY			0x1F
#define X3TRIG_B_FAST_MAX_ALIGN_DELAY			0xF

#define X3TRIB_TIMEC_ENB_VARIABLE_WIDTH		(1<<0)
#define X3TRIB_TIMEC_SET_VARIABLE_WIDTH(x)	(((x)&0x1f)<<1)
#define X3TRIB_TIMEC_GET_VARIABLE_WIDTH(x)	(((x)>>1)&0x1f)
#define X3TRIB_TIMEC_ENB_SPLIT_TOUCHING		(1<<6)
#define X3TRIB_TIMEC_ENB_FAST_DIFF2			(1<<7)
#define X3TRIG_B_TIMEC_MAX_VARIABLE_DELAY	0x1F

//! [XSP3_TRIGGERBC_OVER_THRESHOLD_SERVO_REGISTER]
#define X3TRIG_BC_SET_OVERTHR_SERVO_DELAY(x)    (((x)&0x1F))
#define X3TRIG_BC_SET_OVERTHR_SERVO_STRETCH(x)  (((x)&0x3F)<<8)
#define X3TRIG_BC_GET_OVERTHR_SERVO_DELAY(x)    (((x)&0x1F))
#define X3TRIG_BC_GET_OVERTHR_SERVO_STRETCH(x)  (((x)>>8)&0x3F)

#define X3TRIG_BC_OVERTHR_SERVO_USE_DET_RESET  	(1<<31)			// Use DeResetTrimmed to disable Over threshold for servo trigger during detector reset (usually Unnecessary)
#define X3TRIG_BC_OVERTHR_USE_DET_RESET  		(1<<30)			// Use DeResetTrimmed to disable Over threshold trigger during detector reset (usually Unnecessary)
//! [XSP3_TRIGGERBC_OVER_THRESHOLD_SERVO_REGISTER]

//! [XSP3_TRIGGERC_OVER_THRESHOLD_SERVO_REGISTER]
#define X3TRIG_C_SET_OVERTHR_SERVO_TRIM(x)     	(((x)&0xF)<<16)
#define X3TRIG_C_GET_OVERTHR_SERVO_TRIM(x)     	(((x)>>16)&0xF)
#define X3TRIG_C_SET_OVERTHR_SERVO_MIN_WIDTH(x)	(((x)&0x7F)<<21)
#define X3TRIG_C_GET_OVERTHR_SERVO_MIN_WIDTH(x)	(((x)>>21)&0x7F)
#define X3TRIG_C_MAX_MIN_WIN					0x7F
//! [XSP3_TRIGGERC_OVER_THRESHOLD_SERVO_REGISTER]

//! [XSP3_TRIGGERC_THRESHOLD_REGISTER]
#define X3TRIG_C_THRES_SET_ARM(x)			(((x)&0x3FF))
#define X3TRIG_C_THRES_SET_END(x)			(((x)&0x3FF)<<10)
#define X3TRIG_C_THRES_SET_DIFF1(x)			(((x)&0x7F)<<20)
#define X3TRIG_C_THRES_SET_AVEMODE(x) 		(((x)&3)<<27)		/* Average mode */
#define X3TRIG_C_THRES_TWO_OVER				(1<<29)
#define X3TRIG_C_THRES_ENB_NEGATIVE			(1<<30)
#define X3TRIG_C_THRES_IGNORE_NEAR_GLITCH	(1<<31)

#define X3TRIG_C_THRES_GET_ARM(x)			(((x)&0x3FF))
#define X3TRIG_C_THRES_GET_END(x)			(((x)>>10)&0x3FF)
#define X3TRIG_C_THRES_GET_DIFF1(x)			(((x)>>20)&0x7F)
#define X3TRIG_C_THRES_GET_AVEMODE(x) 		(((x)>>27)&3)
//! [XSP3_TRIGGERC_THRESHOLD_REGISTER]

//! [XSP3_TRIGGER_B_RINGING_REGISTER]
#define X3TRIG_B_RINGING_SCALE_A(x)	(((x)&0x7FF))			// 11 bit Signed scaling where 1024 would be 0.5
#define X3TRIG_B_RINGING_DELAY_A(x)	(((x)&0x1F)<<11)		// Delay coded 0..31 gives a delay of 3..34
#define X3TRIG_B_RINGING_SCALE_B(x)	(((x)&0x7FF)<<16)		// Scaling for second correction term
#define X3TRIG_B_RINGING_DELAY_B(x)	(((x)&0x1F)<<27)		// Delay for second correction tap

#define X3TRIG_B_GET_RINGING_SCALE_A(x)	(((x)&0x7FF))			// 11 bit Signed scaling where 1024 would be 0.5
#define X3TRIG_B_GET_RINGING_DELAY_A(x)	(((x)>>1)&0x1F)		// Delay coded 0..31 gives a delay of 3..34
#define X3TRIG_B_GET_RINGING_SCALE_B(x)	(((x)>>16)&0x7FF)		// Scaling for second correction term
#define X3TRIG_B_GET_RINGING_DELAY_B(x)	(((x)>>27)&0x1F)		// Delay for second correction tap

#define XSP3_TRIGB_RING_MIN_DEL 3
//! [XSP3_TRIGGER_B_RINGING_REGISTER]

//! [XSP3_TRIGGERC_AVEMODE_BITS 27-28]
#define X3TRIG_C_AVE2				0
#define X3TRIG_C_AVE4				1
#define X3TRIG_C_AVE8				2
#define X3TRIG_C_AVE16				3
//! [XSP3_TRIGGERC_AVEMODE_BITS 27-28]

#define X3TRIG_B_SCALED_THRES_OFF 0
#define X3TRIG_B_SCALED_THRES_DIV4 0x8
#define X3TRIG_B_SCALED_THRES_DIV2 0x9

#define X3TRIG_B_AVE1				0
#define X3TRIG_B_AVE2				1
#define X3TRIG_B_AVE4				2
#define X3TRIG_B_AVE8				3

#define X3DEFAULT_TRIGB_DELAY       (40+16) /* Trig B delay to use when using trigger A only */
#define X3TRIG_B_MIN_DELAY			(18+8+16+1)
#define X3TRIG_B_MAX_TRIM			31
#define X3TRIG_B_MAX_OVERTHR_DELAY	31
#define X3TRIG_B_MAX_OVERTHR_STRETCH	0x3F
#define X3TRIG_B_MAX_THRES			0x3FF


#define X3TRIG_C_MAX_THRES			0x3FF
#define X3TRIG_C_MAX_SEP1			0x7F
#define X3TRIG_C_MAX_OVERTHR_DELAY	31
#define X3TRIG_C_MAX_OVERTHR_STRETCH	0x3F
#define X3TRIG_C_MAX_TRIM			15

#define X3TRIG_B_MIN_EVENT_TIME		0x2
#define X3TRIG_B_MAX_EVENT_TIME		0x7F

/**
@defgroup XSP3_CAL_EV	Setting for calibration event generator.
@ingroup XSP3_MACROS
@{
*/
#define XSP3_CAL_EV_ENABLE				1							//!< Enable Calibration events
#define XSP3_CAL_EV_PERIOD(x)			(((x)&0x3FFF)<<1)			//!< Set period of calibration events. In 100 ns Units
#define XSP3_CAL_EV_AVOID				(1<<31)						//!< Calibration event avoid clashing with real events.
#define XSP3_CAL_EV_MAX_PERIOD 			0x3FFF						//!< Maximum cal event period in 100 ns Units.
/**
@}
*/


/* XSPRESS3 In Window registers */
#define X3WINDOW_LOW_START 	    	 0
#define X3WINDOW_LOW_MASK 	0x0000ffff
#define X3WINDOW_HIGH_START 	    16 
#define X3WINDOW_HIGH_MASK 	0xffff0000
#define X3WINDOW_ALL_EVENT_MASK  0xf000f000

/* Interaction of scope mode with servo settling */
#define X3SCOPE_DELAY_TRIG      (1<<23)
#define X3SCOPE_RESTART_SERVO   (1<<24)

#define XSP3_SERVO_C_ENABLE			1

//! [XSP3_SERVO_A_REGISTER]
#define XSP3_SERVO_A_DIV(x)				((x)&7)
#define XSP3_SERVO_A_STRETCH(x)			(((x)&0x7F)<<3)
#define XSP3_SERVO_A_PWL_TIME(x)		(((x)&3)<<10)
#define XSP3_SERVO_A_PRE_GAP(x)			(((x)&0xF)<<12)
#define XSP3_SERVO_A_GRAD_ERR_LIM(x)	(((x)&0xFF)<<16)
#define XSP3_SERVO_A_MASK_GLOBAL_GLITCH	(1<<24)
#define XSP3_SERVO_A_ACCUM_ON_UPDATE	(1<<25)
#define XSP3_SERVO_A_CLEAR_ON_UPDATE	(1<<26)
#define XSP3_SERVO_A_CAL_EXCLUDE(x)  	(((x)&3)<<27)
#define XSP3_SERVO_A_FROM_CAL			(1<<29)
#define XSP3_SERVO_A_MASK_GLITCH     	(1<<30)
#define XSP3_SERVO_A_PWL_ENB         	(1<<31)

#define XSP3_SERVO_A_GET_STRETCH(x)		(((x)>>3)&0x7F)
#define XSP3_SERVO_A_GET_PWL_TIME(x)	(((x)>>10)&3)
#define XSP3_SERVO_A_GET_PRE_GAP(x)		(((x)>>12)&0xF)
//! [XSP3_SERVO_A_REGISTER]

//! [XSP3_SERVO_B_REGISTER]
#define XSP3_SERVO_B_GRAD_EST_LIM(x)	(((x)&0xFFFF)<<0)
#define XSP3_SERVO_B_ENB_GLOB_GRAD		(1<<16)
#define XSP3_SERVO_B_GLOB_ADJ_SIZE(x)	(((x)&0x3)<<17)
#define XSP3_SERVO_B_GLOB_ADJ_POSN(x)	(((x)&0x7F)<<19)
#define XSP3_SERVO_B_GLOB_ADJ_SHIFT(x)	(((x)&0x7)<<26)
#define XSP3_SERVO_B_STRETCH_TIME		(1<<29)
#define XSP3_SERVO_B_GLITCH_AS_RESET	(1<<30)
#define XSP3_SERVO_B_USE_ADC_TOP4		(1<<31)
//! [XSP3_SERVO_B_REGISTER]

//! [XSP3_SERVO_C_REGISTER]
#define XSP3_SERVO_C_ENB_SERVO			(1<<0)
#define XSP3_SERVO_C_RESTART_ON_RUN		(1<<1)
#define XSP3_SERVO_C_USE_OTD_SERVO		(1<<2)
#define XSP3_SERVO_C_SET_RESET_STRETCH(x) (((x)&0x1FF)<<3)
#define XSP3_SERVO_C_SET_DITHER_CONT(x) (((x)&0x7)<<12)
#define XSP3_SERVO_C_GET_RESET_STRETCH(x) (((x)>>3)&0x1FF)
#define XSP3_SERVO_C_GET_DITHER_CONT(x) (((x)>>12)&0x7)
#define XSP3_SERVO_C_MAX_RESET_STRETCH	0x1FF
#define XSP3_SERVO_C_MAX_DITHER_CONT	5

#define XSP3_SERVO_C_ENB_BI_LIN_TIME		(1<<15)			//!< Enable detailed control of Bi-linear timing.
#define XSP3_SERVO_C_BI_LIN_TIME_CODE(x)	(((x)&3)<<16)	//!< Coded length for longer chunks 0=> 128, 1=>256, 2=>512, 3=>1024
#define XSP3_SERVO_C_BI_LIN_TIME_START(x)	(((x)&7)<<18)	//!< Coded number of short chunks before jumping to longer chunks. 0=>4, 1=>8, 2=>16, 3=>32, 4=>64, 5=>128, 6=>256, 7=>256

#define XSP3_SERVO_C_RESET_PRE_TIME(x)		(((x)&0x1F)<<21)	//!< Start Masking error accummulation some time before Reset is asserted.
#define XSP3_SERVO_C_GR_PRE_TIME(code)		(((code)&7)<<26)	//!< Top 3 bits of GR pretime
#define XSP3_SERVO_C_GR_STRETCH(code)		(((code)&7)<<29)	//!< Top 3 bits of GR Stretch
#define XSP3_SERVO_MAX_RESET_STRETCH 0x1FF			//!< Maximum stretch time for DetReset
#define XSP3_SERVO_MAX_RESET_PRE_TIME 0x1F			//!< Maximum Pre time for DetReset.

//! [XSP3_SERVO_C_REGISTER]

#define XSP3_SERVO_MAX_ERR_LIM		0xFF
#define XSP3_SERVO_MAX_STRETCH		0x7F

#define XSP3_SERVO_MAX_GR_PRE		0x1F
#define XSP3_SERVO_MAX_GR_STRETCH  	0x3F

#define XSP3_CHAN_GLOB 15

//! [XSP3_CSHR_CONT_REGISER]

#define XSP3_CSHR_CONT_ENB					(1<<0)
#define XSP3_CSHR_CONT_SET_COINC(x)			(((x)&3)<<1)
#define XSP3_CSHR_CONT_GET_COINC(x)			(((x)>>1)&3)
#define XSP3_CSHR_CONT_ENB_NEB_TRIG			(1<<8)
#define XSP3_CSHR_CONT_ENB_MASK_LARGE		(1<<9)
#define XSP3_CSHR_CONT_NEB_TRIG_DISCARD_BAD	(1<<10)

#define XSP3_CSHR_COINC_ANY		0		//!< Use all neighbour charge measurement, even if they overlap an event.
#define XSP3_CSHR_COINC_PM2		1		//!< Use non-overlapping neighbour charge measurements and those that are coincident to triggers on the neighbour +/- 2 cycles, ignore others which overlap events
#define XSP3_CSHR_COINC_PM1		2		//!< Use non-overlapping neighbour charge measurements and those that are coincident to triggers on the neighbour +/- 1 cycles, ignore others which overlap events
#define XSP3_CSHR_COINC_COINC	3		//!< Use non-overlapping neighbour charge measurements and those that are coincident to triggers on the neighbour to the cycle, ignore others which overlap events

#define XSP3_CSHR_CONT_DEFAULT				(XSP3_CSHR_CONT_NEB_TRIG_DISCARD_BAD)
#define XSP3_CSHR_CONT_DISCARD_ALL			(XSP3_CSHR_CONT_NEB_TRIG_DISCARD_BAD)


//! [XSP3_CSHR_CONT_REGISER]

//! [XSP3_CSHR_MAPPING_REGISER]

#define XSP3_CSHR_MAP_CHAN_OF_CARD(ch,neb)		(((u_int64_t)(ch)&0xF)<<(6*neb))
#define XSP3_CSHR_MAP_REL_BOARD(brd,neb)		(((u_int64_t)(brd)&0x3)<<(4+6*neb))

#define XSP3_CSHR_MAP_GET_CHAN_OF_CARD(x,neb)	(((x)>>(6*neb))&0xF)
#define XSP3_CSHR_MAP_GET_REL_BOARD(x,neb)		(((x)>>(4+6*neb))&0x3)

//! [XSP3_CSHR_MAPPING_REGISER]

//! [XSP3_REGIONS]
#define XSP3_REGION_RAM_TESTPAT		1
#define XSP3_REGION_RAM_RESET_TAIL	2
#define XSP3_REGION_RAM_QUOTIENT	3
#define XSP3_REGION_RAM_ROI			4
#define XSP3_REGION_RAM_EVENT_TAIL	5
#define XSP3_REGION_RAM_PWL_SERVO	6
#define XSP3_REGION_RAM_PILEUP_TIME	7
#define XSP3_REGION_RAM_AUX1		8
#define XSP3_REGION_RAM_SERVO_TAIL	9
#define XSP3_REGION_RAM_EVENT_LEAD	10
#define XSP3_REGION_RAM_WIDTH_TIME	11
#define XSP3_REGION_RAM_XTK_MAP  	12
#define XSP3_REGION_RAM_XTK_SHAPE 	13
#define XSP3_REGION_GLOB_REG		15
//! [XSP3_REGIONS]

#define XSP3_REGION_RAM_MAX		13
#define XSP3_REGION_REGS		0

#define XSP3_REGS_SIZE			32
#define XSP3_CHAN_TP_SIZE		0
#define XSP3_RESET_TAIL_SIZE	1024
#define XSP3_QUOTIENT_SIZE		1024
#define XSP3_ROI_SIZE			4096
#define XSP3_EVENT_TAIL_SIZE	1024
#define XSP3_PWL_SERVO_SIZE		512
#define XSP3_PILEUP_TIME_SIZE	2048
#define XSP3_AUX1_SIZE			4096
#define XSP3_SERVO_TAIL_SIZE	1024
#define XSP3_EVENT_LEAD_SIZE	1024
#define XSP3_WIDTH_TIME_SIZE	256
#define XSP3_XTK_MAP_SIZE	    256
#define XSP3_XTK_SHAPE_TIME		128
#define XSP3_XTK_SHAPE_CHAN		16
#define XSP3_XTK_SHAPE_SIZE	    (XSP3_XTK_SHAPE_TIME*XSP3_XTK_SHAPE_CHAN)

#define XSP3_EVENT_TAIL_W4_NT	512
#define XSP3_EVENT_TAIL_W4_NW	16
#define XSP3_EVENT_LEAD_W4_NT	512
#define XSP3_EVENT_LEAD_W4_NW	16

#define XSP3_EVENT_LEAD_TAIL_MAX_NT	16

#define XSP3_NUM_FINE_TIME		16
#define XSP3_NUM_FT_WIDTH		16

/* Set to max BRAM size for use in save/restore */
#define XSP3_MAX_BRAM_SIZE 		8192

#define XSP3_FEV_TP_RESET		0x8000

#define XSP3_PWL_SERVO_SIZE16 2048

#define XSP3_XTK_MAP_STRETCH_WIDTH	(1<<31)				//!< Event width is stretched by specified time, rather than a fixed width pulse.

#define XSP3_XTK_MAP_CSHR_ENB		(1<<30)				//!< Enable making a trigger to measure charse shared signal from neighbour
#define XSP3_XTK_MAP_CSHR_STRETCH_WIDTH	(1<<29)			//!< Event width for charge sharing measurement is stretched by specified time, rather than a fixed width pulse.
// #define XSP3_XTK_MAP_CSHR_NEB_NUM(x)(((x)&0x7)<<26)		//!< Neighbour number 0..3 for square pixels, 0..5 for hexagonal pixels.
#define XSP3_XTK_MAP_CSHR_WIDTH(x)	(((x)&0x3F)<<20)	//!< Event width or strtech time for charge sharing measurement

#define XSP3_XTK_MAP_MASK_WIDTH(x)	(((x)&0x3F)<<14)
#define XSP3_XTK_MAP_MASK_DELAY(x)	(((x)&0x3F)<<8)
#define XSP3_XTK_MAP_VALID_SUBTRACT	(1<<7)
#define XSP3_XTK_MAP_VALID_COIN		(1<<6)
#define XSP3_XTK_MAP_ENB_MASK_COND 	(1<<5)
#define XSP3_XTK_MAP_ENB_MASK_UNCOND (1<<4)
#define XSP3_XTK_MAP_GET_MAPPED(x)	((x)&0xF)

#define XSP3_XTK_MAP_MAX_DELAY 		0x3F
#define XSP3_XTK_MAP_MAX_WIDTH		0x3F
#define XSP3_XTK_MAP_TOTAL_B2B	 7
#define XSP3_XTK_MAP_OFFSET(b2b)	((1<<1) | (((b2b)&0x7) << 8))

/**
	@defgroup XSP3_GLOBAL_REGS_OFFSETS   Address offsets of global registers
	@ingroup XSP3_MACROS
	@{
*/
#define XSP3_GLOB_CLOCK_CONT		0
#define XSP3_GLOB_MDIO_WRITE		1

#define XSP3_GLOB_TIMING_A			2
#define XSP3_GLOB_TIMING_FIXED		3

#define XSP3_GLOB_SCOPE_CHAN_SEL	4
#define XSP3_GLOB_SCOPE_SRC_SEL		5
#define XSP3_GLOB_SCOPE_NWORDS		6
#define XSP3_GLOB_SCOPE_ALTERNATE	7

#define XSP3_GLOB_DATA_PATH_CONT	8
// Register 9 is available now.
#define XSP3_GLOB_DATA_MUX_CONT		10
#define XSP3_NUM_GLOB_REG			15

// The Internal TFG setup is something the use should set, so WAS not included in the global save/restore controlled by XSP3_NUM_GLOB_REG
// But the Global reset generator must be.
#define XSP3_GLOB_ITFG_FRAME_LEN	11
#define XSP3_GLOB_ITFG_NUM_FRAMES	12

#define XSP3_GLOB_GLOB_RST_GEN_A	13
#define XSP3_GLOB_GLOB_RST_GEN_B	14

#define XSP3_GLOB_ITFG_MARKER		15	// Not included in save restroe of global registers.

#define XSP3_GLOB_FAN_SPEED 		31

#define XSP3_GLOB_MDIO_READ			33

#define XSP3_GLOB_LL_STATUS			32
#define XSP3_GLOB_TIMING_STATUS_A	34
#define XSP3_GLOB_FEATURES_A		37
#define XSP3_FEATURES_NUM			3
/**
 @}
 */

//! [XSP3_GLOBAL_CLOCK]
#define XSP3_GLOB_CLK_FROM_ADC			(1<<0)			 // Enable Clock from Spartans (from ADC) for Virtex 5 (0 at reset, write 1 for normal operation after setting up ADC board Clocks).
#define XSP3_GLOB_CLK_SP_RESET			(1<<1)			 // Software reset of Spartan DCMs after setting up ADC board Clocks.
#define XSP3_GLOB_CLK_IDC_TIMING_DRIVER	(1<<2)			 // 
#define XSP4_GLOB_CLK_MMCM_RESET		(1<<3)			// Software Reset ofthe MMCM in XSPRESS4, needed only with DRP actions?

#define XSP4_GLOB_CLK_TSYNC_MODE(x)	(((x)&3)<<6)	// Time Stamp synchronisation mode.
#define XSP4_TSYNC_MODE_ASYNC			0			// Time Stamp is reset by run on each card, so NOT locked.
#define XSP4_TSYNC_MODE_EXT_SYNC		1			// Time Stamp is reset by free running time stamp sync input
#define XSP4_TSYNC_MODE_EXT_SYNC_RUN	2			// Time Stamp is reset by free time stamp sync input, which is reset by run on card 0.



#define XSP3_GLOB_CLK_TP_ENB_SP_TOP	(1<<8)			 // Enable Test pattern from Spartans TOP.
#define XSP3_GLOB_CLK_USER_TSYNC_MODE(x)	(((x)&7)<<9)	// User Time Stamp synchronisation mode.

#define XSP3_USER_TSYNC_FRAMING 		0			//!< User time stamp Sync in run mode uses framing input to reset user (output) time stamp.
#define XSP3_USER_TSYNC_STANDALONE 		1			//!< User time stamp Sync in run mode uses repeating sync signal within each box to reset user (output) time stamp.
#define XSP3_USER_TSYNC_TEST_GLOB_RST 	4			//!< User time stamp Sync in test mode reuses Global reset extracted from playback data
#define XSP3_USER_TSYNC_TEST_GLOB_RST0 	5			//!< User time stamp Sync in test mode reuses Global reset extracted from playback data of channel 0 for all channels
#define XSP3_USER_TSYNC_TEST_STANDALONE 6			//!< User time stamp Sync in test mode uses repeating sync signal within each box to reset user (output) time stamp.

#define XSP3_GLOB_CLK_RS232_SEL		(1<<31) 		 // Select for RS232 control (temporary).
//! [XSP3_GLOBAL_CLOCK]

//! [XSP3_GLOBAL_TIMEA_SOURCE]
#define XSP3_GTIMA_SRC_FIXED			0		//!< Fixed constant time frame now replaced by Software timed but incremented time frame.
#define XSP3_GTIMA_SRC_SOFTWARE			0		//!< Timing controlled by software, incrementing on each subsequent continue.
#define XSP3_GTIMA_SRC_INTERNAL			1		//!< Time frame from internal timing generator (for future expansion).
#define XSP3_GTIMA_SRC_IDC				3		//!< Time frame incremented and rest by signals from IDC expansion connector.
#define XSP3_GTIMA_SRC_TTL_VETO_ONLY	4		//!< Time frame incremented by TTL Input 1.
#define XSP3_GTIMA_SRC_TTL_BOTH			5		//!< Time frame incremented by TTL Input 1 and reset to Fixed register by TTL Input 0.
#define XSP3_GTIMA_SRC_LVDS_VETO_ONLY	6		//!< Time frame incremented by LVDS Input.
#define XSP3_GTIMA_SRC_LVDS_BOTH		7		//!< Time frame incremented and reset by LVDS Inputs.
//! [XSP3_GLOBAL_TIMEA_SOURCE]

//! [XSP3_GLOBAL_TIMEA_REGISTER]

#define XSP3_GLOB_TIMA_TF_SRC(x)		((x)&7)		//!< Sets Time frame info source see XSP3_GTIMA_SRC_*.
#define XSP3_GLOB_TIMA_F0_INV			(1<<3)		//!< Invert Frame Zero signal polarity to make signal active low, resets time frame when sampled low by leading edge of Veto.
#define XSP3_GLOB_TIMA_VETO_INV			(1<<4)		//!< Invert Veto signal polarity to make signal active low, counts when Veto input is low.
#define XSP3_GLOB_TIMA_ENB_SCALER		(1<<5)		//!< Enables scalers.
#define XSP3_GLOB_TIMA_ENB_HIST			(1<<6)		//!< Enables histogramming.
#define XSP3_GLOB_TIMA_LOOP_IO			(1<<7)		//!< Loop TTL_IN(0..3) to TTL_OUT(0..3) for hardware testing (only).
#define XSP3_GLOB_TIMA_NUM_SCAL_CHAN(x) (((x)&0xF)<<8)	//!< Sets the number of channels of scalers to be transfered to memory by the DMA per time frame.
#define XSP3_GLOB_TIMA_SW_MARKERS(x)	(((x)&3)<<12)	//!< Set Software markers for use (testing) in circular buffer mode.
#define XSP3_GLOB_TIMA_FRAME_CAPTURE	(1<<14)			//!< Enable Frame Capture mode in circular buffer mode.
#define XSP3_GLOB_TIMA_FROM_RADIAL		(1<<15)			//!< Enable from Radial trigger signals for XSPRESS4 with backplane builds.
#define XSP3_GLOB_TIMA_DEBOUNCE(x)		(((x)&0xFF)<<16)	//!< Set debounce time in 80 MHz cycles to ignore glitches or ringing on Frame0 or Framign signal from any source.
#define XSP3_GLOB_TIMA_ALT_TTL(x)		(((x)&0xF)<<24)		//!< Alternate uses of the TTL Outputs (including channel in windows signals etc).
#define XSP3_GLOB_TIMA_RUN				(1<<31)		//!< Overall Run enable signal, set after all DMA channels have been configured.
#define XSP3_GLOB_TIMA_PB_RST			(1<<30)		//!< Resets Playback FIFO as part of clean start.
#define XSP3_GLOB_TIMA_COUNT_ENB		(1<<29)		//!< In software timing (XSP3_GTIMA_SRC_FIXED) mode enable counting when high. Transfers scalers on falling edge. After first frame, increments time frame on risign edge.
#define XSP3_GLOB_TIMA_ITFG_RUN			(1<<28)		//!< From versions 11/8/2014 onwards this is a separate Run signal to the internal time frame generator, which could be used to crash stop the ITFG before stopping the rest.

#define XSP3_GLOB_TIMA_GET_TF_SRC(x)	((x)&7)		//!< Sets Time frame info source see XSP3_GTIMA_SRC_*.
//! [XSP3_GLOBAL_TIMEA_REGISTER]

//! [XSP3_GLOBAL_TIMEB_REGISTER]


#define XSP3_GLOB_TIMB_FIXED(x)		((x)&0xFFFFFF)		//!< Sets Fixed (starting) Time frame
#define XSP3_GLOB_TIMB_ITFG_MARK(x)	(((x)&3)<<30)		//!< Set Marker mode when using Internal TFG
#define XSP3_GLOB_TIMB_GET_FIXED(x)		((x)&0xFFFFFF)	//!< Gets Fixed (starting) Time frame

//! [XSP3_GLOBAL_TIMEB_REGISTER]
#define XSP3_GTIMB_IMRK_TTL			0					//!< Use TTL Inputs TTL(0) => Capture, TTL(2)=>Marker(1), TTL(3)=>Marker(2)
#define XSP3_GTIMB_IMRK_SW			1					//!< Use Software Markers {@link XSP3_GLOB_TIMA_SW_MARKERS} SW Bit 0 => Capture, SW Bit 1 => Marker(1).
#define XSP3_GTIMB_IMRK_ITFG		2					//!< Use ITFG generated marked (when coded).
//! [XSP3_GLOBAL_TIMEA_ALT_TTL]
#define XSP3_ALT_TTL_TIMING_VETO			0		//!< Output the currently selected Count Enable Signal from Internal TFG or other inputs and replicate 4 times on TTL_OUT 0...3 Rev 1.22 onwards
#define XSP3_ALT_TTL_TIMING_ALL				1		//!< Output TTL_OUT(0)= Veto, (1)=Veto (2) = Running, (3) = Paused from Internal TFG (when present)
#define XSP3_ALT_TTL_TIMING_VETO_RUN		2		//!< Output TTL_OUT(0)= ITFGRun, (1)=Veto (2) = Veto, (3) = Veto
#define XSP3_ALT_TTL_TIMING_VETO_GR			4		//!< Output Global Reset on TTL_OUT(0) and the currently selected Count Enable Signal from Internal TFG or other inputs and replicate 3 times on TTL_OUT 1...3 Rev 1.26 onwards
#define XSP3_ALT_TTL_TIMING_ALL_GR			5		//!< Output Global Reset on TTL_OUT(0) and  TTL_OUT(1)=Veto, (2) = Running, (3) = Paused from Internal TFG (when present)
#define XSP3_ALT_TTL_INWINDOW				0x8		//!< Output in-window signal for channels 0:3.
#define XSP3_ALT_TTL_INWINLIVE				0x9		//!< Output 2 in-window signals and 2 live signals.
#define XSP3_ALT_TTL_INWINLIVETOGGLE		0xA		//!< Output 2 in-window signals and 2 live signals toggling.
#define XSP3_ALT_TTL_INWINGOODLIVE			0xB		//!< Outputs in-window Allevent AllGood and LiveLevel from Chan 0.
#define XSP3_ALT_TTL_INWINGOODLIVETOGGLE	0xC		//!< Outputs in-window Allevent AllGood and Live toggling from Chan 0.
//! [XSP3_GLOBAL_TIMEA_ALT_TTL]


#define XSP3_GSCOPE_CS_ENB_SCOPE	1
#define XSP3_GSCOPE_CS_BYTE_SWAP	(1<<1)
#define XSP3_GSCOPE_CS_DELAY_START	(1<<2)
#define XSP3_GSCOPE_CS_EXTRA_DELAY	(1<<3)

#define XSP3_GSCOPE_CHAN_SEL(s,x)	(((x)&0xF)<<4*((s)+1))
#define XSP3_GSCOPE_CHAN_SEL_GET(s,x) (((x)>>4*((s)+1))&0xF)

#define XSP3_GSCOPE_CHAN_SEL0(x)	(((x)&0xF)<<4)
#define XSP3_GSCOPE_CHAN_SEL1(x)	(((x)&0xF)<<8)
#define XSP3_GSCOPE_CHAN_SEL2(x)	(((x)&0xF)<<12)
#define XSP3_GSCOPE_CHAN_SEL3(x)	(((x)&0xF)<<16)
#define XSP3_GSCOPE_CHAN_SEL4(x)	(((x)&0xF)<<20)
#define XSP3_GSCOPE_CHAN_SEL5(x)	(((x)&0xF)<<24)

#define XSP3_GSCOPE_SRC_SEL(s,x)	(((x)&0xF)<<4*(s))
#define XSP3_GSCOPE_SRC_SEL_GET(s,x) (((x)>>4*((s)))&0xF)

#define XSP3_GSCOPE_SRC_SEL0(x)		(((x)&0xF)<<0)
#define XSP3_GSCOPE_SRC_SEL1(x)		(((x)&0xF)<<4)
#define XSP3_GSCOPE_SRC_SEL2(x)		(((x)&0xF)<<8)
#define XSP3_GSCOPE_SRC_SEL3(x)		(((x)&0xF)<<12)
#define XSP3_GSCOPE_SRC_SEL4(x)		(((x)&0xF)<<16)
#define XSP3_GSCOPE_SRC_SEL5(x)		(((x)&0xF)<<20)

#define XSP3_GSCOPE_ALT(s,x)		(((x)&0xF)<<4*(s))

#define XSP3_GSCOPE_ALT0(x)			(((x)&0xF)<<0)
#define XSP3_GSCOPE_ALT1(x)			(((x)&0xF)<<4)
#define XSP3_GSCOPE_ALT2(x)			(((x)&0xF)<<8)
#define XSP3_GSCOPE_ALT3(x)			(((x)&0xF)<<12)
#define XSP3_GSCOPE_ALT4(x)			(((x)&0xF)<<16)
#define XSP3_GSCOPE_ALT5(x)			(((x)&0xF)<<20)
#define XSP3_GSCOPE_ALTERNATE_GET(s,x) (((x)>>4*((s)))&0xF)

#define XSP3_SCOPE_NUM_STREAMS 6
//! [XSP3_SCOPE_SOURCES]
/* Stream 0 is usually used for the digital bits associated with the other 5 streams */
#define XSP3_SCOPE_SEL0_INP			0	// ADC Data
#define XSP3_SCOPE_SEL0_DIGITAL		1	// Digital data associated with analogue on other 5 streams
#define XSP3_SCOPE_SEL0_ALL_RESETS		2	// Digital data from All channel resets
#define XSP3_SCOPE_SEL0_RESET_DET_RESETS	3	// Digital data from All channel Reset Detector Det resets

/* Streams 1..3 are 4 input mux . Input, TriggerB and Servo are available in all 5 streams as these are used for the whole detector auto calibration */
/* Output is available in stream 1..3 for use to search for bad events when debugging 1 or 2 channels */
#define XSP3_SCOPE_SEL123_INP			0
#define XSP3_SCOPE_SEL123_TRIG_B_OUT	1
#define XSP3_SCOPE_SEL123_SERVO_OUT		2
#define XSP3_SCOPE_SEL123_DIG_OUT		3

/* Streams 4 & 5 are 8 input mux with more detail */
#define XSP3_SCOPE_SEL45_INP			0
#define XSP3_SCOPE_SEL45_TRIG_B_OUT		1
#define XSP3_SCOPE_SEL45_SERVO_OUT		2
#define XSP3_SCOPE_SEL45_GLITCH			3
#define XSP3_SCOPE_SEL45_TRIG_B_DIFF1	4
#define XSP3_SCOPE_SEL45_TRIG_B_DIFF2	5
#define XSP3_SCOPE_SEL45_SERVO_GRAD_ERR	6
#define XSP3_SCOPE_SEL45_SERVO_GRAD_EST	7
#define XSP3_SCOPE_SEL45_RESET_DET		8
#define XSP3_SCOPE_SEL45_TRIG_C_DIFF1	9
//! [XSP3_SCOPE_SOURCES]

#define XSP3_GSCOPE_CS_HIST_TO_DMA	(1<<2)

#define XSP3_DPC_HIST_TO_DMA		(1<<0)		// Send Hist dat to DMA rather than directly to 10 G Core
#define XSP3_DPC_ENB_10G_RX			(1<<1)		// Enable 10G Data to DMA channel
#define XSP3_DPC_ENB_10G_RX_ACKS	(1<<2)		// Enable Hardware Acknowledge packets fo 10G RX data and set multiplexer to connect 10G TX to ACKs.
#define XSP3_DPC_SEL_10G_TX_DMA		(1<<3)		// Select 10G Tx data path from DMA (usually for scope mode output).
#define XSP3_DPC_FARM_MODE			(1<<4)		// Enable Farm Mode (1 word = Chan = select PC/Port in Hist to LL direct to 10G

#define XSP3_DPC_RX_RESET			(1<<30)		// Reset all the RX data paths Excluding the xaui block
#define XSP3_DPC_TX_RESET			(1<<31)		// Reset all the TX data paths Excluding the xaui block

/** @defgroup XSP3_GLOBAL_STATUS_A	Global General Status Register A layout
@ingroup XSP3_MACROS
@{
*/
#define XSP3_GLOB_STAT_PLAYBACK_UR	(1<<1)		//!< Under run of data from DMA into Playback system
#define XSP3_GLOB_STAT_SCOPE_OR		(1<<2)		//!< Over Run of data into Scope DMA
#define XSP3_GLOB_STAT_SCALERS_OR	(1<<3)   	//!< Over Run of Frames into Scaler DMA
#define XSP3_GLOB_STAT_HIST_FRAMES_OR (1<<4)   	//!< Over Run of Frames in BRAM Histogrammer (XSpress3 Mini only so far)
#define XSP3_GLOB_STAT_HIST_LIST_OR (1<<5)   	//!< Over Run of Events into software Histogrammer (XSpress3 Mini only so far)
/** @} */

/** @defgroup XSP3_XLLDMA_STATUS	Xilinx Local Link DMA Status Register A layout
@ingroup XSP3_MACROS
@{
*/
#define XLLDMA_SR_IRQ_ON_END_MASK   0x00000040 /**< IRQ on end has occurred */
#define XLLDMA_SR_STOP_ON_END_MASK  0x00000020 /**< Stop on end has occurred */
#define XLLDMA_SR_COMPLETED_MASK    0x00000010 /**< BD completed */
#define XLLDMA_SR_SOP_MASK          0x00000008 /**< Current BD has SOP set */
#define XLLDMA_SR_EOP_MASK          0x00000004 /**< Current BD has EOP set */
#define XLLDMA_SR_ENGINE_BUSY_MASK  0x00000002 /**< Channel is busy */
/** @} */


/** @defgroup XSP3_XAXIDMA_STATUS	Xilinx AXI DMA Status Register A layout
@ingroup XSP3_MACROS
@{
*/
#define XAXIDMA_HALTED_MASK		0x00000001  /**< DMA channel halted */
#define XAXIDMA_IDLE_MASK		0x00000002  /**< DMA channel idle */
#define XAXIDMA_ERR_INTERNAL_MASK	0x00000010  /**< Datamover internal
						      *  err */
#define XAXIDMA_ERR_SLAVE_MASK		0x00000020  /**< Datamover slave err */
#define XAXIDMA_ERR_DECODE_MASK		0x00000040  /**< Datamover decode
						      *  err */
#define XAXIDMA_ERR_SG_INT_MASK		0x00000100  /**< SG internal err */
#define XAXIDMA_ERR_SG_SLV_MASK		0x00000200  /**< SG slave err */
#define XAXIDMA_ERR_SG_DEC_MASK		0x00000400  /**< SG decode err */
#define XAXIDMA_ERR_ALL_MASK		0x00000770  /**< All errors */

 /**< Channel is busy */
/** @} */


/** @defgroup XSP3_GLOB_ADC_DATA_MUX_CONT	Data Mux Control register Layout.
@ingroup XSP3_MACROS
@{
*/

#define XSP3_GLOB_ADC_MUX(adc)	(((adc)&0xF)<<0) // ADC Channel Mux selects any 1 channel which can be selected by any or all channels using XSP3_CC_SEL_DATA_MUX_DATA
												 // This allows different processing options to be trialled on the same data stream.
#define XSP3_GLOB_ADC_MUX_GRST_FROM0	0x8000 	// Force Channel 0 Reset onto GlobalResetActive, which can be used by all channels using XSP3_CC_SEL_DATA_GLOB_RST
/** @} */

/** @defgroup XSP3_GLOBAL_TIME_STATUS_A Global Timing Status Register A layout.
	@ingroup XSP3_MACROS
	@{
*/
//! [XSP3_GLOBAL_TIME_STATUS_A_CODE]
#define XSP3_GLOB_TSTAT_A_FRAME(x)			(((x)>>0)&0xFFFFFF)	//!< Read the current frame number value.
#define XSP3_GLOB_TSTAT_A_ITFG_COUNTING(x)	(((x)>>24)&0x1)		//!< Read whether the ITFG is currently enabling counting
#define XSP3_GLOB_TSTAT_A_ITFG_RUNNING(x)	(((x)>>25)&0x1)		//!< Read whether the ITFG is currently running.
#define XSP3_GLOB_TSTAT_A_ITFG_PAUSED(x)	(((x)>>26)&0x1)		//!< Read whether the ITFG is paused wait for software or hardware trigger
#define XSP3_GLOB_TSTAT_A_ITFG_FINISHED(x)	(((x)>>27)&0x1)		//!< Read whether the ITFG has finished a run. This clears when the bit XSP3_GLOB_TIMA_RUN is negated using {@link xsp3_histogram_stop()}.
//! [XSP3_GLOBAL_TIME_STATUS_A_CODE]

/** @} */

/** @defgroup XSP3_GLOB_RST_GEN Global reset Generator Register layout
  *  @ingroup XSP3_MACROS
  * @{
*/
#define XSP3_GLOB_RST_GEN_A_ENB					(1<<0)				//!< Enable Global reset generator
#define XSP3_GLOB_RST_GEN_A_SYNC_MODE(x)		(((x)&3)<<1)		//!< 0 => Drive Global reset as soon as requested. 1 => Syncronise global reset to beam using TTL_IN(3)
#define XSP3_GLOB_RST_GEN_A_DET_RESET_WIDTH(x)	(((x)&0x7F)<<4)		//!< Width in ADC clock cycles (80 MHz) of detector reset signal sent to the detector
#define XSP3_GLOB_RST_GEN_A_HOLD_OFF_TIME(x)	(((x)&0x3FF)<<12)	//!< Hold off time in ADC clock cycles from END of detector reset pulse until next cheching for global reset request
#define XSP3_GLOB_RST_GEN_A_ACTIVE_DELAY(x)		(((x)&0x7F)<<22)	//!< Delay from Det reset to detector asserted to Global Reset Active sent to all cards. (can be 0)

#define XSP3_GLOB_RST_GEN_B_ACTIVE_WIDTH(x)		(((x)&0x3FF)<<0)	//!< Width of Global reset Active signal sent to all cards
#define XSP3_GLOB_RST_GEN_B_CIRC_OFFSET(x)		(((x)&0x7FF)<<12)	//!< Delay from beam circulation trigger to Detector Reset assertion.

#define XSP3_GLOB_RST_GEN_MAX_RESET_WIDTH		0x7f
#define XSP3_GLOB_RST_GEN_MAX_HOLD_OFF			0x3ff
#define XSP3_GLOB_RST_GEN_MAX_ACTIVE_DELAY		0x7f
#define XSP3_GLOB_RST_GEN_MAX_ACTIVE_WIDTH		0x3ff
#define XSP3_GLOB_RST_GEN_MAX_CIRC_OFFSET		0x7FF
/**
@}
*/

extern int xsp3_bram_size_table[XSP3_REGION_RAM_MAX+1];
extern int xsp3_bram_width[XSP3_REGION_RAM_MAX+1];
extern const char *xsp3_bram_name[XSP3_REGION_RAM_MAX+1];
extern char *xsp3_feature_test_data_source_a[4] ;
extern char *xsp3_feature_test_data_source_b[4] ;
extern char *xsp3_feature_real_data_source[16] ;
extern char *xsp3_feature_data_mux20_pr[8] ;
extern char *xsp3_feature_data_mux3[2] ;

extern char *xsp3_feature_inl_corr_a[4] ;
extern char *xsp3_feature_inl_corr_b[4] ;
extern char *xsp3_feature_reset_detector[16] ;
extern char *xsp3_feature_reset_corr[16];
extern char *xsp3_feature_glitch_detect02[8] ;
extern char *xsp3_feature_glitch_detect3[2] ;
extern char *xsp3_feature_glitch_pad[16] ;
extern char *xsp3_feature_trigger_b_l[4] ;
extern char *xsp3_feature_trigger_b_m[4] ;
extern char *xsp3_feature_trigger_c[16] ;
extern char *xsp3_feature_trigger_extra0[2] ;
extern char *xsp3_feature_trigger_extra1[2] ;
extern char *xsp3_feature_trigger_extra2[2] ;
extern char *xsp3_feature_trigger_extra3[2] ;
extern char *xsp3_feature_calibrator[16] ;
extern char *xsp3_feature_neighbour_events0[2] ;
extern char *xsp3_feature_neighbour_events32[4] ;
extern char *xsp3_feature_servo_base[16] ;
extern char *xsp3_feature_servo_details0[2];
extern char *xsp3_feature_servo_details1[2];
extern char *xsp3_feature_servo_details23[4];
extern char *xsp3_feature_run_ave3[2] ;
extern char *xsp3_feature_lead_tail0[2];
extern char *xsp3_feature_lead_tail12[4] ;
extern char *xsp3_feature_output_format[16] ;
extern char *xsp3_feature_format_details_a01[4] ;
extern char *xsp3_feature_format_details_a23[4] ;
extern char *xsp3_feature_global_reset[16] ;
extern char *xsp3_feature_timing_generator[16] ;
extern char *xsp3_feature_scope_mode210[8] ;
extern char *xsp3_feature_scope_mode3[2] ;

//! [XSP3_RUN_FLAGS]
#define XSP3_RUN_FLAGS_PLAYBACK 1		//!< Enable build of descriptors and start of DMA for Playback DMA
#define XSP3_RUN_FLAGS_SCOPE 	2		//!< Enable build of descriptors and start of DMA for Scope Mode DMA(s)
#define XSP3_RUN_FLAGS_SCALERS 	4
#define XSP3_RUN_FLAGS_HIST 	8
#define XSP3_RUN_FLAGS_DIAG_HIST 0x10

#define XSP3_RUN_FLAGS_CIRCULAR_BUFFER 0x100	//!< Enable recircualting buffer mode.
//! [XSP3_RUN_FLAGS]

/** @defgroup XSP3_MT_FLAGS Flags to disable Thread per card options 
 * @ingroup XSP3_MACROS
 * @{
*/
#define XSP3_MT_DISABLE_SCOPE			1	//!< Disable Thread per card code in {@link xsp3_read_scope_data()}
#define XSP3_MT_DISABLE_MEASURE_TRIG	2	//!< Disable Thread per card code in measure trigger b code ?? 
/**
 * @}
 */
 
 /** @defgroup XSP3_SPI_ADDRESSES XSPRESS3 SPI Addresses
 * @ingroup XSP3_MACROS
 @{
 */
//! [XSP3_SPI_ADDRESSES_CODE]
 #define XSP3_SPI_S3_READ_WRITE_MASK 0xFFFFFF
#define XSP3_SPI_SS_SPARTAN		5
#define XSP3_SPI_SS_CLK0		0
#define XSP3_SPI_SS_CLK1		1
#define XSP3_SPI_SS_CLK2		2
#define XSP3_SPI_SS_CLK3		3
//! [XSP3_SPI_ADDRESSES_CODE]
/** @} */
/**
@defgroup XSP_SPI_REGISTERS XSPRESS3 SPI register bit layout.
@ingroup XSP_MACROS
@{
[XSP3_SPI_REGISTERS_CODE]
*/

#define XSP3_SPI_CLK_GOE0			(1<<0)	
#define XSP3_SPI_CLK_GOE1			(1<<1)
#define XSP3_SPI_CLK_GOE2			(1<<2)
#define XSP3_SPI_CLK_GOE3			(1<<3)
#define XSP3_SPI_CLK_NSYNC0			(1<<4)
#define XSP3_SPI_CLK_NSYNC1			(1<<5)
#define XSP3_SPI_CLK_NSYNC2			(1<<6)
#define XSP3_SPI_CLK_NSYNC3			(1<<7)
#define XSP3_SPI_DITHER				(1<<8)
#define XSP3_SPI_PSU(x)				(((x)&0x7F)<<9)
#define XSP3_SPI_TP_TYPE(x)			(((x)&0x3)<<21)
#define XSP3_SPI_TP_ENB				(1<<23)
#define XSP3_SPI_CLK_LD2			(1<<24)
#define XSP3_SPI_CLK_LD3			(1<<25)
#define XSP3_SPI_CLK_DIS_OVER_TEMP	(1<<9)
#define XSP3_SPI_CLK_SHUTDOWN0		(1<<10)
#define XSP3_SPI_CLK_SHUTDOWN123	(1<<11)
#define XSP3_SPI_CLK_SHUTDOWN4		(1<<12)
#define XSP3_SPI_CLK_SHUTDOWN5678	(1<<13)
/**
[XSP3_SPI_REGISTERS_CODE]
@}
*/
#define XSP3_NUM_LM75	4		//!< Number of LM75 devices on Xspress3
#define XSP3M_NUM_LM75	4		//!< Number of LM75 devices on Xspress3-Mini
#define XSP4_NUM_LM75	5		//!< Number of LM75 devices on Xspress4

//! [XSP3_I2C_REGISTERS]
#define XSP3_I2C_BUS_ADC	3
#define XSP3_LM75_ADDR(dev) (0x9<<3|((dev)&7))
#define XSP3M_LM75_ADDR(dev) (0x9<<3|(((dev)+1)&7))

#define XSP4_MP_LM75_ADDR(dev) (0x9<<3|(((dev)+5)&7))

#define XSP3_I2C_USE_REG_ADDR 0x80000000
#define XSP3M_LMK61E2_ADDR 88
#define XSP3M_FAN_DAC_ADDR 44

#define XSP3_I2C_BUS_LM82	0
#define XSP3_I2C_LM82_ADDR	0x18

#define XSP3M_I2C_DEV_ADC 0
#define XSP4_I2C_DEV_ADC 2
#define XSP4_I2C_BUS_MIDPLANE 2				//!< Bus number in server structures, but use  XSP4_I2C_BUS_MIDPLANE_TEMP etc to address via PCA9544
#define XSP4_I2C_DEV_MIDPLANE 3

#define XSP4_I2C_BUS_MIDPLANE_TEMP 		4
#define XSP4_I2C_BUS_MIDPLANE_FEM2_MON 	5
#define XSP4_I2C_BUS_MIDPLANE_DIGIO 	6
#define XSP4_I2C_BUS_MIDPLANE_CLK     	7

#define XSP4_LMK61E2_MP_ADDR 90

#define XSP4_I2C_ADDR_PCA9544		112
#define XSP4_I2C_PCA9544_CR(b)		(4|((b)&3))
//! [XSP3_I2C_REGISTERS]

//! [XSP3_CLOCK_SRC]
#define XSP3_CLK_SRC_INT				0		//!< channel processing clock comes from fpga processor (testing only)
#define XSP3_CLK_SRC_XTAL				1		//!< adc and channel processing clock from crystal on the ADC board (normal single board or master operation).
#define XSP3_CLK_SRC_EXT				2		//!< adc and channel processing clock from lemo clock connector on ADC board (slave boards)
#define XSP3_CLK_SRC_FPGA				3		//!< not implemented, for future expansion

#define XSP3M_CLK_SRC_CDCM61004			0x10	//!< Xspress3Mini or Xspress4 Clock Source CDCM61004 on ADC board
#define XSP3M_CLK_SRC_LMK61E2			0x11	//!< Xspress3Mini or Xspress4 Clock Source LMK61E2 on ADC board

#define XSP4_CLK_SRC_MIDPLN_CDCM61004	0x20	//!< Xspress4 Clock Source CDCM61004 on Mid plane board
#define XSP4_CLK_SRC_MIDPLN_LMK61E2		0x21	//!< Xspress4 Clock Source LMK61E2 on Mid Plane board
//! [XSP3_CLOCK_SRC]

//! [XSP3_CLOCK_FLAGS]
#define XSP3_CLK_FLAGS_MASTER		(1<<0)		// this clock generate clocks for other boards in the system
#define XSP3_CLK_FLAGS_NO_DITHER	(1<<1)		// disables dither within the ADC
#define XSP3_CLK_FLAGS_STAGE1_ONLY	(1<<2)		// performs stage of the lmk 03200 setup, does not enable zero delay mode
#define XSP3_CLK_FLAGS_NO_CHECK		(1<<3)		// dont check for lock detect from lmk 03200
#define XSP3_CLK_FLAGS_TP_ENB		(1<<4)		// enable test pattern from spartans
#define XSP3_CLK_FLAGS_DIS_OVER_TEMP (1<<5)		// Disable Over temperature protection on ADC Board
#define XSP3_CLK_FLAGS_SHUTDOWN0 	(1<<6)		// Shutdown ADC channel 0
#define XSP3_CLK_FLAGS_SHUTDOWN123 	(1<<7)		// Shutdown ADC channels 123
#define XSP3_CLK_FLAGS_SHUTDOWN4 	(1<<8)		// Shutdown ADC channel 4 (middle (unused?))
#define XSP3_CLK_FLAGS_SHUTDOWN5678 (1<<9)		// Shutdown ADC channel5678 last 4
#define XSP3_CLK_FLAGS_SHUTDOWN1 	(1<<10)		// Shutdown ADC channel 1  XSPRESS3-Mini and XSPRESS4
#define XSP3_CLK_FLAGS_SHUTDOWN2 	(1<<11)		// Shutdown ADC channel 2  XSPRESS4 only
#define XSP3_CLK_FLAGS_SHUTDOWN3 	(1<<12)		// Shutdown ADC channel 3  XSPRESS4 only
#define XSP3_CLK_FLAGS_SHUTDOWN5 	(1<<13)		// Shutdown ADC channel 5  XSPRESS4 only
#define XSP3_CLK_FLAGS_SHUTDOWN6 	(1<<14)		// Shutdown ADC channel 6  XSPRESS4 only
#define XSP3_CLK_FLAGS_SHUTDOWN7 	(1<<15)		// Shutdown ADC channel 7  XSPRESS4 only
#define XSP3_CLK_FLAGS_SHUTDOWN8 	(1<<16)		// Shutdown ADC channel 8  XSPRESS4 only
#define XSP3_CLK_FLAGS_SHUTDOWN9 	(1<<17)		// Shutdown ADC channel 9  XSPRESS4 only

#define XSP3_CLK_FLAGS_TS_SYNC 		(1<<30)		// Restart time stamp on Run on card 0
//! [XSP3_CLOCK_FLAGS]

/* Now removed from format control */
//#define XSP3_FORMAT_DISABLE_TFG		2	/* Write 1 here to disable TFG input and just use software controlled timing 	*/
//#define XSP3_FORMAT_DISABLE_SCALER		4	/* Disable scaler so top histogramming locations are not overwritten.		 	*/
//#define XSP3_FORMAT_DISABLE_HIST		8   /* Disable all histogram data and use all memory for many scalers.				*/
//! [XSP3_FORMAT_LAYOUT]
#define XSP3_FORMAT_CSHR_MODE(x)	(((x)&0x3)<<1)
#define XSP3_FORMAT_USER_TIMESTAMP	(1<<3)		//!< Output timestamped data for builds which support this.
#define XSP3_FORMAT_PILEUP_REJECT	(1<<31)
#define XSP3_FORMAT_AUX1_MODE(x)	(((x)&0xF)<<4)
#define XSP3_FORMAT_AUX1_THRES(x)	(((x)&0x3FF)<<8)
#define XSP3_FORMAT_NBITS_ENG(x)	(((x)&0xF)<<21)
#define XSP3_FORMAT_NBITS_ADC(x)	(((x)&0x7)<<25)
#define XSP3_FORMAT_AUX_MODE(x)		(((x)&0x7)<<28)
//! [XSP3_FORMAT_LAYOUT]

#define XSP3_FORMAT_CSHR_NONE		0		//!<	No Charge sharing processing
#define XSP3_FORMAT_CSHR_MARK		1		//!< 	Double size of aux1 dimesions (typically from 1 to 2) to mark Charge shared events as <normal aux0...aux n-1> <Cshared aux0...aux n-1>
#define XSP3_FORMAT_CSHR_DELETE		2		//!<	Delete Charge shared events from MCA and in window scalars but NOT AllEvent or AllGood.

#define XSP3_FORMAT_NBITS_AUX0	0
#define XSP3_FORMAT_NBITS_AUX4	1
#define XSP3_FORMAT_NBITS_AUX6	2
#define XSP3_FORMAT_NBITS_AUX8	3
#define XSP3_FORMAT_NBITS_AUX10	4
#define XSP3_FORMAT_NBITS_AUX12	5
#define XSP3_FORMAT_NBITS_AUX14	6
#define XSP3_FORMAT_NBITS_AUX16	7

//! [XSP3_AUX2_MODE]
#define XSP3_FORMAT_AUX2_ADC					0
#define XSP3_FORMAT_AUX2_WIDTH					1
#define XSP3_FORMAT_AUX2_RST_START_ADC			2
#define XSP3_FORMAT_AUX2_NEB_RST_ADC			3
#define XSP3_FORMAT_AUX2_NEB_RST_RST_START_ADC	4
#define XSP3_FORMAT_AUX2_TIME_FROM_RST			5
#define XSP3_FORMAT_AUX2_NEB_RST_TIME_FROM_RST	6
#define XSP3_FORMAT_AUX2_NEB_RST_TIME_FROM_RSTX8 7
//! [XSP3_AUX2_MODE]

//! [XSP3_AUX1_MODE]
#define XSP3_FORMAT_AUX1_MODE_NONE			0		//!< No Auxilliary data 1 									
#define XSP3_FORMAT_AUX1_MODE_PILEUP		7		//!< 1 bit of Aux1, set when pileup detected for Width or hardware 
#define XSP3_FORMAT_AUX1_MODE_CSHR			10		//!< 4 or 6 bits showing the 4 or 6 CShare status bit.
#define XSP3_FORMAT_AUX1_MODE_CSHR_ENC		11		//!< 3 bit 0..5 showing the priority encoded CShare status bits, otherwise coded 7 is "good"
#define XSP3_FORMAT_AUX1_MODE_GOOD_GRADE	15		//!< 0 bits, but Masks MCA (and scalers) for events with min < thres
#define XSP3_FORMAT_AUX1_MODE_TIMESTAMPED	100		//!< 0 bits, Auxiliary data is replaced by timestamp. Not written to hardware.
//! [XSP3_AUX1_MODE]


#define XSP3_FORMAT_RES_MODE_NONE		0		/* No resolution grades 									*/
#define XSP3_FORMAT_RES_MODE_MINDIV8	1		/* Normal Resolution grade mode of shorter of two runs / 8 	*/
#define XSP3_FORMAT_RES_MODE_THRES		2		/* 2 Res grades using threshold 						 	*/
#define XSP3_FORMAT_RES_MODE_LOG		3		/* Approx logarithmic grade 									*/
#define XSP3_FORMAT_RES_MODE_TOP		4		/* All 7 or 10 bits of top count							*/
#define XSP3_FORMAT_RES_MODE_BOT		5		/* All 7 or 10 bits of Bottom Count							*/
#define XSP3_FORMAT_RES_MODE_MIN		6		/* All 7 or 10 bits of Min of top and bottom				*/
#define XSP3_FORMAT_RES_MODE_PILEUP		7		/* 1 bit of Aux1, set when pileup detected for Width or hardware */
#define XSP3_FORMAT_RES_MODE_LUT_SETUP	8		/* 12 bits of res grade to learn setup for LO/LUT mode 		*/
#define XSP3_FORMAT_RES_MODE_LUT_THRES	9		/* 1 bit thresholded using the LUT LSB						*/

#define XSP3_FORMAT_RES_MODE_GOOD_GRADE	15		/* 0 bits, but Masks MCA (and scalers) for events with min < thres	*/

#define XSP3_FORMAT_GET_CSHR_MODE(x)		(((x)>>1)&0x3)
#define XSP3_FORMAT_GET_AUX1_MODE(x)		(((x)>>4)&0xF)
#define XSP3_FORMAT_GET_AUX1_THRES(x)		(((x)>>8)&0x3FF)
#define XSP3_FORMAT_GET_NBITS_ENG_LOST(x)	(((x)>>21)&0xF)
#define XSP3_FORMAT_GET_NBITS_ADC(x)		(((x)>>25)&0x7)
#define XSP3_FORMAT_GET_AUX2_MODE(x)		(((x)>>28)&0x7)

#define XSP3_MIN_BITS_ENG  1
#define XSP3_MAX_BITS_ENG  12

/** @defgroup XSP3_DTC_FLAGS Dead Time Correction Flags
* @ingroup XSP3_MACRO
* @{
*/
#define XSP3_DTC_OMIT_CHANNEL (1<<0)
#define XSP3_DTC_USE_GOOD_EVENT (1<<1)
/** @} */

#define XSP3_SCALER_TIME			0
#define XSP3_SCALER_RESETTICKS		1
#define XSP3_SCALER_RESETCOUNT		2
#define XSP3_SCALER_ALLEVENT		3
#define XSP3_SCALER_ALLGOOD			4
#define XSP3_SCALER_INWINDOW0		5
#define XSP3_SCALER_INWINDOW1		6
#define XSP3_SCALER_PILEUP			7

#define XSP3_MDIO_DIRECT	0
#define XSP3_MDIO_AUX		1


/*
 * fan_control.h
 *
 *  Created on: 20 Feb 2013
 *      Author: wih73
 */
typedef struct _fan_log {
	int16_t temp; 	// Signed int in 0.5 deg C units
	int16_t speed; // 0 = off, 4000 = full speed
} FanLog;

typedef struct _xs3m_fan_log {
	int16_t temp; 	// Signed int in 0.5 deg C units
	int16_t zynq_temp; 	// Signed int in 0.5 deg C units
	int16_t speed; // 0 = off, 4000 = full speed
} X3mFanLog;

#define XSP3_FAN_LOG_POINTS 10000
#define XSP3_FAN_MODE_OFF				0
#define XSP3_FAN_MODE_MONITOR_LOOP 		1
#define XSP3_FAN_MODE_MONITOR_ONESHOT 	2
#define XSP3_FAN_MODE_CONTROL 			3

#define XSP3_FAN_CONT_NUM_AVERAGE 10
#define XSP3_FAN_CONT_FRAC_PRECISION 10

typedef struct _fan_cont
{
	int32_t mode;
	int32_t start_run;
	int32_t cur_temp[4];
	int32_t cur_max;
	int32_t target, p_const, i_const;
	int32_t cur_point;
	FanLog log[XSP3_FAN_LOG_POINTS];
} FanControl;
typedef struct _x3m_fan_cont
{
	int32_t mode;
	int32_t start_run;
	int32_t cur_temp[4];
	int32_t cur_max;
	int32_t target, p_const, i_const;
	int32_t cur_point;
	X3mFanLog log[XSP3_FAN_LOG_POINTS];
} X3mFanControl;

#define XSP3_FAN_OFFSET_MODE 		0
#define XSP3_FAN_OFFSET_START 		1
#define XSP3_FAN_OFFSET_TEMP 		2
#define XSP3_FAN_OFFSET_MAX 		6
#define XSP3_FAN_OFFSET_SET_POINT 	7
#define XSP3_FAN_OFFSET_P_CONST 	8
#define XSP3_FAN_OFFSET_I_CONST 	9
#define XSP3_FAN_OFFSET_CUR_POINT 	10
#define XSP3_FAN_OFFSET_LOG			11

/**
 * @defgroup XSP3_FEATURES  Macros to describe features enabled in current firmware build	
 * @ingroup XSP3_MACROS
  @{
*/
#define XSP3_FEATURE_GET_TEST_DATA_SOURCE(x)	(((x)>>0)&0xF)
#define XSP3_FEATURE_GET_REAL_DATA_SOURCE(x)	(((x)>>4)&0xF)
#define XSP3_FEATURE_GET_DATA_MUX(x)			(((x)>>8)&0xF)
#define XSP3_FEATURE_GET_INL_CORR(x)			(((x)>>12)&0xF)
#define XSP3_FEATURE_GET_RESET_DETECTOR(x)		(((x)>>16)&0xF)
#define XSP3_FEATURE_GET_RESET_CORR(x)			(((x)>>20)&0xF)
#define XSP3_FEATURE_GET_GLITCH_DETECT(x)		(((x)>>24)&0xF)
#define XSP3_FEATURE_GET_GLITCH_PAD(x)			(((x)>>28)&0xF)

#define XSP3_FEATURE_GET_TRIGGER_B(x)			(((x)>>0)&0xF)
#define XSP3_FEATURE_GET_TRIGGER_C(x)			(((x)>>4)&0xF)
#define XSP3_FEATURE_GET_TRIGGER_EXTRA(x)		(((x)>>8)&0xF)
#define XSP3_FEATURE_GET_CALIBRATOR(x)			(((x)>>12)&0xF)
#define XSP3_FEATURE_GET_NEIGHBOUR_EVENTS(x)	(((x)>>16)&0xF)
#define XSP3_FEATURE_GET_SERVO_BASE(x)			(((x)>>20)&0xF)
#define XSP3_FEATURE_GET_SERVO_DETAIL(x)		(((x)>>24)&0xF)
#define XSP3_FEATURE_GET_RUN_AVE(x)				(((x)>>28)&0xF)

#define XSP3_FEATURE_GET_LEAD_TAIL_CORR(x)		(((x)>>0)&0xF)
#define XSP3_FEATURE_GET_OUTPUT_FORMAT(x)		(((x)>>4)&0xF)
#define XSP3_FEATURE_GET_FORMAT_DETAILS_A(x)	(((x)>>8)&0xF)
#define XSP3_FEATURE_GET_FORMAT_DETAILS_B(x)	(((x)>>12)&0xF)
#define XSP3_FEATURE_GET_GLOBAL_RESET(x)		(((x)>>16)&0xF)
#define XSP3_FEATURE_GET_TIMING_SOURCE(x)		(((x)>>20)&0xF)
#define XSP3_FEATURE_GET_TIMING_GENERATOR(x)	(((x)>>24)&0xF)
#define XSP3_FEATURE_GET_SCOPE(x)				(((x)>>28)&0xF)

#define XSP3_FEATURE_GET_TEST_SRC_A(x)			(((x)>>0)&3)	//!< Test source A from lower 2 bits.
#define XSP3_FEATURE_GET_TEST_SRC_B(x)			(((x)>>2)&3)	//!< Test source A from upper 2 bits.

#define XSP3_FEATURE_TEST_SRC_A_NONE			0				//!< No Playback.
#define XSP3_FEATURE_TEST_SRC_A_PB2				1				//!< Output is list of complete 32 address offsets for histogramming.
#define XSP3_FEATURE_TEST_SRC_B_NONE			0				//!< No test pattern generator (current builds).
#define XSP3_FEATURE_TEST_SRC_B_TPGEN			1				//!< BRAM based TP generator.

#define XSP3_FEATURE_RESET_CORR_FIXED1024		0				//!< Only build is fixed 1024 point table.

/**
@defgroup XSP_FEATURE_INL INL Correction and NBitsFrac
@ingroup XSP3_FEATURES
@{
*/
#define XSP3_FEATURE_INL_GET_INL(x)             ((x)&3)			//! Get bits 1..0 used to Code INL correction type
#define XSP3_FEATURE_INL_NONE					0				//!< No INL correction.
#define XSP3_FEATURE_INL_YES					1				//!< INL correction.
#define XSP3_FEATURE_INL_GET_NFRAC(x)             ((x)>>2)		//! Get bits 3..2 used to Code NBitsFrac Back End
#define XSP3_FEATURE_INL_NFRAC2					0				//!< NBitsFracBE == 2
#define XSP3_FEATURE_INL_NFRAC4					1				//!< NBitsFracBE == 4

/**
@}
*/

/**
@defgroup XSP_FEATURE_GDET Crosstalk Glitch detector type.
@ingroup XSP3_FEATURES
@{
*/
#define XSP3_FEATURE_GDET_TYPE_MASK             7				//! Mask to extract the Gdet type.
#define XSP3_FEATURE_GDET_NONE					0				//!< No glitch detector.
#define XSP3_FEATURE_GDET_THRES8				1				//!< Glitch detector from gradient, original 8 bit threshold register layout.
#define XSP3_FEATURE_GDET_THRES10				2				//!< Glitch detector from gradient, modified 10 bit threshold register layout.
#define XSP3_FEATURE_GDET_LONG					3				//!< Glitch detector from gradient, modified 10 bit threshold register layout and upto 511 pre-delay..

#define XSP3_FEATURE_GDET_RST_XTK			 	8				//!< Adds Direct glitch from neighbouring resets feature

/**
@}
*/
/**
@defgroup XSP3_FEATURE_NEIGHBOUR_EVENTS Neighbour coincidence and crosstalk correction options.
@ingroup XSP3_FEATURES
@{
*/
#define XSP3_FEATURE_NEB_GET_XTK_CORR(x)			(((x)>>2)&0x3)	//!< Get 2 bits of Xtk correction type. 0 and 1 defined so far.
#define XSP3_FEATURE_NEB_XTK_CORR_NONE    			0				//!< No crosstalk correction (does not preclude neighbour event triggering )
#define XSP3_FEATURE_NEB_XTK_CORR_ADC_SUBTRACT    	1				//!< Subtraction of Scaled crosstalk signatures before servo and running average with single pass trigger.
#define XSP3_FEATURE_NEB_XTK_CORR_ADC_SUB_AND_CSHR  2				//!< Subtraction of Scaled crosstalk signatures before servo and running average with single pass trigger and Charge sharing detection.


/**
@}
*/
/**
@defgroup XSP3_FEATURE_TRIGGER Trigger options.
@ingroup XSP3_FEATURES
@{
*/
/* to be extracted from features.trigger_b */

#define XSP3_FEATURE_GET_TRIGGER_B_SRL32(x)		(((x)>>1)&1)		//!< Get if trigger-B is a SRL32 type delay wih minimum delay 1 instead of 3
#define XSP3_FEATURE_GET_TRIGGER_B_RINGING(x)	(((x)>>2)&3)		//!< Get ringing subraction filter option
/* to be extracted from features.trigger_extra */
#define XSP3_FEATURE_GET_TRIGGER_B_DUAL_DIFF(x) (((x)>>1)&1)		//!< Get if trigger-B has Dual first differential option.
#define XSP3_FEATURE_GET_TRIGGER_B_VARY_WID(x)  (((x)>>2)&1)		//!< Get if trigger-B has Varianle width CFD signal option.
#define XSP3_FEATURE_GET_TRIGGER_B_FINE_TIME(x) (((x)>>3)&1)		//!< Get if trigger-B has fine time features.
/**
@}
*/

/**
@defgroup XSP_FEATURE_LEAD_TAIL Running Average processing options.
@ingroup XSP3_FEATURES
@{
*/
#define XSP3_FEATURE_LEAD_TAIL_LEAD				1				//!< Include Correction for event lead in.
#define XSP3_FEATURE_LEAD_TAIL_GET_WIDTH(x)		(((x)>>1)&3)	//!< Get feature of event lead/tail correction based on OTD width.
#define XSP3_FEATURE_LEAD_TAIL_WIDTH_NONE			0				//!< No dependence on Width available.
#define XSP3_FEATURE_LEAD_TAIL_WIDTH4				1				//!< Run ave lead/tail correction depends on width(3 downto 0)
/**
@}
*/

/**
@defgroup XSP_FEATURE_LEAD_TAIL Running Average processing options.
@ingroup XSP3_FEATURES
@{
*/
#define XSP3_FEATURE_LEAD_TAIL_LEAD				1				//!< Include Correction for event lead in.
#define XSP3_FEATURE_LEAD_TAIL_GET_WIDTH(x)		(((x)>>1)&3)	//!< Get feature of event lead/tail correction based on OTD width.
#define XSP3_FEATURE_LEAD_TAIL_WIDTH_NONE			0				//!< No dependence on Width available.
#define XSP3_FEATURE_LEAD_TAIL_WIDTH4				1				//!< Run ave lead/tail correction depends on width(3 downto 0)
/**
@}
*/

#define XSP3_FEATURE_SERVO_BASE_NONE			0				//!< No servo.
#define XSP3_FEATURE_SERVO_BASE_PWL1			1				//!< Single table (512 points) PWL servo.
#define XSP3_FEATURE_SERVO_BASE_PWL16			2				//!< 16 table or 1 x 2048 points PWL servo.
#define XSP3_FEATURE_SERVO_BASE_DUAL			3				//!< Combined linear (gross) and PWL (single table) servo.

#define XSP3_FEATURE_GET_FORMAT_A_NBITS(x)			(((x)>>0)&3)	//!< Codes Nbits energy 12, 13,14Test source A from lower 2 bits.
#define XSP3_FEATURE_GET_FORMAT_A_AUX1(x)			(((x)>>2)&3)	//!< Codes Aux1 functionality.

#define XSP3_FEATURE_AUX1_FUNC_NONE				0		//!< None or Just good mode.
#define XSP3_FEATURE_AUX1_FUNC_DEBUG			1		//!< Debug modes.
#define XSP3_FEATURE_AUX1_FUNC_THRES			2		//!< Thresholded Good/bad and Debug.
#define XSP3_FEATURE_AUX1_FUNC_FULL				3		//!< Full functionality.


#define XSP3_FEATURE_OUTPUT_FORMAT_ADDR32		0		//!< Output is list of complete 32 address offsets for histogramming.
#define XSP3_FEATURE_OUTPUT_FORMAT_HEIGHTS64	1		//!< Output is list of 64 bit words including processed event height and all auxiliary info.
#define XSP3_FEATURE_OUTPUT_FORMAT_RAW_AVERAGES	2		//!< Output is Raw running avreages, needing lead and tail correction and then top-bottom subtraction.
#define XSP3_FEATURE_OUTPUT_FORMAT_HEIGHTS64TS	3		//!< Output is list of 64 bit words including processed event height and all auxiliary info and timestamps
#define XSP3_FEATURE_OUTPUT_FORMAT_ADDR32_TF64	4		//!< Output is list of complete 32 address offsets for histogramming with 64 bit time frame for Circular buffers

#define XSP3_FEATURE_OUTPUT_FORMAT_DIFFERENCES	8		//!< Output is ADC input data as a list of differences.

#define XSP3_FEATURE_FORMAT_B_ADDR32_NO_ROI			8		//!< If XSP3_FEATURE_OUTPUT_FORMAT_ADDR32 this bit implies that Region of Interest have been disabled

#define XSP3_FEATURE_FORMAT_B_HGT64_ACK_EOF			4		//!< If XSP3_FEATURE_OUTPUT_FORMAT_HEIGHTS64 this bit implies there is an Acknowledge retry on the eond of frame markers.
#define XSP3_FEATURE_FORMAT_B_HGT64_ACK_TIME		1		//!< If XSP3_FEATURE_OUTPUT_FORMAT_HEIGHTS64 this bit implies that the ackl and any frame data sends to previous frames total time.
/** 
@}
*/
/** 
	@defgroup XSP3_FEATURES_RESET_DETECTOR Macros describing the reset detector
	@ingroup XSP3_FEATURES
	@{
*/
#define XSP3_FEATURE_RESET_DETECTOR_NONE			0		//!< Reset Detector is not present
#define XSP3_FEATURE_RESET_DETECTOR_V1				1		//!< Reset Detector is orginal version without long hold off period.
#define XSP3_FEATURE_RESET_DETECTOR_LONG_HOLDOFF64	2		//!< Reset Detector version 2 with long hold off period x64 clock cycles.
/** @} */

/** 
	@defgroup XSP3_FEATURES_TRIGGER_C Macros describing the trigger-C
	@ingroup XSP3_FEATURES
	@{
*/
#define XSP3_FEATURE_TRIGGER_C_NONE					0		//!< Trigger-C is not present
#define XSP3_FEATURE_TRIGGER_C_V1					1		//!< Trigger-C is orginal version without min width feature
#define XSP3_FEATURE_TRIGGER_C_MIN_WDITH			2		//!< Trigger-C with minimum width code.
/** @} */
/** 
	@defgroup XSP3_FEATURES_TIMING_GEN Macros describing the internal timing generator
	@ingroup XSP3_FEATURES
	@{
*/
#define XSP3_FEATURE_TIMING_GEN_NONE			0		//!< Timing Generator is not present
#define XSP3_FEATURE_TIMING_GEN_MINIMAL_ITFG	1		//!< Timing Generator is minimal Internal TFG generating nframe all of same length, burst, started or all triggered.
#define XSP3_FEATURE_TIMING_GEN_MINIMAL2_ITFG	2		//!< Timing Generator is minimal 2nd generation Internal TFG generating nframe all of same length, burst, started or all triggered.
														//!< With continuous mode and option ot acquire while paused - to count between edges of trigger.
#define XSP3_FEATURE_TIMING_GEN_MINIMAL_PLUS_MARKER_ITFG 3 //!< Minimal ITFG with test marker option.											
#define XSP3_FEATURE_TIMING_GEN_MINIMAL3_ITFG	4		//!< Timing Generator is minimal 2nd generation Internal TFG generating nframe all of same length, burst, started or all triggered.
														//!< With continuous mode and option to acquire waiting for 1 or counter triggers.
#define XSP3_FEATURE_TIMING_GEN_MINIMAL3_PLUS_MARKER_ITFG 5 //!< Minimal ITFG with test marker option.											
															//!< With continuous mode and option to acquire waiting for 1 or counter triggers.
#define XSP3_FEATURE_TIMING_GEN_FULL_ITFG		8  		//!< Place holder for proposed full feature internal time frame generator.

/** @} */

/** 
	@defgroup XSP3_FEATURS_SCOPE Macros describing the scope mode features
	@ingroup XSP3_FEATURES
	@{
*/
#define XSP3_FEATURE_SCOPE_BIT15_GR_ONLY		0		//!< Bit 15 of scope mode is alway Global Reset active (origianl builds)
#define XSP3_FEATURE_SCOPE_BIT15_ALT_ENB		1		//!< Bit 15 of scope mode is uses alternate bits 3..0 to allow HistEnable to be seen.
#define XSP3_FEATURE_SCOPE_ADC_CLK_MMCM_DRP		8		//!< For Xspress4 (and eventually Xspress3Mini), this bit marks that the ADC CLKC MMCP supports DRP.
/** @} */

#define XSP3_FEATURE_FORMAT_B_NO_ROI 			(1<<3)	//!< Output does not have Roi function.

#define XSP3_HGT64_SOF_GET_FRAME(x)				(((x)>>0)&0xFFFFFF)	//!< Get time frame from first (header) word
#define XSP3_HGT64_SOF_GET_PREV_TIME(x)				(((x)>>24)&0xFFFFFFFF)	//!< Get total integration time from previous time frame from first (header) word
#define XSP3_HGT64_SOF_GET_CHAN(x)				(((x)>>60)&0xF)		//!< Get channel number from first (header) word

#define XSP3_HGT64_GET_HEIGHT(x)				(((x)>>0)&0xFFF)	//!< Get event height
#define XSP3_HGT64_GET_IN_RANGE(x)				(((x)>>12)&1)		//!< Get in range signal
#define XSP3_HGT64_GET_CAL_EVENT(x)				(((x)>>13)&1)		//!< Get cal event flag
#define XSP3_HGT64_GET_DIFF_NEGATIVE(x)			(((x)>>14)&1)		//!< Get diff negative flag
#define XSP3_HGT64_GET_RESET(x)					(((x)>>15)&1)		//!< Get reset flag
#define XSP3_HGT64_GET_GOOD_GRADE(x)			(((x)>>16)&1)		//!< Get good grade flag
#define XSP3_HGT64_GET_AUX2(x)					(((x)>>17)&0xFFFF)	//!< Get Aux2 data
#define XSP3_HGT64_GET_WIDTH(x)					(((x)>>33)&0xFF)	//!< Get event or Reset Width
#define XSP3_HGT64_GET_RESET_START(x)			(((x)>>41)&0xF)		//!< Get Reset Start value
#define XSP3_HGT64_GET_CHAN(x)					(((x)>>48)&0xF)		//!< Get channel
#define XSP3_HGT64_GET_AUX1(x)					(((x)>>52)&0xFFF)	//!< Get AUX1 data

#define XSP3_HGT64_GET_RESET_WIDTH(x)			(((x)>>0)&0x3FF)	//!< Get Reset width which replaces event height.
#define XSP3_HGT64_DUMMY_RESET_LEN				0x400					//!< Value to add to reset tick when a dummy reset occurs
#define XSP3_HGT64_MASK_IN_RANGE				(1L<<12)				//!< In Range mask
#define XSP3_HGT64_MASK_CAL_EVENT				(1L<<13)				//!< Mask for Calibration event
#define XSP3_HGT64_MASK_DIFF_NEGATIVE			(1L<<14)				//!< Mask for difference (top-bot) < 0
#define XSP3_HGT64_MASK_RESET					(1L<<15)				//!< Mask for real or reset
#define XSP3_HGT64_MASK_GOOD_GRADE				(1L<<16)				//!< Mask for good resolution grade
#define XSP3_HGT64_MASK_RESET_DUMMY				((u_int64_t)1<<45)				//!< Mask for dummy reset.

#define XSP3_HGT64_MASK_CSHARE					((u_int64_t)1<<49)				//!< Mask for Charge Shared event

#define XSP3_HGT64_MASK_TF_DELETE				((u_int64_t)1<<56)				//!< Mask for Time Frame Delete (clear) previous frame data.
#define XSP3_HGT64_MASK_TF_MARKER1				((u_int64_t)1<<57)				//!< Mask for Time Frame Status Marker 1
#define XSP3_HGT64_MASK_TF_MARKER2				((u_int64_t)1<<58)				//!< Mask for Time Frame Status Marker 2

#define XSP3_HGT64_MASK_END_OF_FRAME			((u_int64_t)1<<59)				//!< Mask for End of Frame Marker.

#define XSP3_HGT64_GET_TS(x)					( (((x)>>17)&0xFFFF) | (((x)>>(46-16))&0x30000) | (((x)>>(52-18))&0x3FFC0000) )	//!< Timestamp

#define XSP3_DIFFS_CODE_DIFFS					0						//!< Differences mode : Differences data
#define XSP3_DIFFS_CODE_TIME_FRAME				1						//!< Differences mode : Time frame first or change
#define XSP3_DIFFS_GET_CODE(x)					(((x)>>60)&0xF)			//!< Differences mode : Get Data code bits

#define XSP3_DIFFS_TF_FIRST_MASK				(1L<<59)				//!< Diffs Time frame is first word 
#define XSP3_DIFFS_TF_CHANGE_MASK				0						//!< Diffs time frame is change of TF 
#define XSP3_DIFF_FIRST_CHAN(x)					(((x)>>55)&0xF)			//!< Diff mode First word, extrac channel
#define XSP3_DIFFS_TF_TIME_STAMP(x)				((x)&0x3FFFFFFF)		//!< Diffs time frame change Extract time stamp
#define XSP3_DIFFS_TF_FRAME(x)					(((x)>>30)&0xFFFFFF)	//!< Diffs time frame change Extract time frame
#define XSP3_DIFFS_TF_ENABLE(x)					(((x)>>(24+30))&1)		//!< Diffs time frame change Extract Count Enable

#define XSP3_DIFFS_DATA_IDLE					0						//!< Diffs 10 bit data special code for idle.
#define XSP3_DIFFS_DATA_START					1						//!< Diffs 10 bit data special code for start.
#define XSP3_DIFFS_DATA_RESET					2						//!< Diffs 10 bit data special code for End of Reset.
#define XSP3_DIFFS_DATA_GLOB_RST				3						//!< Diffs 10 bit data special code for End of Global Reset.
/**
	@defgroup XSP3_ITFG_REGS   Internal Time Frame Generator Register arrangement
	@ingroup XSP3_MACROS
	@{
*/

#define XSP3_ITFG_SET_FRAMES(x)					((x)&0xFFFFFF)			//!< Set number of frames for internal TFG
#define XSP3_ITFG_SET_TRIG_MODE(x)				(((x)&7)<<24)			//!< Set trigger mode for internal TFG. See {@link XSP3_ITFG_TRIG_MODE}
#define XSP3_ITFG_ACQ_WHEN_PAUSED(x)			(((x)&3)<<27)			//!< Acquire when pasued mode ignores the frame length and acquires between active edges of trigger.
#define XSP3_ITFG_SET_GAP_MODE(x)				(((x)&3)<<30)			//!< Set Miniumum mode for internal TFG
#define XSP3_ITFG_MAX_NUM_FRAMES 				0xFFFFFF				//!< Maximum number of time frames for internal TFG. Note that configurAtion of rest of system will usually limit the number of frame to less than this.
#define XSP3_ITFG_GET_FRAMES(x)					((x)&0xFFFFFF)			//!< Get number of frames for internal TFG
#define XSP3_ITFG_GET_TRIG_MODE(x)				(((x)>>24)&7)			//!< Get trigger mode for internal TFG
#define XSP3_ITFG_GET_GAP_MODE(x)				(((x)>>30)&3)			//!< Get fraem to frame gap mode for internal TFG

#define XSP3_ITFG_SET_MARK_FRAME(x)				((x)&0xFFFF)			//!< Set marker frame of ITFG Marker register where supported
#define XSP3_ITFG_SET_MARK_PERIOD(x)			(((x)&0xFFFF)<<16)		//!< Set marker period of ITFG Marker register where supported. Actually write reguired period-1
/**
 @}
 */

/**
	@defgroup XSP3_ITFG_TRIG_MODE   Internal Time Frame Generator Trigger Modes
	@ingroup XSP3_ITFG_REGS
	@{
*/
#define XSP3_ITFG_TRIG_MODE_BURST				0						//!< Run burst of back to back frames.
#define XSP3_ITFG_TRIG_MODE_SOFTWARE			1						//!< Pause before every frame and wait for rising edge on CountEnb bit.
#define XSP3_ITFG_TRIG_MODE_HARDWARE			2						//!< Pause before every frame and wait for rising edge on TTL_IN(1).
#define XSP3_ITFG_TRIG_MODE_PB0GLOB_RESET		3						//!< Pause before every frame and wait for Playback 0 Global reset (test mode for sub-frames)
#define XSP3_ITFG_TRIG_MODE_SOFTWARE_ONLY_FIRST	5						//!< Pause before first frame and wait for rising edge on CountEnb bit.
#define XSP3_ITFG_TRIG_MODE_HARDWARE_ONLY_FIRST	6						//!< Pause before first frame and wait for rising edge on TTL_IN(1).

/**
 @}
*/
/**
	@defgroup XSP3_ITFG_ACQ_PAUSED   Internal Time Frame Generator Acquire while paused Modes.
	In these modes the ITFG Acquires data while waiting fro 1 or more triggers, re-using the frame length register.
	@ingroup XSP3_ITFG_REGS
	@{
*/
#define XSP3_ITFG_TRIG_ACQ_PAUSED_OFF			0						//!< Normal operation, do not acquire while paused wait for trigger
#define XSP3_ITFG_TRIG_ACQ_PAUSED_ALL			1						//!< Acquire when waiting for trigger(s), including while waiting for the first trigger
#define XSP3_ITFG_TRIG_ACQ_PAUSED_EXCL_FIRST	2						//!< Acquire when waiting for trigger(s), except when waiting for first trigger. So first trigger starts frame 0.

/**
 @}
 */
/**
	@defgroup XSP3_ITFG_GAP_MODE   Internal Time Frame Generator Minimum frame to frame gap
	@ingroup XSP3_ITFG_REGS
	The gap names give the correct gap width when the ADC clock is 80 MHz. At other clock frequencies the times will scale. The debounce time will scale by the same amount, so the settings in cycles are still valid.
	To find the actual ADC clock period use {@link xsp3_get_clock_period()}
	@{
*/
#define XSP3_ITFG_GAP_MODE_25NS				0						//!< Minimal gap between frames. Care when using multiple boxes. Short cables and/or termination. 0 debounce time.
#define XSP3_ITFG_GAP_MODE_200NS			1						//!< 200ns gap between frames. Use short cables and short (approx 10 cycle debounce time) when using multiple boxes.
#define XSP3_ITFG_GAP_MODE_500NS			2						//!< 500ns gap between frames. Use approx 30 cycle debounce time when using multiple boxes.
#define XSP3_ITFG_GAP_MODE_1US				3						//!< 1us gap between frames. Allows long cables and  approx 70 cycle debounce time when using multiple boxes.

/**
 @}
 */

#define XSP3_UDP_SIG (SIGRTMAX-1)							//!< Signal used for UDP timeout on no data.

/* Start of XSPESS3 in Virtex 7 and XSPRESS4 code
 *
 *
 */
#define XSP4_GLOB_REG_BUS_CLK_FLAG		0x100000

#define XSP4_CHAN_GLOB_ADC_CLK 31
#define XSP4_CHAN_GLOB_BUS_CLK 30
#define XSP4_NUM_GLOB_REG_ADC_CLK 32
#define XSP4_NUM_GLOB_REG_BUS_CLK 5
//#define XSP4_MAX_CHANS_PER_CARD 9

/* Global registers in bus clock domain */
#define XSP4_GLOB_CLOCK_CONT		(0|XSP4_GLOB_REG_BUS_CLK_FLAG)
#define XSP34_GLOB_BC_FAN_SPEED 	(1|XSP4_GLOB_REG_BUS_CLK_FLAG)
#define XSP4_GLOB_10G_TPG 			(2|XSP4_GLOB_REG_BUS_CLK_FLAG)
#define XSP4_GLOB_ADC_CONT 			(3|XSP4_GLOB_REG_BUS_CLK_FLAG)
#define XSP4_GLOB_AURORA_CONT 		(4|XSP4_GLOB_REG_BUS_CLK_FLAG)

#define XSP4_GLOB_ADC_CLK_FREQ 		(16|XSP4_GLOB_REG_BUS_CLK_FLAG)

#define XSP4_GLOB_AURORA_STATUS0	(29|XSP4_GLOB_REG_BUS_CLK_FLAG)
#define XSP4_GLOB_AURORA_STATUS1	(30|XSP4_GLOB_REG_BUS_CLK_FLAG)
#define XSP4_GLOB_ADC_BRD_STATUS	(31|XSP4_GLOB_REG_BUS_CLK_FLAG)

#define XSP4_10G_TPG_NQWORDS(n)		((n)&0xFFFFF)
#define XSP4_10G_TPG_NBLOCKS(n)		(((n)&0x3FF)<<20)
#define XSP4_10G_TPG_ENABLE			(1<<31)

/**
	@defgroup XSP4_ADC_CLOCK_FREQ   Control bits for Board to Board Aurora Links.
	@ingroup XSP3_MACROS
	@{
*/
#define XSP4_ADC_CLOCK_FREQ_GET_FREQ(x)	((x)&0xFFFFF)		//!< Get the Frequency counter part of the ADC Clock frequency Measurer
#define XSP4_ADC_CLOCK_FREQ_READY		(1<<31)				//!< The MSB is an overall ready bit, but gives no information if the measurement fails.
#define XSP4_ADC_CLOCK_FREQ_TIMER_READY	(1<<30)				//!< The Bus Clk timer has expired, so the ADC Clk should be ready
#define XSP4_ADC_CLOCK_FREQ_ADC_VALID	(1<<29)				//!< Reports that the ADC Clk has seem the timer expire is ready. If not The Bus Clk timer has expired, so the ADC Clk should be ready
/** @} */

/**
	@defgroup XSP4_AURORA_CONTROL   Control bits for Board to Board Aurora Links.
	@ingroup XSP3_MACROS
	@{
*/
#define XSP4_AURORA_CONT_SET_LOOPBACK(stream,val)		(((val)&7)<<((stream)*4))		//!< Set loopback code for board to board stream0..5 */
#define XSP4_AURORA_CONT_SET_RXCDROVRDEN(stream,val)	(((val)&1)<<(3+(stream)*4))		//!< Set override into loopback mode?
#define XSP4_AURORA_CONT_GET_LOOPBACK(stream,reg)		(((reg)>>((stream)*4)))&7)		//!< Get loopback code for board to board stream0..5 */
#define XSP4_AURORA_CONT_GET_RXCDROVRDEN(stream,reg)	(((reg)>>(3+(stream)*4)))&1)	//!< Get override into loopback mode?

#define XSP4_AURORA_CONT_DISABLE_ECC	(1<<30)			//!< Disable Error Detection and correction on the board to board serial links
#define XSP4_AURORA_CONT_RESET			(1<<31)			//!< Force reset of all board to board tranceivers

#define XSP4_AURORA_CONT_LOOPBACK_NONE	 	0			//!< Normal operation
#define XSP4_AURORA_CONT_LOOPBACK_NEAR_PCS	1			//!< Near-End loopback after the PCS 64B66B encoded data
#define XSP4_AURORA_CONT_LOOPBACK_NEAR_PMA	2			//!< Near-End loopback after the PMA of the serialised data 
#define XSP4_AURORA_CONT_LOOPBACK_FAR_PMA	4			//!< Loopback at the far end tranceiver of the 66Bit data with the TX drive clocked from teh RX clock
#define XSP4_AURORA_CONT_LOOPBACK_FAR_PCS	6 			//!< Loopback at the far end tranceiver of the extracted 64 bit data. Requires clock correction ,which should be OK in this case.

/**
@}
@defgroup XSP4_ADC_CONTROL   Control bits for XSPRESS4 ADC Board features.
@ingroup XSP3_MACROS
@{

*/
//! [XSP4_ADC_CONTROL_CODE]
#define XSP4_ADC_CONT_SPI_ADDR(x)	((x)&0x1F)
#define XSP4_ADC_CONT_NSYNC_OUT 	(1<<8)
#define XSP4_ADC_CONT_DITHER 		(1<<9)
#define XSP4_ADC_CONT_CLK_SEL_MP	(1<<10)			//!< ADC board clock sellect to use clock from mid-plane
#define XSP4_ADC_CONT_PSU_ENB		(1<<31)			//!< PSU Enable	Enable PSU to ADC board.
//! [XSP4_ADC_CONTROL_CODE]
/**
@}
@defgroup XSP3M_ADC_CONTROL   Control bits for XSPRESS3Mini ADC Board features.
@ingroup XSP3_MACROS
@{
*/
//! [XSP3M_ADC_CONTROL_CODE]
#define XSP3M_ADC_CONT_CLK_SEL  	(1<<7)			//!< Select LMK61E2 Clock generator.
#define XSP3M_ADC_CONT_SHUTDOWN(x)	(((x)&3)<<16)	//!< Shutdown signals for ADC channels 0 and 1
	

#define XSP3M_ADC_CONT_USER_NOE(x)	(((x)&3)<<12)	//!< Output Disable for full strength output drive, leave 50 ohm terminated drivers enables
#define XSP3M_ADC_CONT_USER_TERM(x) (((x)&3)<<28)	//!< Enable 50 ohm termination on User Inputs 0 and 1

#define XSP3M_ADC_CONT_GET_USER_NOE(x)	(((x)>>12)&3)	//!< Output Disable for full strength output drive, leave 50 ohm terminated drivers enables
#define XSP3M_ADC_CONT_GET_USER_TERM(x) (((x)>>28)&3)	//!< Enable 50 ohm termination on User Inputs 0 and 1


#define XSP3M_ADC_CONT_IGNORE_OVER_TEMP	(1<<30)		//!< Ignore over temperature shutdown of ADCs
#define XSP3M_ADC_CONT_PSU_ENB		(1<<31)			//!< PSU Enable	Enable PSU to ADC board.
//! [XSP3M_ADC_CONTROL_CODE]

/**
@}
	@defgroup XSP4_AURORA_STATUS   Status bits for Board to Board Aurora Links. Used to undestand 16 bit unpacked data returned by {@link xsp4_get_aurora_status}
	@ingroup XSP3_MACROS
	@{
*/
#define XSP4_AUROA_STAT_HARD_ERR		(1<<0)			//!< Hard error
#define XSP4_AUROA_STAT_SOFT_ERR		(1<<1)			//!< Soft Error
#define XSP4_AUROA_STAT_CHANNEL_UP		(1<<2)			//!< Channel up
#define XSP4_AUROA_STAT_LANE_UP			(1<<3)			//!< Lane up (single bit in this case, 1 lane per board to board link)
#define XSP4_AUROA_STAT_LINK_RESET		(1<<4)			//!< Link reset, caused by hotplug logic.
#define XSP4_AUROA_STAT_GT_PLL_LOCK		(1<<5)			//!< Channel PLL lock due to relative low bit rate (3.125 GBit/s)

#define XSP4_NUM_BRD2BRD					6			//!< Number of Board to Board links
/**
 @}
	@defgroup XSP4_ADC_BRD_STATUS   Status bits for Board to Board Aurora Links. Used to undestand 16 bit unpacked data returned by {@link xsp4_get_aurora_status}
	@ingroup XSP3_MACROS
	@{
*/
//! [XSP4_ADC_BRD_STATUS_CODE]
#define XSP4_ADC_BRD_STAT_ADC_SENSE		(1<<0)		//!< Pulled low when ADC board is plugged in
#define XSP4_ADC_BRD_STAT_PSU_OK		(1<<1)		//!< Power supply OK. Pulled low when LM75 detects over temperature
#define XSP4_ADC_BRD_STAT_MASTER_CLK_LD	(1<<2)		//!< Lock Detect from midplane clock cleaner/distributor chip.
//! [XSP4_ADC_BRD_STATUS_CODE]

/**
@}
@defgroup XSP4_GLOB_REG_ADC_CLK Global register in ADC Clock domain 
@ingroup XSP3_MACROS
@{
*/

#define XSP4_GLOB_BOARD_NUM			0
#define XSP4_GLOB_TIMING_A			2
#define XSP4_GLOB_TIMING_FIXED		3

#define XSP4_GLOB_SCOPE_CONT		4
#define XSP4_GLOB_SCOPE_CHAN_SEL0	5
#define XSP4_GLOB_SCOPE_CHAN_SEL1	6
#define XSP4_GLOB_SCOPE_SRC_SEL0	7
#define XSP4_GLOB_SCOPE_SRC_SEL1	8
#define XSP4_GLOB_SCOPE_ALTERNATE0	9
#define XSP4_GLOB_SCOPE_ALTERNATE1	10
#define XSP4_GLOB_SCOPE_NWORDS		11
#define XSP4_GLOB_DATA_PATH_CONT	12
#define XSP4_GLOB_DATA_MUX_CONT		13

#define XSP4_GLOB_ITFG_FRAME_LEN	16
#define XSP4_GLOB_ITFG_NUM_FRAMES	17
#define XSP4_GLOB_ITFG_MARKER		18

#define XSP4_GLOB_GLOB_RST_GEN_A	20
#define XSP4_GLOB_GLOB_RST_GEN_B	21

#define XSP4_GLOB_STATUS_A			32

#define XSP4_GLOB_TIMING_STATUS_A	34
#define XSP4_GLOB_FEATURES_A		37
#define XSP4_FEATURES_NUM			3


/* @} */
/**
	@defgroup XSP4_SCOPE_REGISTERS   XSPRESS3-Mini and XSPRESS4 scope mode control registers
	@ingroup XSP4_GLOB_REG_ADC_CLK
	@{
*/

#define XSP4_GSCOPE_ENB_SCOPE	1
#define XSP4_GSCOPE_BYTE_SWAP	(1<<1)
#define XSP4_GSCOPE_DELAY_START	(1<<2)
#define XSP4_GSCOPE_EXTRA_DELAY	(1<<3)


#define XSP4_GSCOPE_NUM_STREAMS(code)	(((code)&7)<<4)
#define XSP4_GSCOPE_NUM_STREAMS_MASK	((7)<<4)
#define XSP4_GSCOPE_NUM_STREAMS4		0
#define XSP4_GSCOPE_NUM_STREAMS8		1
#define XSP4_GSCOPE_NUM_STREAMS16		2

#define XSP4_GSCOPE_GET_NUM_STREAMS(x)	(((x)>>4)&7)

#define XSP3M_GSCOPE_NUM_STREAMS_MASK	((7)<<4)
#define XSP3M_GSCOPE_NUM_STREAMS1		0
#define XSP3M_GSCOPE_NUM_STREAMS2		1
#define XSP3M_GSCOPE_NUM_STREAMS3		2
#define XSP3M_GSCOPE_NUM_STREAMS4		3
#define XSP3M_GSCOPE_NUM_STREAMS6		4


#define XSP4_GSCOPE_PLAYBACK16			(1<<8)		//!< Enable 16 playback streams.
#define XSP4_GSCOPE_PLAYBACK_RADIAL		(1<<12)		//!< Enable playback from Radial Run signal
#define XSP4_GSCOPE_PLAYBACK_COUNT_ENB	(1<<14)		//!< Enable playback from CountEnbale signal (typically from LEMO Veto signals in Xspress3 Mini)


#define XSP4_GSCOPE_CHAN_SEL(s,x)	(((x)&0xF)<<4*((s)))
#define XSP4_GSCOPE_CHAN_SEL_GET(s,x0,x1) (((s)>=8)?(((x1)>>4*((s)-8))&0xF):(((x0)>>4*((s)))&0xF))


#define XSP4_GSCOPE_SRC_SEL(s,x)	(((x)&0xF)<<4*(s))
#define XSP4_GSCOPE_SRC_SEL_GET(s,x0,x1) (((s)>=8)?(((x1)>>4*((s)-8))&0xF):(((x0)>>4*((s)))&0xF))

#define XSP4_GSCOPE_ALT(s,x)		(((x)&0xF)<<4*(s))
#define XSP4_GSCOPE_ALTERNATE_GET(s,x0,x1) (((s)>=8)?(((x1)>>4*((s)-8))&0xF):(((x0)>>4*((s)))&0xF))
/** @}
*/


/**
@defgroup XSP4_SCOPE_SOURCES   XSPRESS4 scope mode data source select values.
@ingroup XSP4_SCOPE_REGISTERS
This data is assembled nibble at a time into the 32 bit hardware registers XSP4_GLOB_SCOPE_SRC_SEL0 and XSP4_GLOB_SCOPE_SRC_SEL1 using XSP4_GSCOPE_SRC_SEL
@{
[XSP4_SCOPE_SOURCE_CODE]
*/

/* Streams 0..5 are 10 input mux with full detail */
#define XSP4_SCOPE_SEL0TO5_INP				0	//!< Select ADC input or reset detector output (see alternates)
#define XSP4_SCOPE_SEL0TO5_TRIG_B_OUT		1	//!< Select Trigger-B Output.
#define XSP4_SCOPE_SEL0TO5_SERVO_OUT		2	//!< Select Servo Output.
#define XSP4_SCOPE_SEL0TO5_DIG_OUT			3	//!< Select output event size.
#define XSP4_SCOPE_SEL0TO5_TRIG_B_DIFF1		4	//!< Select trigger-B 1st differential or fast 1st differential
#define XSP4_SCOPE_SEL0TO5_TRIG_B_DIFF2		5	//!< Select trigger-B 2nd differential
#define XSP4_SCOPE_SEL0TO5_SERVO_GRAD_ERR	6	//!< Select gradient error feedback to servo
#define XSP4_SCOPE_SEL0TO5_SERVO_GRAD_EST	7	//!< Select gradient estimate calculated by servo
#define XSP4_SCOPE_SEL0TO5_RESET_DET		8	//!< Select the reset detector differential
#define XSP4_SCOPE_SEL0TO5_TRIG_C_DIFF1		9	//!< Select trigger-C 1st differential
#define XSP4_SCOPE_SEL0TO5_GLITCH			10	//!< Select the glitch detector differential
#define XSP4_SCOPE_SEL0TO5_XTK_OUT_CORR		11	//!< Crosstalk correction block with correction.

/* Streams 6..11 are 4 input mux. Input, TriggerB and Servo are available in up to 14 streams as these are used for the whole detector auto calibration */
#define XSP4_SCOPE_SEL6TO11_INP				0	//!< Select ADC input or reset detector output (see alternates)
#define XSP4_SCOPE_SEL6TO11_TRIG_B_OUT		1	//!< Select Trigger-B Output.
#define XSP4_SCOPE_SEL6TO11_SERVO_OUT		2	//!< Select Servo Output.
#define XSP4_SCOPE_SEL6TO11_DIG_OUT			3	//!< Select output event size.
#define XSP4_SCOPE_SEL6TO11_XTK_EVENT		4	//!< Crosstalk Event list monitor
#define XSP4_SCOPE_SEL6TO11_XTK_OUT_RAW		5	//!< Crosstalk block output if not corrected
#define XSP4_SCOPE_SEL6TO11_XTK_OUT_CORR	6	//!< Crosstalk correction block with correction.
#define XSP4_SCOPE_SEL6TO11_XTK_CORR		7	//!< Correction to be subtracted by crosstalk correction block
#define XSP4_SCOPE_SEL6TO11_XTK_TIMESTAMP	8	//!< Time stamp
#define XSP4_SCOPE_SEL6TO11_XTK_EVENTS_OUT 11	//!< Xtk event list output from current channel
#define XSP4_SCOPE_SEL6TO11_XTK_NOISE_EVENTS 12	//!< Calculation of Noise events
#define XSP4_SCOPE_SEL6TO11_CSHR_MEASURE 	13	//!< Measurement of Charge shared signals on neighbouring channels.
#define XSP4_SCOPE_SEL6TO11_CSHR_MARKERS 	14	//!< Marking of event output as charge shared.


#define XSP4_SCOPE_CHAN_CSHR				15	//!< Special cahnnel number reads datafrom Charge sharing data paths.

/* Streams 12..13 are 4 input mux. Input, TriggerB and Servo and are used in the 7 out of 8 or 14 out of 16 analogue modes, or can be set using the XSP4_SCOPE_SEL_DIG below */
#define XSP4_SCOPE_SEL12TO13_INP		0	//!< Select ADC input or reset detector output (see alternates)
#define XSP4_SCOPE_SEL12TO13_TRIG_B_OUT	1	//!< Select Trigger-B Output.
#define XSP4_SCOPE_SEL12TO13_SERVO_OUT	2	//!< Select Servo Output.
#define XSP4_SCOPE_SEL12TO13_DIG_OUT	3	//!< Select output event size.

#define XSP4_SCOPE_SEL_DIG_5BIT	8			//!<  Select digital data when using 12 streams plus 5 digital per stream
#define XSP4_SCOPE_SEL_DIG_4BIT	9			//!<  Select digital data when using 12 streams plus 4 digital per stream + 1 all digital
#define XSP4_SCOPE_SEL_DIG_2BIT	10			//!<  Select digital data when using 12 streams plus 2 digital per stream 
#define XSP4_SCOPE_SEL_DIG_ALL	11			//!<  Selection for various All digital interpretations.

#define XSP4_SCOPE_SEL14_FINE_TIME			15	//!< Fine time data for channels 3..0

#define XSP4_SCOPE_SEL15_ALL_RESETS			11	//!< Digital data from All channel resets
#define XSP4_SCOPE_SEL15_RESET_DET_RESETS	12	//!< Digital data from All channel Reset Detector Det resets
#define XSP4_SCOPE_SEL15_RAW_EVENT			13	//!< Digital data from All channels raw event signal
#define XSP4_SCOPE_SEL15_RESET_DET_AND_EVENT 14	//!< Digital data from All channel Reset Detector Det resets and raw event
#define XSP4_SCOPE_SEL15_FINE_TIME			15	//!< Fine time data for channels 7..4

/**
[XSP4_SCOPE_SOURCE_CODE]
* @}
*/

/**
@defgroup XSP3M_SCOPE_SOURCES   XSPRESS3-Mini scope mode data source select values.
@ingroup XSP4_SCOPE_REGISTERS
This data is assembled nibble at a time into the 32 bit hardware registers XSP4_GLOB_SCOPE_SRC_SEL0 and XSP4_GLOB_SCOPE_SRC_SEL1 using XSP4_GSCOPE_SRC_SEL
@{
*/
//! [XSP3M_SCOPE_SOURCE_CODE]

/* Streams 0..4 are 10 input mux with full detail */
#define XSP3M_SCOPE_SEL0TO4_INP				0	//!< Select ADC input or reset detector output (see alternates)
#define XSP3M_SCOPE_SEL0TO4_TRIG_B_OUT		1	//!< Select Trigger-B Output.
#define XSP3M_SCOPE_SEL0TO4_SERVO_OUT		2	//!< Select Servo Output.
#define XSP3M_SCOPE_SEL0TO4_DIG_OUT			3	//!< Select output event size.
#define XSP3M_SCOPE_SEL0TO4_TRIG_B_DIFF1	4	//!< Select trigger-B 1st differential or fast 1st differential
#define XSP3M_SCOPE_SEL0TO4_TRIG_B_DIFF2	5	//!< Select trigger-B 2nd differential
#define XSP3M_SCOPE_SEL0TO4_SERVO_GRAD_ERR	6	//!< Select gradient error feedback to servo
#define XSP3M_SCOPE_SEL0TO4_SERVO_GRAD_EST	7	//!< Select gradient estimate calculated by servo
#define XSP3M_SCOPE_SEL0TO4_RESET_DET		8	//!< Select the reset detector differential
#define XSP3M_SCOPE_SEL0TO4_TRIG_C_DIFF1	9	//!< Select trigger-C 1st differential
#define XSP3M_SCOPE_SEL0TO4_GLITCH			10	//!< Select the glitch detector differential

#define XSP3M_SCOPE_SEL5_INP            	0	//!<  Select analog (input) on last stream.
#define XSP3M_SCOPE_SEL5_DIG_5BIT			8	//!<  Select digital data when using 3 streams plus 5 digital per stream, total 2 or 4 streams
#define XSP3M_SCOPE_SEL5_DIG_3BIT			9	//!<  Select digital data when using 5 streams plus 3 digital per stream, total 6 streams
#define XSP3M_SCOPE_SEL5_FINE_TIME			15	//!<  4 Bit digital from  streams 0 and 1  and Fine time data for channels 0..1

/**
* @}
*/
//! [XSP3M_SCOPE_SOURCE_CODE]

/**
@}
@defgroup XSP4_ZYNQ_REGS Register in Zynq PL on XSPRESS$ enclustra controller 
@ingroup XSP3_MACROS
@{
*/
#define XSP4_ZYNQ_LOCAL_REVISION 0  		//!< Local Revision number Register on Zynq Bus XSP4_BUS_LOCAL
#define XSP4_ZYNQ_LOCAL_C2C_CSR 2  			//!< AXI Chip 2 Chip Link Status and Control Register


#define XSP4_ZYNQ_LOCAL_CONF_CONT ((1<<10)|0)  		//!< Local Configuration Control Register on Zynq Bus XSP4_BUS_LOCAL
#define XSP4_ZYNQ_LOCAL_CONF_STAT ((1<<10)|2)  		//!< Local Configuration Status Register on Zynq Bus XSP4_BUS_LOCAL

#define XSP4_ZYNQ_LOCAL_C2C_GOOD 		1  		//!< AXI Chip 2 Chip Link Status Good Link status
#define XSP4_ZYNQ_LOCAL_C2C_MBIT_ERR 	2	  		//!< AXI Chip 2 Chip Link Status Multibit error
#define XSP4_ZYNQ_LOCAL_C2C_LINK_ERR 	4	  		//!< AXI Chip 2 Chip Link Status Link error

#define XSP4_ZYNQ_LOCAL_C2C_LINK_RESET 	0x80	  	//!< Write to register to Reset the AXI Chip 2 Chip Link 

#define XSP4_ZYNC_CONF_CONT_V7_PRG 		(1<<0)			//!< Virtex-7 Prog Pin.
#define XSP4_ZYNC_CONF_CONT_V7_INIT_B 	(1<<1)			//!< Virtex-7 INIT_B pin, write 1 => Z for normal operation.
#define XSP4_ZYNC_CONF_CONT_FSEL(x)		(((x)&3)<<8) 	//!< Virtex-7 Firmware Flash select 0..3 (note bit reversal if probing FSEL_0..3
#define XSP4_ZYNC_CONF_CONT_F_CLK_SEL 	(1<<4)			//!< Virtex-7 Flash clock select. 0=> FCLK_1 => IO_L3N_T0_DQS_EMCCLK_14, 1=>F_CLK_0=>CCLK 

#define XSP4_ZYNC_CONF_CONT_FLASH_RST 	(1<<5)			//!< Reset to Flash memory. Write 1 to reset
#define XSP4_ZYNC_CONF_CONT_V7_RST		(1<<6)  		//!< Reset to Virtex-7 Write 1 to reset.
#define XSP4_ZYNC_CONF_CONT_EN_1V0	 	(1<<7) 			//!< Power enable to Virtex-7. Write (or leave) 1 here for normal operation.

#define XSP4_ZYNC_CONF_CONT_LED_BURST	 (1<<15) 		//!< Writing a rising edge here causes a burst of flahes on the LED.

#define XSP4_ZYNC_CONF_STAT_DONE 		(1<<0)			//!< Configuration done bit.

/**
@}
*/

/**
@defgroup XSP4_ADC_SPI_REGS Registers on SPI busses on ADC board 
@ingroup XSP3_MACROS
@{
*/
//! [XSP4_ADC_SPI_REGS_CODE]
#define XSP4_SPI_ADDR_REGION_DAC	0x0		//!< Code to offset address to identify switches
#define XSP4_SPI_ADDR_REGION_SW		0x1		//!< Code to offset address to identify switches
#define XSP4_SPI_ADDR_REGION_CPLD	0x2		//!< Code to offset address to identify Registers in CPLD

#define XSP4_SPI_ADDR_ASSEMBLE(chan,region)	((((region)&3)<<8)|((chan)&0xFF))	//!< Assemble channel and region for xspress4 protocol.

#define XSP4_SPI_ADDR_REGION_MASK	0x300		//!< Mask to find codes for DACS, Switches or CPLD registers
#define XSP4_SPI_ADDR_CHAN_MASK		0x0FF		//!< Mask to find channel number
#define XSP4_SPI_ADDR_GET_CHAN(x)	((x)&0xFF)			//!< Extract channel number from, address token
#define XSP4_SPI_ADDR_GET_REGION(x)	(((x)&0x300)>>8)	//!< Extract region number from, address token

#define XSP4_SPI_CHAN_PSU_CONTROL  	0		//!< Logical SPI device address token used to identify the PSU control SPI register in the ADC board CPLD
#define XSP4_SPI_CHAN_PSU_REVISION 	1		//!< SPI device address token used to identify the PSU revision SPI register in the ADC board CPLD
#define XSP4_SPI_ADDR_PSU_CONTROL  	XSP4_SPI_ADDR_ASSEMBLE(XSP4_SPI_CHAN_PSU_CONTROL, XSP4_SPI_ADDR_REGION_CPLD)	//!< SPI device address token used to identify the PSU control SPI register in the ADC board CPLD
#define XSP4_SPI_ADDR_PSU_REVISION 	XSP4_SPI_ADDR_ASSEMBLE(XSP4_SPI_CHAN_PSU_REVISION, XSP4_SPI_ADDR_REGION_CPLD)	//!< SPI device address token used to identify the PSU revision SPI register in the ADC board CPLD
#define XSP4_SPI_ADDR_MAX XSP4_SPI_ADDR_PSU_REVISION 				//!< Maximum Valid SPI address

#define XSP4_SPI_PHYS_ADDR_PSU_CONTROL  30		//!< SPI address used to identify the PSU control SPI register in the ADC board CPLD
#define XSP4_SPI_PHYS_ADDR_PSU_REVISION 14			//!< SPI address used to identify the PSU revision SPI register in the ADC board CPLD

#define XSP4_SPI_NUM_CHAN			32			//!< Number of SPI select slots on SPI bus, only 22 of 32 are used.
#define XSP4_SPI_NUM_MID_PLANE		4			//!< Nunber of devices on the Timing SPI bus.

#define XSP4_SPI_CHAN_BUS 		32766			//!< spidev bus number of ADC board Offset and gain SPI bus in XSPRESS4.
#define XSP4_SPI_TIMING_BUS 	32764			//!< spidev bus number of Timing SPI bus in XSPRESS4.

#define XSP4_PSU_CONT_IGNORE_OVER_TEMP	(1<<15)			//!<	IGNORE Over temperature on ADC board (for debugging only)
#define XSP4_PSU_CONT_SHUTDOWN(x)		((x)&0x3FF)		//!<	Bit mask to shutdown ADC for channels 0..9.
//! [XSP4_ADC_SPI_REGS_CODE]

//! [XSP4_MP_SPI_REGS_CODE]
#define XSP4_SPI_MID_PLANE_CHIP_TERM	2		//! < Chip number for Midplane SPI bus for termination control
#define XSP4_SPI_MID_PLANE_CHIP_LED		3		//! < Chip number for Midplane SPI bus for PSU LEDs
#define XSP4_PCA9502_ADDR_IODIR			0xA		//!< Data direection register in PCA9502
#define XSP4_PCA9502_ADDR_DATA			0xB		//!< Data register for PCA9502			

#define XSP4_DIGIO_USER_TERM(x)			(((x)&0xF)<<4)		//!< Enable bits for the 4 user input terminations.
#define XSP4_DIGIO_USER_NOE(x)			(((x)&0xF)<<0)		//!< Disable bits for full drive strength (0 => full drive, 1 => 50 Ohm termination) for 4 user outputs.

#define XSP4_DIGIO_GET_USER_TERM(x)		(((x)>>4)&0xF)		//!< Enable bits for the 4 user input terminations.
#define XSP4_DIGIO_GET_USER_NOE(x)		(((x)>>0)&0xF)		//!< Disable bits for full drive strength (0 => full drive, 1 => 50 Ohm termination) for 4 user outputs.

//! [XSP4_MP_SPI_REGS_CODE]

/** 
@}
*/

#define XSP4_PB_MAX_STREAMS 16
#define XSP4_SCOPE_MAX_STREAMS 16

#define XSP3M_STATUS_BRAM_CLEARING (1<<16)

#define XSP3M_HW_NUM_SCALERS 8

int xsp4_write_glob_reg(int path, int card, int offset, int size, u_int32_t* value);
int xsp4_read_glob_reg(int path, int card, int offset, int size, u_int32_t *value);
int xsp4_setup_10g_tpg(int path, int card, int enable, int nqwords, int nframes);
int xsp4_system_start_count_enb(int path, int card, int count_enb, int pb_num_t, int scope_num_t);


/**
@defgroup XSP4_XADC_OFFSETS Logical numbering of XADC system monitor fucntions for Xspress3Mini adn Xspress4 
@ingroup XSP3_MACROS
@{
*/
#define XSP3_XADC_ZYNQ_TEMP			0
#define XSP3_XADC_VIRTEX7_TEMP		1
#define XSP3_XADC_ZYNQ_VCC_INT		2
#define XSP3_XADC_ZYNQ_VCC_AUX		3
#define XSP3_XADC_ZYNQ_VCC_BRAM		4
#define XSP3_XADC_ZYNQ_VCC_PINT		5
#define XSP3_XADC_ZYNQ_VCC_PAUX		6
#define XSP3_XADC_ZYNQ_VCCO_DDR		7
#define XSP3_XADC_VIRTEX7_VCC_INT	8
#define XSP3_XADC_VIRTEX7_VCC_AUX	9
#define XSP3_XADC_VIRTEX7_VCC_BRAM	10

#define XSP3_XADC_MAX				10
#define XSP3_XADC_NUM      			(XSP3_XADC_MAX+1)
/**
@}
*/

#define XSP4_GTH2_TX_DRIVE_MODE		0x0019


#endif /* XSPRESS3_H_ */

