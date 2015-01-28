
/*****************************************************************************
*
*	Structure / typedef to define XSPRESS3 Data stored in Shared memory
*
*****************************************************************************/
/*
	Scope mode data

	This data is stored as it comes out of the 10 G Link. Per card there are 2 data regions
*/

#ifndef XSP3_SCOPE_MODULE_INCLUDE
#define XSP3_SCOPE_MODULE_INCLUDE

#include "datamod.h"

#ifdef __cplusplus
extern "C" {
#endif
#undef XSP3_MAX_MODNAME
#define XSP3_MAX_MODNAME 100
#define XSP3_MAX_CARDS 8
#define XSP3_SCOPE_MODULE_MAGIC "xsp3scp"
typedef struct xsp3_scope_data_module
{
	struct xsp3_scope_module_head
	{
		char magic[8];
		int32_t layout;			// Module version in case the layout is revised.
		int32_t num_cards;			// Number of FEM cards contained within
		int32_t hw_version;			// hardware version in case we need to change interpretation of values.
		int total_lwords_per_card;	// Total number of LWords used per card, 1/2 this is specified to the scope DMA
		int num_t;					// Number of time points to use.
		struct
		{
			u_int32_t chan_sel, src_sel, alternate;	// Scope mode registers to be interpreted from xspress3.h
		} card[XSP3_MAX_CARDS];
	} head;
	u_int16_t data[1];
}	XSP3ScopeModule;

/**
	@defgroup XSP3_SCOPE_MOD_LAYOUT Layouts of Scope moed data module to suit various hardware/firmware/software.
	@ingroup XSP£_MACROS
	@{
*/
#define XSP3_SCOPE_MOD_LAYOUT_2X3	0		//!< Original 2 blocks of 3 streams interlaved (To suit XC5VF PowrePC DMA) 
#define XSP3_SCOPE_MOD_LAYOUT_DIFFS_TEST	1		//!< 9 blocks each of 1 stream to suit reconstruction of differences data.


/**
@}
*/
#define XSP3_SCOPE_POINTS_FIXED (64*1024*1024)		//!< Fixed sized scope mode number of points in XSP3_SCOPE_MOD_LAYOUT_DIFFS_TEST


XSP3ScopeModule *xsp3_scope_mod_create(char *name, int num_cards, int lwords_per_card, mh_com ** mod_head, int layout);
u_int16_t * xsp3_scope_mod_get_ptr(XSP3ScopeModule *mod, int card, int stream);
int xsp3_scope_mod_get_inc(XSP3ScopeModule *mod);
int xsp3_scope_mod_get_nstreams(XSP3ScopeModule *mod); 
int xsp3_scope_chan(XSP3ScopeModule *mod, int card, int stream); 
int xsp3_scope_mod_get_nstreams(XSP3ScopeModule *mod); 
int xsp3_scope_get_str_sel(XSP3ScopeModule *mod, int card, int stream); 

#define XSP3_SCOPE_UDP_PACKET	8000
#define XSP3_SCOPE_UDP_FRAME	0x100000

#define XSP3_SCOPE_NUM_SRC0		4
#define XSP3_SCOPE_NUM_SRC123	4
#define XSP3_SCOPE_NUM_SRC45	10
extern const char *xsp3_scope_name_s0[XSP3_SCOPE_NUM_SRC0];
extern const char *xsp3_scope_name_s123[XSP3_SCOPE_NUM_SRC123];
extern const char *xsp3_scope_name_s45[XSP3_SCOPE_NUM_SRC45];
extern const char *xsp3_scope_alt_names_s0[XSP3_SCOPE_NUM_SRC0][16];
extern const char *xsp3_scope_alt_names_s123[XSP3_SCOPE_NUM_SRC123][16];
extern const char *xsp3_scope_alt_names_s45[XSP3_SCOPE_NUM_SRC45][16];
#define XSP3_SCOPE_FLAGS_SIGNED 1
#define XSP3_SCOPE_FLAGS_ADD_DIG 2
const char *xsp3_scope_stream_name(XSP3ScopeModule *mod, int card, int stream);
int xsp3_scope_stream_flags(XSP3ScopeModule *mod, int card, int stream);


#ifdef __cplusplus
}
#endif

#endif
