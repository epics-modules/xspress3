
/*****************************************************************************
*
*	Structure / typedef to define XSPRESS3 Data stored in Shared memory
*
*****************************************************************************/
/**
@addtogroup scope_module 

# Introduction
The XSPRESS systems provide scope mode capabilities which allow raw ADC data and data from various points in the processing stream to be recorded for of order 64M samples.
The "analogue" data can have some digital bits associated with it. These are typically the detector reset signal at the input and then the derived event trigger signals for processed data.
The firmware contains a series of scope mode multiplexers which select which channels the data comes from and from which points within the channels. The streams can be set to take data from 
multiple points within one channel or to take data from 1 point across multiple channels, or some mixture of this.
@n
Once the scope mode multiplexer have been set up, the system, is started using {@link xsp3_system_start()}. This starts the system and records the data into DRAM on the FEM-1, FEM-2 or ZYNQ.
The scope mode DMAs are allowed to complete using {@link xsp3_scope_wait}
The scope mode data is then transferred back to the PC memory and stored in RAM under /dev/shm/xsp3_scope<nn> using {@link xsp3_read_scope_data()}.
Once the data is in this data module it can be accessed.

The layout of the data is chosen to suit the DMAs in the firmware. For XSPRESS3 it is in 2 blocks to match the use of 2 DMA streams. For XSPRESS4 it is in 1 block
The functions here {@link xsp3_scope_mod_get_nstreams()}, {@link xsp3_scope_mod_get_ptr()} and {@link xsp3_scope_mod_get_inc() } should be used to determine pointers to 16 bit values to access it.
# Linking to the data module.
For software which has full direct control of XSPRESS3/4 using the path returned by {@link xsp3_config()}, the scope mode data module is usually created as part of the call to xsp3_config.
The pointer to the scope mode data is the found from {@link xsp3_scope_get_module()}.

However, since only 1 process can have full control of the XSPRESS3/4 and own the UDP ports to run the histogramming threads, it is possible for other processes to just access 
scope mode data from the module.
In this case the scope mode module is linked to using the _os_link function from libimg_mod which fronts access to shm_open and mmap.
This is used in imgd.

@verbatim
int xspress3_find_modules(int *first_valid)
{
	int i;
	XSP3ScopeModule *mod=NULL;
	u_int16_t type_lang=0, attr_rev = 0;
	int err;
	char mname[20];
	char *name;
	int n=0;
	mh_com *mod_head;

//	*first_valid= -1; // Rely on sys_num initialised to -1 and then leave it alone on subsequent calls.

	for (i=0; i<XSP3_MAX_PATH; i++)
	{
		sprintf(mname, "xsp3_scope%d", i);
		name = mname;
		if (xspress3_sys[i].scope_mod == NULL && (err=_os_link(&name, &mod_head, (void **)&(xspress3_sys[i].scope_mod), &type_lang, &attr_rev)) != 0)
		{
			if (i == 0)
				printf("Cannot link to xspress scope module '%s', err=%d\n", name, err);
		}
		else
		{
			printf("System number %d, layout=%d\n", i, xspress3_sys[i].scope_mod->head.layout);
			if (*first_valid == -1)
				*first_valid = i;
			n++;
		}
	}
	if (n == 0)
		printf("Cannot link to any xspress3 scope modules\n", name, err);

	return n;
}
	
@endverbatim
The process with direct control of XSPRSS3/4 then has to run the scope mode burst and transfer the data back to the module.
@verbatim
int xspress3_run_scope(int path, int card)
{
	int rc;
	int do_read = 1;
	int count_enb = 1;
	int scope_setup = 1;
	int save_flags;

	if (scope_setup)
	{
		if ((rc=xsp3_scope_settings_from_mod(path))<0)
		{
			printf("! ERROR updating scope settings from module: %s\n", xsp3_get_error_message());
			return rc;
		}
	}
	if (do_read)
	{
		save_flags = xsp3_get_run_flags(path);
		xsp3_set_run_flags(path, save_flags | XSP3_RUN_FLAGS_SCOPE);
	}
	rc = xsp3_system_start_count_enb(path, card, count_enb, 0, 0);
	if (rc < 0)
	{
		printf("! ERROR starting system: %s\n", xsp3_get_error_message());
		return rc;
	}

	if (do_read)
	{
		rc = xsp3_scope_wait(path, card);
		if (rc < 0) {
			printf("! ERROR waiting for scope DMAs %s\n", xsp3_get_error_message());
		}
	
		rc = xsp3_read_scope_data(path, card);
		if (rc < 0) {
			printf("! ERROR reading scope data %s\n", xsp3_get_error_message());
		}
		xsp3_set_run_flags(path, save_flags);
	}
	return rc;
}
@endverbatim
# Controlling the stream multiplexers.	
The scope mode headers include copies of the control registers which configure the multiplexers. 
These can be set by a GUI (such as imgd) in the data module and then copied to the hardware using {@link xsp3_scope_settings_from_mod()} 
called in the process with direct control on XSPRESS3/4 before calling {@link xsp3_system_start()}.
In other cases the auto-calibration software will set these multiplexers as it requires and puts a copy into the module using {@link xsp3_scope_settings_to_mod()}.
Whenever the scope mode data is displayed these fields in the data module should be used to interpret the data.
The function {@link xsp3_scope_stream_details} decodes these fields to match the requirements of the various generations of system and is hence the preferred way to access this information.
## XSPRESS3 streams.
XSPRESS3 provides a fixed 6 scope streams. Usually stream 0 is used for digital data and 1..5 are for various "analogue" data streams.
## XSPRESS3Mini Scope Streams
Scope mode in xspress3Mini is rather complicated to make best use of the limited amount of memory.
The system can be programmed to use 1, 2, 3, 4, or 6 scope streams.
### 1 stream mode
1 stream mode is intended to be used to record just ADC data for later processing in software or by playback for a single channel.
### 2 stream mode
2 stream mode can be used to record just ADC data from 2 channels for later processing.
It is also used for automatic calibration of a single channel system.
### 3 stream mode
3 stream mode is intended for automatic calibration of 2 channel systems with 2 analogue streams and 1 combined digital stream.
### 4 stream mode
4 stream mode is for general interactive debug where 5 bits of digital data are required. With 4 streams, 3 streams (0..2) will be analogue with 
the last stream carrying 5 bit digital data from each of the 3 analogue streams.
### 6 stream mode
6 stream mode is for general interactive debug with 3 bits of digital data. With 6 streams, 5 will be analogue with the last stream carrying 3 bit digital data from each of the 5 analogue streams.

## XSPRESS4 Scope Streams
Scope mode in xspress4 is rather complicated to make best use of the large number of streams and digital bits.
Normally it takes 16 streams, but this can be switched to 8 or 4 to make the memory cover more time.

### 4 stream mode
    Streams 0..2 are some form of analog  @n
    Stream 3 goes comes for VHDL stream 12 and can be analogue OR 5 or 4  of digital from 0..2 OR "all digital" @n
    Currently there is no advantage to use 4 bit, though could multiplex in 4 global digital signals. @n
### 8 Stream
    It is possible to program modes which don't keep consistent numbers of digital bits from each stream. The software should not allow this and will not display it correctly.
	Streams 6 and 7 come from VHDL streams 12 and 13, so these channel select, source select and alternates nibbles must be used.
    Reasonable modes are: @n
    6 analogue + 32 bit dig = 2* 5 bit + 2 global on 6 and 7. @n
    6 analogue + 32 bit dig = 2 bit digital on 7 + 16 bit all digital on 6 @n
    7 analogue + 16 bit digital = 2 bit digital on 7 @n
    7 analogue + 16 bit digital = all digital on 7 @n
    8 analogue , no digital @n
### 16 stream mode
	12 analogue on (0..11)  + 5 bit digital on 12..15 @n
	12 analogue on (0..11)  + 4 bit digital on 12..14, all digital on 15 @n
	14 analogue on (0..11)	+ 2 bit digital on 14..15 @n
For fine time calibration. @n
	8 analogue on (0..7) @n
	8..11 unused @n
	12..13 4 bit digital from 0..7 @n
	14..15 fine_time as extra info from 0..7 @n


*/


#ifndef XSP3_SCOPE_MODULE_INCLUDE
#define XSP3_SCOPE_MODULE_INCLUDE

#include "datamod.h"

#ifdef __cplusplus
extern "C" {
#endif
#undef XSP3_MAX_MODNAME
#define XSP3_MAX_MODNAME 100
#define XSP3_MAX_CARDS 16
#define XSP3_SCOPE_MODULE_MAGIC "xsp3scp"

/**
* @ingroup scope_module.
* Layout of the scope mode data module header for xspress3.
* The layout of the {@link XSP3ScopeModule} and {@link XSP4ScopeModule} are aligned at first so it is usual to read particularly num_cards and num_t.
*/
typedef struct xsp3_scope_data_module
{
	struct xsp3_scope_module_head
	{
		char magic[8];
		int32_t layout;				//!< Module layout to differentiate between xspress3, xspress3-mini and xspress4.  See {@link XSP3_SCOPE_MOD_LAYOUT},  but usually use inquiry functions.
		int32_t num_cards;			//!< Number of FEM cards contained within the module. 	
		int32_t hw_version;			//!< hardware version in case we need to change interpretation of values.
		int total_lwords_per_card;	//!< Total number of LWords used per card, 1/2 this is specified to the scope DMA
		int num_t;					//!< Number of time points to use.
		struct
		{
			u_int32_t chan_sel, src_sel, alternate;	//!< Scope mode registers to be interpreted from xspress3.h
		} card[XSP3_MAX_CARDS];
	} head;
	u_int16_t _x3_data[1];			//!< Beginning of data, but use {@link xsp3_scope_mod_get_ptr()} for access
}	XSP3ScopeModule;

/**
* @ingroup scope_module.
* Layout of the scope mode data module header for xspress4
* The layout of the {@link XSP3ScopeModule} and {@link XSP4ScopeModule} are aligned at first so it is usual to read particularly num_cards and num_t.
*/
typedef struct xsp4_scope_data_module
{
	struct xsp4_scope_module_head
	{
		char magic[8];
		int32_t layout;				//!< Module layout to differentiate between xspress3, xspress3-mini and xspress4.  See {@link XSP3_SCOPE_MOD_LAYOUT},  but usually use inquiry functions.
		int32_t num_cards;			//!< Number of FEM cards contained within module
		int32_t hw_version;			//!< hardware version in case we need to change interpretation of values.
		int total_lwords_per_card;	//!< Total number of LWords used per card when module created
		int num_t;					//!< Number of time points to use.
		int current_lwords_per_card;	//!< This allows a memory layout with smaller scope to use part of the module.
		struct
		{
			u_int32_t control;		//!< XSPRESS4 scope mode control register. See {@link XSP4_SCOPE_REGISTERS}
			u_int32_t chan_sel[2]; 	//!< XSPRESS4 scope mode channel select registers which can be extracted with {@link XSP4_GSCOPE_CHAN_SEL_GET()}
			u_int32_t src_sel[2];   //!< XSPRESS4 scope mode source select registers which can be extracted with {@link XSP4_GSCOPE_SRC_SEL_GET()} and interpreted using {@link XSP4_SCOPE_SOURCES}
			u_int32_t alternate[2];	//!< XSPRESS4 Scope mode alternate registers to be interpreted from {@link XSP4_SCOPE_REGISTERS}
			u_int32_t pad[4];									//! < Padding for future expansion
			int num_chan;
		} card[XSP3_MAX_CARDS];		//!< Settings are set on a per card basis, 0..num_cards-1
	} head;
	u_int16_t _x4_data[1];		//!< Beginning of data, but use {@link xsp3_scope_mod_get_ptr()} for access
}	XSP4ScopeModule;


/**
	@defgroup XSP3_SCOPE_MOD_LAYOUT Layouts of Scope moed data module to suit various hardware/firmware/software.
	@ingroup XSP3_MACROS
	@{
*/
#define XSP3_SCOPE_MOD_LAYOUT_2X3	0		//!< Original 2 blocks of 3 streams interlaved (To suit XC5VF PowerPC DMA) 
#define XSP3_SCOPE_MOD_LAYOUT_DIFFS_TEST	1		//!< 9 blocks each of 1 stream to suit reconstruction of differences data.
#define XSP3_SCOPE_MOD_LAYOUT_1_MINI		8		//!< Single block of 1 x 16 bit for XSPRESS3 mini 
#define XSP3_SCOPE_MOD_LAYOUT_2_MINI		9		//!< Two blocks  of 2 x 16 bit for XSPRESS3 mini  
#define XSP3_SCOPE_MOD_LAYOUT_3_MINI		10		//!< Single block of 3 x 16 bit for XSPRESS3 mini  
#define XSP3_SCOPE_MOD_LAYOUT_4_MINI		11		//!< Two block of 4 x 16 bit for XSPRESS3 mini  
#define XSP3_SCOPE_MOD_LAYOUT_6_MINI		12		//!< Two block of 6 x 16 bit for XSPRESS3 mini  
#define XSP3_SCOPE_MOD_LAYOUT_4				16		//!< Single block of 4 x 16 bit for  interpretation of XSPRESS4 in by 4 mode?
#define XSP3_SCOPE_MOD_LAYOUT_8				17		//!< Single block of 8 x 16 bit for interpretation of XSPRESS4 in by 8 mode?
#define XSP3_SCOPE_MOD_LAYOUT_16			18		//!< Single block of 16 x 16 bit for interpretation of XSPRESS4 in by 16 mode (default)


/**
@}
*/
#define XSP3_SCOPE_POINTS_FIXED (64*1024*1024)		//!< Fixed sized scope mode number of points in XSP3_SCOPE_MOD_LAYOUT_DIFFS_TEST


XSP3ScopeModule *xsp3_scope_mod_create(char *name, int num_cards, int lwords_per_card, mh_com ** mod_head, int layout);
u_int16_t * xsp3_scope_mod_get_ptr(XSP3ScopeModule *mod, int card, int stream);
int xsp3_scope_mod_get_inc(XSP3ScopeModule *mod);
int xsp3_scope_mod_get_nstreams(XSP3ScopeModule *mod); 
int xsp3_scope_mod_max_nstreams(XSP3ScopeModule *mod);
int xsp3_scope_mod_get_nstreams_ana(XSP3ScopeModule *mod, int card, int *firstP, int *numP); 
int xsp3_scope_chan(XSP3ScopeModule *mod, int card, int stream); 
int xsp3_scope_mod_get_nstreams(XSP3ScopeModule *mod); 
int xsp3_scope_get_str_sel(XSP3ScopeModule *mod, int card, int stream); 
int xsp3_scope_set_streams(XSP3ScopeModule *mod, int num_streams); 
int xsp3_scope_calc_num_t(XSP3ScopeModule *mod); 

#define XSP3_SCOPE_UDP_PACKET	8000
#define XSP3_SCOPE_UDP_FRAME	0x100000

#define XSP3_SCOPE_NUM_SRC0		4
#define XSP3_SCOPE_NUM_SRC123	4
#define XSP3_SCOPE_NUM_SRC4		16
#define XSP3_SCOPE_NUM_SRC5		16

#define XSP4_SCOPE_NUM_SRC0	 	 16
#define XSP4_SCOPE_NUM_SRC1	 	 16
#define XSP4_SCOPE_NUM_SRC2TO5	 12
#define XSP4_SCOPE_NUM_SRC6TO11  15
#define XSP4_SCOPE_NUM_SRC12TO15 16

#define XSP4_SCOPE_NUM_SRC_CSHR	12

#define XSP3M_SCOPE_NUM_SRC0  	16
#define XSP3M_SCOPE_NUM_SRC1 	16
#define XSP3M_SCOPE_NUM_SRC2TO4  11
#define XSP3M_SCOPE_NUM_SRC5	16

/**
 * @ingroup scope_module
 * Data Type to describe the data stored in a scope mode stream see {@link xsp3_scope_stream_details()}
 */
typedef enum {
	Xsp3ScopeStream_Error=-1, //!< Error interpreting arguments
	Xsp3ScopeStream_NotPresent=0,  //!< The stream is not present in the current configuration of the data module.
	Xsp3ScopeStream_Unsigned=1,    //!< The data is to be interpreted as 16 bit unsiged "analogue" data.
	Xsp3ScopeStream_Signed=2,      //!< The data is to be interpreted as 16 bit signed "analogue" data.
	Xsp3ScopeStream_AllDigital=3,  //!< The data is digital data extracted from across all channels as should be displayed as up to 16 digital traces.
	Xsp3ScopeStream_AuxDigital=4   //!< The data is digital data which is associated with other streams and is ofter displayed with those streams rather than in its own right.
	} Xsp3ScopeStream;

extern const char *xsp3_scope_name_s0[XSP3_SCOPE_NUM_SRC0];
extern const char *xsp3_scope_name_s123[XSP3_SCOPE_NUM_SRC123];
extern const char *xsp3_scope_name_s4[XSP3_SCOPE_NUM_SRC4];
extern const char *xsp3_scope_name_s5[XSP3_SCOPE_NUM_SRC5];
extern const char *xsp3_scope_alt_names_s0[XSP3_SCOPE_NUM_SRC0][16];
extern const char *xsp3_scope_alt_names_s123[XSP3_SCOPE_NUM_SRC123][16];
extern const char *xsp3_scope_alt_names_s4[XSP3_SCOPE_NUM_SRC4][16];
extern const char *xsp3_scope_alt_names_s5[XSP3_SCOPE_NUM_SRC5][16];


extern const char *xsp4_scope_name_s0[16];
extern const char *xsp4_scope_name_s1[16];
extern const char *xsp4_scope_name_s2to5[16];
extern const char *xsp4_scope_name_s6to11[16];
extern const char *xsp4_scope_name_s12[16];
extern const char *xsp4_scope_name_s13[16];
extern const char *xsp4_scope_name_s14to15[16];
extern const char *xsp4_scope_alt_names_s0[XSP4_SCOPE_NUM_SRC0][16];
extern const char *xsp4_scope_alt_names_s1[XSP4_SCOPE_NUM_SRC1][16];
extern const char *xsp4_scope_alt_names_s2to5[XSP4_SCOPE_NUM_SRC2TO5][16];
extern const char *xsp4_scope_alt_names_s6to11[XSP4_SCOPE_NUM_SRC6TO11][16];
extern const char *xsp4_scope_alt_names_s12to13[XSP4_SCOPE_NUM_SRC12TO15][16];
extern const char *xsp4_scope_cshr_src_name[16];
extern const char *xsp4_scope_cshr_alt_name[XSP4_SCOPE_NUM_SRC_CSHR][16];

extern const char *xsp3m_scope_name_s0[16];
extern const char *xsp3m_scope_name_s1[16];
extern const char *xsp3m_scope_name_s2to4[16];
extern const char *xsp3m_scope_name_s5[16];
extern const char *xsp3m_scope_alt_names_s0[XSP3M_SCOPE_NUM_SRC0][16];
extern const char *xsp3m_scope_alt_names_s1[XSP3M_SCOPE_NUM_SRC1][16];
extern const char *xsp3m_scope_alt_names_s2to4[XSP3M_SCOPE_NUM_SRC2TO4][16];
extern const char *xsp3m_scope_alt_names_s5[XSP4_SCOPE_NUM_SRC12TO15][16];

#define XSP3_SCOPE_FLAGS_SIGNED 0x40000000
#define XSP3_SCOPE_FLAGS_NUM_DIG(x) ((x)&0x3FFFFFFF)
const char *xsp3_scope_stream_name(XSP3ScopeModule *mod, int card, int stream);
int xsp3_scope_stream_flags(XSP3ScopeModule *mod, int card, int stream);
Xsp3ScopeStream xsp3_scope_stream_details(XSP3ScopeModule *mod, int card, int stream, int *num_digP, int *bit_posnP, u_int16_t **dig_ptrP, int *num_extraP, int *extra_posnP, u_int16_t **extra_ptrP);
int xsp3_scope_stream_src_alt(XSP3ScopeModule *mod, int card, int stream, int *srcP, int *altP);
int xsp3_scope_stream_has_event(XSP3ScopeModule *mod, int card, int stream, int *otdP, int *cfdP, int *resetP, int *grP); 



#ifdef __cplusplus
}
#endif

#endif
