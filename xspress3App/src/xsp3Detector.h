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
#ifndef XSP3DETECTOR_H
#define XSP3DETECTOR_H

#include "xsp3Api.h"

class xsp3Detector: public xsp3Api {
// Construction
public:
    xsp3Detector(asynUser * pasynUser);
    virtual ~xsp3Detector();

protected:
    virtual int xsp3Api_clocks_setup(int path, int card, int clk_src, int flags, int tp_type);
    virtual int xsp3Api_close(int path);
    virtual int xsp3Api_config(int ncards, int num_tf, char* baseIPaddress, int basePort, char* baseMACaddress, int nchan, int createmodule, char* modname, int debug, int card_index);
    virtual int xsp3Api_format_run(int path, int chan, int aux1_mode, int res_thres, int aux2_cont, int disables, int aux2_mode, int nbits_eng);
    virtual int xsp3Api_getDeadtimeCorrectionParameters(int path, int chan, int *flags, double *processDeadTimeAllEventGradient,
                                                     double *processDeadTimeAllEventOffset, double *processDeadTimeInWindowOffset, double *processDeadTimeInWindowGradient);
    virtual char* xsp3Api_get_error_message();
    virtual int xsp3Api_get_good_thres(int path, int chan, u_int32_t *good_thres);
    virtual int xsp3Api_get_window(int path, int chan, int win, u_int32_t *low, u_int32_t *high);
    virtual int xsp3Api_hist_dtc_read4d(int path, double *hist_buff, double *scal_buff, unsigned eng, unsigned aux, unsigned chan, unsigned tf,
                                     unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf);
    virtual int xsp3Api_histogram_clear(int path, int first_chan, int num_chan, int first_frame, int num_frames);
    virtual int xsp3Api_histogram_continue(int path, int card);
    virtual int xsp3Api_histogram_pause(int path, int card);
    virtual int xsp3Api_histogram_arm(int path, int card);
    virtual int xsp3Api_histogram_is_any_busy(int path);
    virtual int xsp3Api_histogram_read4d(int path, u_int32_t *buffer, unsigned eng, unsigned aux, unsigned chan, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf);
    virtual int xsp3Api_histogram_start(int path, int card);
    virtual int xsp3Api_histogram_stop(int path, int card);
    virtual int xsp3Api_restore_settings(int path, char *dir_name, int force_mismatch);
    virtual int xsp3Api_save_settings(int path, char *dir_name);
    virtual int xsp3Api_scaler_check_progress(int path);
    virtual int xsp3Api_set_glob_timeA(int path, int card, u_int32_t time);
    virtual int xsp3Api_set_glob_timeFixed(int path, int card, u_int32_t time);
    virtual int xsp3Api_set_good_thres(int path, int chan, u_int32_t good_thres);
    virtual int xsp3Api_set_run_flags(int path, int flags);
    virtual int xsp3Api_set_window(int path, int chan, int win, int low, int high);
    virtual int xsp3Api_itfg_setup(int path, int card, int num_tf, u_int32_t col_time, int trig_mode, int gap_mode);
    virtual int xsp3Api_itfg_setup2(int path, int card, int num_tf, u_int32_t col_time, int trig_mode, int gap_mode, int acq_in_pause, int marker_period, int marker_frame);
    virtual int xsp3Api_itfg_start(int path, int card);
    virtual int xsp3Api_itfg_stop(int path, int card);
    virtual int xsp3Api_has_itfg(int path, int card);
    virtual int xsp3Api_scaler_read(int path, u_int32_t *dest, unsigned scaler, unsigned chan, unsigned t, unsigned n_scalers, unsigned n_chan, unsigned dt);
    virtual int xsp3Api_get_trigger_b(int path, unsigned chan, Xspress3_TriggerB *trig_b);
    virtual int xsp3Api_get_dtcfactor(int path, u_int32_t *scaData, double *dtcFactor, double *dtcAllEvent, unsigned chan);
    virtual int xsp3Api_get_generation(int path, int card);
};

#endif /* XSP3DETECTOR_H */
