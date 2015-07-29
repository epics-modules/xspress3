
#ifndef _DATAMOD_H_

#define _DATAMOD_H_ 1
/*****************************************************************************
*
*	Structure / typedef to define images in a data module. 
*
*****************************************************************************/
/* The data type field of the structure can be:-
 	long  - 32 bit or 24 bit on 32 bit boundaries
 	short - 16 bit
 	float - 32 bit floating point
 	double- 64 bit floating point
*/
 
/* The image source is stored and checked by the functions pointed to
	Any new data types must be added here, code should tolerate unknown types
	where possible.
*/

/* The preffered way to display the images can be of the following types:
	2d		- A 2 D image of num_x cols and num_y rows.
	1D_XY	- One or more 1d images, where the first row contains X and the
			  second any subsequent rows contains Y data sets.
*/
#ifndef DISP_2D
#define DISP_2D		1
#define DISP_1D_XY	2
#define DISP_1D_Y	3
#define DATA_LONG	1
#define DATA_SHORT	2
#define DATA_FLOAT	3
#define DATA_DOUBLE	4
#endif

/* The structure contains x y and title strings */
#define MOD_LABLEN 80
#define MOD_TITLEN 80

/* Important note. Since V2.4, Microware have unnecessarily cocked around
 * with the format of data modules. Now, the layout of the header is the
 * same as `mh_exec', ie. the common header + various other fields to do
 * with executable programs (completely useless in a data module, of course).
 * However (and here's the catch), only the first of these extra fields is
 * actually used. This is the `execution entry point', which is the offset
 * of the data. So, rather stupidly, the format looks like this:
 *
 *		struct modhcom			48 bytes	Common module header
 *		execution entry point	4 bytes		Points to the very next word
 *		data								Data area
 *
 * So to make ID work with the new format, I've added the extra field
 * `cocked_up' below. This will break any old binaries that might produce
 * data modules, of course, but what did you expect?
 *
 * Notice that _os_datmod and _os_link calls return separate pointers to
 * the header and the data, and the data pointer is header + 52. So we
 * are stuck with this extra field in all future software. In my code, I
 * use only `struct mod_header', and I ignore MOD_IMAGE, so if the format
 * changes again, my code should survive.
 *													R.W.M.J. Sept. 1994

 * W.I.H 27/1/2014 
 * Adding 2D module type, using top 16 bits of DISP type.
 */
#if defined(USE_GTK) || defined(__LINUX__) || defined(linux)
#include "os9types.h"
#include <sys/types.h>

typedef void mh_com;
typedef void mh_data;

#define MP_OWNER_READ  S_IRUSR
#define MP_OWNER_WRITE S_IWUSR
#define MP_GROUP_READ  S_IRGRP
#define MP_WORLD_READ  S_IROTH
#define MA_REENT 0
#define MT_DATA  0
#define ML_ANY   0
#define mkattrevs(a,b) 0
#define mktypelang(a,b) 0
#else
#include <module.h>
#include <types.h>
#endif


#define IMG_MOD_GET_TYPE(p)  ((((p)->head.disp_type)>>16)&0xFFFF)
#define IMG_MOD_GET_DISP(p)  ((((p)->head.disp_type)>>0)&0xFFFF)
#define IMG_MOD_SET_DISP_TYPE(mtype,disp)   (((mtype)&0xFFFF)<<16 | (disp))
#define IMG_MOD_2D				0
#define IMG_MOD_3D				1

typedef struct
{
	struct mod_header
	{
		int32 data_type; 		/* data type long short etc see above 			*/
		int32 disp_type;		/* Prefered display technique see above			*/
		u_int32 num_x;/* Number of elements in x direction 			*/
		u_int32 num_y;/* Number of elements in y direction			*/
		int32 version;		/* incrementing version for derived images? 	*/
							/* Get a block of data so that it can be read 	*/
#ifdef __i386__
		int32 padding;		/* Padding on i386 makes aligned agree with ia64. I386 and IA64 can meet on the same machine. I386 and PPC cannot meet and byte order is different anyway */
#endif
		double aspect;		/* Aspect ratio of pixels X wid / Y wid (-ve = ignore)*/
		char x_label[MOD_LABLEN+2];
		char y_label[MOD_LABLEN+2];
		char z_label[MOD_LABLEN+2];
		char title[MOD_TITLEN+2];
	} head;
	u_int32 data[1];
}	MOD_IMAGE;


typedef struct
{
	struct mod_header3d
	{
		int32 data_type; 		/* data type long short etc see above 			*/
		int32 disp_type;		/* Prefered display technique see above			*/
		u_int32 num_x;			/* Number of elements in x direction 			*/
		u_int32 num_y;			/* Number of elements in y direction			*/
		u_int32 num_t;
		int32 version;		/* incrementing version for derived images? 	*/
							/* Get a block of data so that it can be read 	*/
#ifdef __i386__
		int32 padding;		/* Padding on i386 makes aligned agree with ia64. I386 and IA64 can meet on the same machine. I386 and PPC cannot meet and byte order is different anyway */
#endif
		double aspect;		/* Aspect ratio of pixels X wid / Y wid (-ve = ignore)*/
		char x_label[MOD_LABLEN+2];
		char y_label[MOD_LABLEN+2];
		char t_label[MOD_LABLEN+2];
		char z_label[MOD_LABLEN+2];
		char title[MOD_TITLEN+2];
		int labels_num, labels_size, labels_spare;		/* For Y lables, future expansion */
		int data_offset;
	} head;
	int labels_offset[1];		/* First label offset if labels_size > 0 */
}	MOD_IMAGE3D;


#ifdef __cplusplus
extern "C" {
#endif

#if defined(__LINUX__) || defined(linux)
int _os_datmod (char *name, int off_t, u_int16 *attr_rev, u_int16 *type_lang, u_int32 perm, void **mod, void **mod_head);
int _os_link(char **namep, void **mod_head, void **mod, u_int16 *type_lang, u_int16 *att_rev);
int _os_link(char **namep, void **mod_head, void **mod, u_int16 *type_lang, u_int16 *att_rev);
int munlink(void *mod_head);
int _os_unlink(void *mod_head);
int datamod_size(void *mod_head);

MOD_IMAGE *id_mkmod ( char *name, int num_x, int num_y, char *x_lab, char *y_lab,  int data_float, void **mod_head);
MOD_IMAGE3D *id_mkmod3d ( char *name, int num_x, int num_y, int num_t, char *x_lab, char *y_lab, char *t_lab, char ** labels, int data_float, mh_com **mod_head);
u_int32 *id_get_ptr(void *mod, int x, int y, int t);

#else
MOD_IMAGE *id_mkmod ( char *name, int num_x, int num_y, char *x_lab, char *y_lab,
			 int data_float, mh_com **mod_head);

MOD_IMAGE3D *id_mkmod3d ( char *name, int num_x, int num_y, int num_t, char *x_lab, char *y_lab, char *t_lab, char ** labels, int data_float, mh_com **mod_head);

u_int32 *id_get_ptr(void *mod, int x, int y, int t);


#endif
#ifdef __cplusplus
}
#endif

#endif
