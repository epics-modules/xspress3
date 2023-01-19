#include "xsp3Detector.h"

xsp3Detector::xsp3Detector(asynUser * user) :
    xsp3Api( user )
{
}
xsp3Detector::~xsp3Detector()
{
}

int xsp3Detector::xsp3Api_clocks_setup(int path, int card, int clk_src, int flags, int tp_type)
{
    int status;
    status = xsp3_clocks_setup(path, card, clk_src, flags, tp_type);
    return status;
}

int xsp3Detector::xsp3Api_close(int path)
{
    int status;
    status = xsp3_close( path );
    return status;
}

int xsp3Detector::xsp3Api_config(int ncards, int num_tf, char* baseIPaddress, int basePort, char* baseMACaddress, int nchan, int createmodule, char* modname, int debug, int card_index)
{
    int status;
    status = xsp3_config( ncards, num_tf, baseIPaddress, basePort, baseMACaddress, nchan, createmodule, modname, debug, card_index );
    return status;
}

int xsp3Detector::xsp3Api_format_run(int path, int chan, int aux1_mode, int res_thres, int aux2_cont, int disables, int aux2_mode, int nbits_eng)
{
    int status;
    status = xsp3_format_run( path, chan, aux1_mode, res_thres, aux2_cont, disables, aux2_mode, nbits_eng);
    return status;
}

int xsp3Detector::xsp3Api_getDeadtimeCorrectionParameters(int path, int chan, int *flags,
                                           double *processDeadTimeAllEventGradient,
                                           double *processDeadTimeAllEventOffset, 
                                           double *processDeadTimeInWindowOffset, 
                                           double *processDeadTimeInWindowGradient)
{
    int status;
    status = xsp3_getDeadtimeCorrectionParameters( path, chan, flags,
                                                   processDeadTimeAllEventGradient,
                                                   processDeadTimeAllEventOffset,
                                                   processDeadTimeInWindowOffset,
                                                   processDeadTimeInWindowGradient );
    return status;
}

char* xsp3Detector::xsp3Api_get_error_message()
{
    char *message;
    message = xsp3_get_error_message();
    return message;
}

int xsp3Detector::xsp3Api_get_good_thres(int path, int chan, u_int32_t *good_thres)
{
    int status;
    status = xsp3_get_good_thres( path, chan, good_thres);
    return status;
}

int xsp3Detector::xsp3Api_get_window(int path, int chan, int win, u_int32_t *low, u_int32_t *high)
{
    int status;
    status = xsp3_get_window( path, chan, win, low, high);
    return status;
}

int xsp3Detector::xsp3Api_hist_dtc_read4d(int path, double *hist_buff, double *scal_buff, unsigned eng, unsigned aux, unsigned chan, unsigned tf,
                         unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf)
{
    int status;
    status = xsp3_hist_dtc_read4d( path, hist_buff, scal_buff, eng, aux, chan, tf,
                                   num_eng, num_aux, num_chan, num_tf);
    return status;
}

int xsp3Detector::xsp3Api_histogram_clear(int path, int first_chan, int num_chan, int first_frame, int num_frames)
{
    int status;
    status = xsp3_histogram_clear( path, first_chan, num_chan, first_frame, num_frames);
    return status;
}

int xsp3Detector::xsp3Api_histogram_continue(int path, int card)
{
    int status;
    status = xsp3_histogram_continue( path, card);
    return status;
}

int xsp3Detector::xsp3Api_histogram_arm(int path, int card)
{
    int status;
    status = xsp3_histogram_arm( path, card);
    return status;  
}

int xsp3Detector::xsp3Api_histogram_pause(int path, int card)
{
    int status;
    status = xsp3_histogram_pause( path, card);
    return status;
}

int xsp3Detector::xsp3Api_histogram_is_any_busy(int path)
{
    int status;
    status = xsp3_histogram_is_any_busy(path);
    return status;
}

int xsp3Detector::xsp3Api_histogram_read4d(int path, u_int32_t *buffer, unsigned eng, unsigned aux, unsigned chan, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf)
{
    int status;
    status = xsp3_histogram_read4d( path, buffer, eng, aux, chan, tf, num_eng, num_aux, num_chan, num_tf);
    return status;
}

int xsp3Detector::xsp3Api_histogram_start(int path, int card)
{
    int status;
    status = xsp3_histogram_start( path, card);
    return status;
}

int xsp3Detector::xsp3Api_histogram_stop(int path, int card)
{
    int status;
    status = xsp3_histogram_stop( path, card);
    return status;
}

int xsp3Detector::xsp3Api_restore_settings(int path, char *dir_name, int force_mismatch)
{
    int status;
    status = xsp3_restore_settings( path, dir_name, force_mismatch );
    return status;
}

int xsp3Detector::xsp3Api_save_settings(int path, char *dir_name)
{
    int status;
    status = xsp3_save_settings( path, dir_name );
    return status;
}

int xsp3Detector::xsp3Api_scaler_check_progress(int path)
{
    int status;
    status = xsp3_scaler_check_progress( path );
    return status;
}

int xsp3Detector::xsp3Api_set_glob_timeA(int path, int card, u_int32_t time)
{
    int status;
    status = xsp3_set_glob_timeA( path, card, time);
    return status;
}

int xsp3Detector::xsp3Api_set_glob_timeFixed(int path, int card, u_int32_t time)
{
    int status;
    status = xsp3_set_glob_timeFixed( path,  card, time);
    return status;
}

int xsp3Detector::xsp3Api_set_good_thres(int path, int chan, u_int32_t good_thres)
{
    int status;
    status = xsp3_set_good_thres( path, chan, good_thres);
    return status;
}

int xsp3Detector::xsp3Api_set_run_flags(int path, int flags)
{
    int status;
    status = xsp3_set_run_flags( path, flags);
    return status;
}

int xsp3Detector::xsp3Api_set_window(int path, int chan, int win, int low, int high)
{
    int status;
    status = xsp3_set_window( path, chan, win, low, high);
    return status;
}

int xsp3Detector::xsp3Api_itfg_setup(int path, int card, int num_tf, u_int32_t col_time, int trig_mode, int gap_mode)
{
    int status;
    status = xsp3_itfg_setup(path, card, num_tf, col_time, trig_mode, gap_mode);
    return status;
}

int xsp3Detector::xsp3Api_itfg_setup2(int path, int card, int num_tf, u_int32_t col_time, int trig_mode, int gap_mode, int acq_in_pause, int marker_period, int marker_frame)
{
    int status;
    status = xsp3_itfg_setup2(path, card, num_tf, col_time, trig_mode, gap_mode, acq_in_pause, marker_period, marker_frame);
    return status;
}

int xsp3Detector::xsp3Api_itfg_start(int path, int card)
{
    int status;
    status = xsp3_itfg_start(path, card);
    return status;
}

int xsp3Detector::xsp3Api_itfg_stop(int path, int card)
{
    int status;
    status = xsp3_itfg_stop(path, card);
    return status;
}

int xsp3Detector::xsp3Api_has_itfg(int path, int card )
{
    int status;
    status = xsp3_has_itfg(path, card);
    return status;
}

int xsp3Detector::xsp3Api_scaler_read(int path, u_int32_t *dest, unsigned scaler, unsigned chan, unsigned t, unsigned n_scalers, unsigned n_chan, unsigned dt)
{
    int status;
    status = xsp3_scaler_read(path, dest, scaler, chan, t, n_scalers, n_chan, dt);
    return status;
}

int xsp3Detector::xsp3Api_get_trigger_b(int path, unsigned chan, Xspress3_TriggerB *trig_b)
{
    int status;
    status = xsp3_get_trigger_b(path, chan, trig_b);
    return status;
}

int xsp3Detector::xsp3Api_get_dtcfactor(int path, u_int32_t *scaData, double *dtcFactor, double *dtcAllEvent, unsigned chan)
{
    int status;
    status = xsp3_calculateDeadtimeCorrectionFactors(path, scaData, dtcFactor, dtcAllEvent, 1, chan, 1);
    return status;
}

int xsp3Detector::xsp3Api_get_generation(int path, int card)
{
    return xsp3_get_generation(path, card);
}

