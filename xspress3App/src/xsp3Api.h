/**
 * Author: Diamond Light Source, Copyright 2014
 *
 * License: This file is part of 'xspress3'
 * 
 * 'xspress3' is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * 'xspress3' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with 'xspress3'.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @brief C++ wrapper for Xspress3 API
 *
 * @author Nick Rees
 * @date Octt 2014
 */
#ifndef XSPRESS3INTERFACE_H
#define XSPRESS3INTERFACE_H

#include "inttypes.h"
#include "xspress3.h"
#include "asynDriver.h"

class xsp3Api {
public:
    xsp3Api(asynUser * pasynUser);
    ~xsp3Api();

protected:
    virtual int xsp3Api_clocks_setup(int path, int card, int clk_src, int flags, int tp_type) = 0;
    virtual int xsp3Api_close(int path) = 0;
    virtual int xsp3Api_config(int ncards, int num_tf, char* baseIPaddress, int basePort, char* baseMACaddress, int nchan, int createmodule, char* modname, int debug, int card_index) = 0;
    virtual int xsp3Api_format_run(int path, int chan, int aux1_mode, int res_thres, int aux2_cont, int disables, int aux2_mode, int nbits_eng) = 0;
    virtual int xsp3Api_getDeadtimeCorrectionParameters(int path, int chan, int *flags, double *processDeadTimeAllEventGradient,
                                                     double *processDeadTimeAllEventOffset, double *processDeadTimeInWindowOffset, double *processDeadTimeInWindowGradient) = 0;
    virtual char* xsp3Api_get_error_message() = 0;
    virtual int xsp3Api_get_good_thres(int path, int chan, uint32_t *good_thres) = 0;
    virtual int xsp3Api_get_window(int path, int chan, int win, uint32_t *low, uint32_t *high) = 0;
    virtual int xsp3Api_hist_dtc_read4d(int path, double *hist_buff, double *scal_buff, unsigned eng, unsigned aux, unsigned chan, unsigned tf,
                                     unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf) = 0;
    virtual int xsp3Api_histogram_clear(int path, int first_chan, int num_chan, int first_frame, int num_frames) = 0;
    virtual int xsp3Api_histogram_arm(int path, int card) = 0;
    virtual int xsp3Api_histogram_continue(int path, int card) = 0;
    virtual int xsp3Api_histogram_pause(int path, int card) = 0;
    virtual int xsp3Api_histogram_is_any_busy(int path) = 0;
    virtual int xsp3Api_histogram_read4d(int path, uint32_t *buffer, unsigned eng, unsigned aux, unsigned chan, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf) = 0;
    virtual int xsp3Api_histogram_start(int path, int card) = 0;
    virtual int xsp3Api_histogram_stop(int path, int card) = 0;
    virtual int xsp3Api_restore_settings(int path, char *dir_name, int force_mismatch) = 0;
    virtual int xsp3Api_save_settings(int path, char *dir_name) = 0;
    virtual int xsp3Api_scaler_check_progress(int path) = 0;
    virtual int xsp3Api_set_glob_timeA(int path, int card, uint32_t time) = 0;
    virtual int xsp3Api_set_glob_timeFixed(int path, int card, uint32_t time) = 0;
    virtual int xsp3Api_set_good_thres(int path, int chan, uint32_t good_thres) = 0;
    virtual int xsp3Api_set_run_flags(int path, int flags) = 0;
    virtual int xsp3Api_set_window(int path, int chan, int win, int low, int high) = 0;
    virtual int xsp3Api_itfg_setup(int path, int card, int num_tf, uint32_t col_time, int trig_mode, int gap_mode) = 0;
    virtual int xsp3Api_itfg_setup2(int path, int card, int num_tf, u_int32_t col_time, int trig_mode, int gap_mode, int acq_in_pause, int marker_period, int marker_frame) = 0;
    virtual int xsp3Api_itfg_start(int path, int card) = 0;
    virtual int xsp3Api_itfg_stop(int path, int card) = 0;
    virtual int xsp3Api_has_itfg(int path, int card) = 0;
    virtual int xsp3Api_scaler_read(int path, uint32_t *dest, unsigned scaler, unsigned chan, unsigned t, unsigned n_scalers, unsigned n_chan, unsigned dt) = 0;
    virtual int xsp3Api_get_trigger_b(int path, unsigned chan, Xspress3_TriggerB *trig_b) = 0;
    virtual int xsp3Api_get_dtcfactor(int path, u_int32_t *scaData, double *dtcFactor, double *dtcAllEvent, unsigned chan) = 0;
    virtual int xsp3Api_get_generation(int path, int card) = 0;

public:
    int clocks_setup(int path, int card, int clk_src, int flags, int tp_type);
    int close(int path);
    int config(int ncards, int num_tf, char* baseIPaddress, int basePort, char* baseMACaddress, int nchan, int createmodule, char* modname, int debug, int card_index);
    int format_run(int path, int chan, int aux1_mode, int res_thres, int aux2_cont, int disables, int aux2_mode, int nbits_eng);
    int getDeadtimeCorrectionParameters(int path, int chan, int *flags, double *processDeadTimeAllEventGradient,
                                      double *processDeadTimeAllEventOffset, double *processDeadTimeInWindowOffset, double *processDeadTimeInWindowGradient);
    char* get_error_message();
    int get_good_thres(int path, int chan, uint32_t *good_thres);
    int get_window(int path, int chan, int win, uint32_t *low, uint32_t *high);
    int hist_dtc_read4d(int path, double *hist_buff, double *scal_buff, unsigned eng, unsigned aux, unsigned chan, unsigned tf,
                    unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf);
    int histogram_clear(int path, int first_chan, int num_chan, int first_frame, int num_frames);
    int histogram_pause(int path, int card);
    int histogram_arm(int path, int card);
    int histogram_continue(int path, int card);
    int histogram_is_any_busy(int path);
    int histogram_read4d(int path, uint32_t *buffer, unsigned eng, unsigned aux, unsigned chan, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf);
    int histogram_start(int path, int card);
    int histogram_stop(int path, int card);
    int restore_settings(int path, char *dir_name, int force_mismatch);
    int save_settings(int path, char *dir_name);
    int scaler_check_progress(int path);
    int set_glob_timeA(int path, int card, uint32_t time);
    int set_glob_timeFixed(int path, int card, uint32_t time);
    int set_good_thres(int path, int chan, uint32_t good_thres);
    int set_run_flags(int path, int flags);
    int set_window(int path, int chan, int win, int low, int high);
    int itfg_setup(int path, int card, int num_tf, uint32_t col_time, int trig_mode, int gap_mode);
    int itfg_setup2(int path, int card, int num_tf, u_int32_t col_time, int trig_mode, int gap_mode, int acq_in_pause, int marker_period, int marker_frame);
    int itfg_start(int path, int card);
    int itfg_stop(int path, int card);
    int has_itfg(int path, int card);
    int scaler_read(int path, uint32_t *dest, unsigned scaler, unsigned chan, unsigned t, unsigned n_scalers, unsigned n_chan, unsigned dt);
    int get_trigger_b(int path, unsigned card, Xspress3_TriggerB *trig_b);
    int get_dtcfactor(int path, u_int32_t *scaData, double *dtcFactor, double *dtcAllEvent, unsigned chan);
    int get_generation(int path, int card);

private:
    asynUser * pasynUser;
};

#endif /* XSPRESS3INTERFACE_H */
