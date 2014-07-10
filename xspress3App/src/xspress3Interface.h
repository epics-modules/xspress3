#ifndef XSPRESS3INTERFACE_H
 #define XSPRESS3INTERFACE_H

#include "xspress3.h"

int xsp3IF_clocks_setup(int path, int card, int clk_src, int flags, int tp_type);
int xsp3IF_close(int path);
int xsp3IF_config(int ncards, int num_tf, char* baseIPaddress, int basePort, char* baseMACaddress, int nchan, int createmodule, char* modname, int debug, int card_index);
int xsp3IF_config_tcp(char femHostName[][XSP3_MAX_IP_CHARS], int femPort, int card, int chan, int debug);
int xsp3IF_config_udp(int path, int card, char *femMACaddress, char*femIPaddress, int femPort, char* hostIPaddress, int hostPort);
int xsp3IF_config_histogram_udp(int path, int card, char *hostIPaddress, int hostPort, char *femIPaddress, int femPort);
int xsp3IF_config_scaler(int path);
int xsp3IF_format_run(int path, int chan, int aux1_mode, int res_thres, int aux2_cont, int disables, int aux2_mode, int nbits_eng);
int xsp3IF_getDeadtimeCorrectionParameters(int path, int chan, int *flags, double *processDeadTimeAllEventGradient,
                                           double *processDeadTimeAllEventOffset, double *processDeadTimeInWindowOffset, double *processDeadTimeInWindowGradient);
char* 	xsp3IF_get_error_message();
int xsp3IF_get_good_thres(int path, int chan, u_int32_t *good_thres);
int xsp3IF_get_window(int path, int chan, int win, u_int32_t *low, u_int32_t *high);
int xsp3IF_hist_dtc_read4d(int path, double *hist_buff, double *scal_buff, unsigned eng, unsigned aux, unsigned chan, unsigned tf,
                         unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf);
int xsp3IF_histogram_clear(int path, int first_chan, int num_chan, int first_frame, int num_frames);
int xsp3IF_histogram_continue(int path, int card);
int xsp3IF_histogram_is_busy(int path, int chan);
int xsp3IF_histogram_is_any_busy(int path);
int xsp3IF_histogram_read4d(int path, u_int32_t *buffer, unsigned eng, unsigned aux, unsigned chan, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf);
int xsp3IF_histogram_start(int path, int card);
int xsp3IF_histogram_start_count_enb(int path, int card, int count_enb);
int xsp3IF_histogram_stop(int path, int card);
int xsp3IF_restore_settings(int path, char *dir_name, int force_mismatch);
int xsp3IF_save_settings(int path, char *dir_name);
int xsp3IF_scaler_check_desc(int path, int card);
int xsp3IF_scaler_check_progress(int path);
int xsp3IF_scaler_dtc_read(int path, double *dest, unsigned scaler, unsigned chan, unsigned t, unsigned n_scalers, unsigned n_chan, unsigned dt);
int xsp3IF_set_glob_timeA(int path, int card, u_int32_t time);
int xsp3IF_set_glob_timeFixed(int path, int card, u_int32_t time);
int xsp3IF_set_good_thres(int path, int chan, u_int32_t good_thres);
int xsp3IF_set_run_flags(int path, int flags);
int xsp3IF_set_window(int path, int chan, int win, int low, int high);


#ifdef XSP3IF_DEBUG
#define xsp3_clocks_setup xsp3IF_clocks_setup
#define xsp3_close xsp3IF_close
#define xsp3_config xsp3IF_config
#define xsp3_config_tcp xsp3IF_config_tcp
#define xsp3_config_udp xsp3IF_config_udp
#define xsp3_config_histogram_udp xsp3IF_config_histogram_udp
#define xsp3_config_scaler xsp3IF_config_scaler
#define xsp3_format_run xsp3IF_format_run
#define xsp3_getDeadtimeCorrectionParameters xsp3IF_getDeadtimeCorrectionParameters
#define xsp3_get_error_message xsp3IF_get_error_message
#define xsp3_get_good_thres xsp3IF_get_good_thres
#define xsp3_get_window xsp3IF_get_window
#define xsp3_hist_dtc_read4d xsp3IF_hist_dtc_read4d
#define xsp3_histogram_clear xsp3IF_histogram_clear
#define xsp3_histogram_continue xsp3IF_histogram_continue
#define xsp3_histogram_is_busy xsp3IF_histogram_is_busy
#define xsp3_histogram_is_any_busy xsp3IF_histogram_is_any_busy
#define xsp3_histogram_read4d xsp3IF_histogram_read4d
#define xsp3_histogram_start xsp3IF_histogram_start
#define xsp3_histogram_start_count_enb xsp3IF_histogram_start_count_enb
#define xsp3_histogram_stop xsp3IF_histogram_stop
#define xsp3_restore_settings xsp3IF_restore_settings
#define xsp3_save_settings xsp3IF_save_settings
#define xsp3_scaler_check_desc xsp3IF_scaler_check_desc
#define xsp3_scaler_check_progress xsp3IF_scaler_check_progress
#define xsp3_scaler_dtc_read xsp3IF_scaler_dtc_read
#define xsp3_set_glob_timeA xsp3IF_set_glob_timeA
#define xsp3_set_glob_timeFixed xsp3IF_set_glob_timeFixed
#define xsp3_set_good_thres xsp3IF_set_good_thres
#define xsp3_set_run_flags xsp3IF_set_run_flags
#define xsp3_set_window xsp3IF_set_window
#else
#define XSP3IF_DEBUG 0
#endif

#endif /* XSPRESS3INTERFACE_H */
