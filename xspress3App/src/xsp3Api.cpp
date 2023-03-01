#include "xsp3Api.h"

#define XSP3IF_DEBUG 0x100

/* Uncomment this line to enable debugging */
xsp3Api::xsp3Api( asynUser * user ) :
        pasynUser(user)
{
}
xsp3Api::~xsp3Api()
{
}

int xsp3Api::clocks_setup(int path, int card, int clk_src, int flags, int tp_type)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_clocks_setup( %d, %d, %d, %x, %d ) = ", path, card, clk_src, flags, tp_type );

    status = xsp3Api_clocks_setup(path, card, clk_src, flags, tp_type);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::close(int path)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_close( %d ) = ", path );

    status = xsp3Api_close( path );

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::config(int ncards, int num_tf, char* baseIPaddress, int basePort, char* baseMACaddress, int nchan, int createmodule, char* modname, int debug, int card_index)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_config( %d, %d, %s, %d, %s, %d, %d, %s, %d, %d ) = ", ncards, num_tf, baseIPaddress, basePort, baseMACaddress, nchan, createmodule, modname, debug, card_index );

    status = xsp3Api_config( ncards, num_tf, baseIPaddress, basePort, baseMACaddress, nchan, createmodule, modname, debug, card_index );

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::format_run(int path, int chan, int aux1_mode, int res_thres, int aux2_cont, int disables, int aux2_mode, int nbits_eng)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_format_run( %d, %d, %d, %d, %d, %d, %d, %d ) = ", path, chan, aux1_mode, res_thres, aux2_cont, disables, aux2_mode, nbits_eng);

    status = xsp3Api_format_run( path, chan, aux1_mode, res_thres, aux2_cont, disables, aux2_mode, nbits_eng);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::getDeadtimeCorrectionParameters(int path, int chan, int *flags,
                                           double *processDeadTimeAllEventGradient,
                                           double *processDeadTimeAllEventOffset, 
                                           double *processDeadTimeInWindowOffset, 
                                           double *processDeadTimeInWindowGradient)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_getDeadtimeCorrectionParameters( %d, %d, &%d,",
                 path, chan, *flags );

    status = xsp3Api_getDeadtimeCorrectionParameters( path, chan, flags,
                                                   processDeadTimeAllEventGradient,
                                                   processDeadTimeAllEventOffset,
                                                   processDeadTimeInWindowOffset,
                                                   processDeadTimeInWindowGradient );

    asynPrint(this->pasynUser, XSP3IF_DEBUG, " &%f, &%f, &%f, &%f ) = %d\n",
                 *processDeadTimeAllEventGradient,
                 *processDeadTimeAllEventOffset,
                 *processDeadTimeInWindowOffset,
                 *processDeadTimeInWindowGradient,
                 status );

    return status;
}

char* xsp3Api::get_error_message()
{
    char *message;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_get_error_message() = " );

    message = xsp3Api_get_error_message();

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%s\n", message );

    return message;
}

int xsp3Api::get_good_thres(int path, int chan, uint32_t *good_thres)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_get_good_thres( %d, %d ", path, chan );

    status = xsp3Api_get_good_thres( path, chan, good_thres);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, ", &%d ) = %d\n", *good_thres, status );

    return status;
}

int xsp3Api::get_window(int path, int chan, int win, uint32_t *low, uint32_t *high)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_get_window( %d, %d, %d ", path, chan, win );

    status = xsp3Api_get_window( path, chan, win, low, high);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, ", &%d, &%d ) = %d\n", *low, *high, status );

    return status;
}

int xsp3Api::hist_dtc_read4d(int path, double *hist_buff, double *scal_buff, unsigned eng, unsigned aux, unsigned chan, unsigned tf,
                         unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf)
{
    int status;

    status = xsp3Api_hist_dtc_read4d( path, hist_buff, scal_buff, eng, aux, chan, tf,
                                   num_eng, num_aux, num_chan, num_tf);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_hist_dtc_read4d( %d, [%f, %f, %f...], %p, %u, %u, %u, %u, %u, %u, %u, %u, ) = ",
                 path,
                 hist_buff[0],hist_buff[1],hist_buff[2],
                 scal_buff, eng, aux, chan, tf,
                 num_eng, num_aux, num_chan, num_tf);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::histogram_clear(int path, int first_chan, int num_chan, int first_frame, int num_frames)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_histogram_clear( %d, %d, %d, %d, %d ) = ", path, first_chan, num_chan, first_frame, num_frames);

    status = xsp3Api_histogram_clear( path, first_chan, num_chan, first_frame, num_frames);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::histogram_pause(int path, int card)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_histogram_pause( %d, %d ) = ", path, card );

    status = xsp3Api_histogram_pause( path, card);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::histogram_arm(int path, int card)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_histogram_arm( %d, %d ) = ", path, card );

    status = xsp3Api_histogram_arm( path, card);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::histogram_continue(int path, int card)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_histogram_continue( %d, %d ) = ", path, card );

    status = xsp3Api_histogram_continue( path, card);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::histogram_is_any_busy(int path)
{
    int status;
asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_histogram_is_any_busy( %d ) = ", path );

    status = xsp3Api_histogram_is_any_busy(path);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::histogram_read4d(int path, uint32_t *buffer, unsigned eng, unsigned aux, unsigned chan, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_histogram_read4d( %d, &%u..., %u, %u, %u, %u, %u, %u, %u, %u, ) = ",
                 path, *buffer, eng, aux, chan, tf, num_eng, num_aux, num_chan, num_tf);

    status = xsp3Api_histogram_read4d( path, buffer, eng, aux, chan, tf, num_eng, num_aux, num_chan, num_tf);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::histogram_start(int path, int card)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_histogram_start( %d, %d ) = ", path, card );

    status = xsp3Api_histogram_start( path, card);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::histogram_stop(int path, int card)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_histogram_stop( %d, %d ) = ", path, card );

    status = xsp3Api_histogram_stop( path, card);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::restore_settings(int path, char *dir_name, int force_mismatch)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_restore_settings( %d, %s, %d ) = ", path, dir_name, force_mismatch );

    status = xsp3Api_restore_settings( path, dir_name, force_mismatch );

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::save_settings(int path, char *dir_name)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_save_settings( %d, %s ) = ", path, dir_name );

    status = xsp3Api_save_settings( path, dir_name );

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::scaler_check_progress(int path)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_scaler_check_progress( %d ) = ", path );

    status = xsp3Api_scaler_check_progress( path );

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::set_glob_timeA(int path, int card, uint32_t time)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_set_glob_timeA( %d, %d, %x ) = ", path, card, time);

    status = xsp3Api_set_glob_timeA( path, card, time);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::set_glob_timeFixed(int path, int card, uint32_t time)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_set_glob_timeFixed( %d, %d, %u ) = ", path, card, time);

    status = xsp3Api_set_glob_timeFixed( path,  card, time);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::set_good_thres(int path, int chan, uint32_t good_thres)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_set_good_thres( %d, %d, %u ) = ", path, chan, good_thres);

    status = xsp3Api_set_good_thres( path, chan, good_thres);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::set_run_flags(int path, int flags)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_set_run_flags( %d, 0x%X ) = ", path, flags);

    status = xsp3Api_set_run_flags( path, flags);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::set_window(int path, int chan, int win, int low, int high)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_set_window( %d, %d, %d, %d. %d ) = ", path, chan, win, low, high);

    status = xsp3Api_set_window( path, chan, win, low, high);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::itfg_setup(int path, int card, int num_tf, uint32_t col_time, int trig_mode, int gap_mode)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_itfg_setup( %d, %d, %d, %u, %d, %d ) = ", path, card, num_tf, col_time, trig_mode, gap_mode);

    status = xsp3Api_itfg_setup(path, card, num_tf, col_time, trig_mode, gap_mode);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::itfg_setup2(int path, int card, int num_tf, uint32_t col_time, int trig_mode, int gap_mode, int acq_in_pause, int marker_period, int marker_frame)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_itfg_setup2( %d, %d, %d, %u, %d, %d, %d, %d, %d ) = ", path, card, num_tf, col_time, trig_mode, gap_mode, acq_in_pause, marker_period, marker_frame);

    status = xsp3Api_itfg_setup2(path, card, num_tf, col_time, trig_mode, gap_mode, acq_in_pause, marker_period, marker_frame);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::itfg_start(int path, int card) {
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_itfg_start( %d, %d ) = ", path, card);

    status = xsp3Api_itfg_start(path, card);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::itfg_stop(int path, int card) {
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_itfg_stop( %d, %d ) = ", path, card);

    status = xsp3Api_itfg_stop(path, card);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}




int xsp3Api::has_itfg(int path, int card )
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_has_itfg( %d, %d ) = ", path, card);

    status = xsp3Api_has_itfg(path, card);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::scaler_read(int path, uint32_t *dest, unsigned scaler, unsigned chan, unsigned t, unsigned n_scalers, unsigned n_chan, unsigned dt)
{
    int status;

    status = xsp3Api_scaler_read(path, dest, scaler, chan, t, n_scalers, n_chan, dt);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_scaler_read( %d, [%u, %u, %u...], %u, %u, %u, %u, %u, %u ) = ",
                 path,
                 dest[0],dest[1],dest[2],
                 scaler, chan, t, n_scalers, n_chan, dt);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::get_trigger_b(int path, unsigned card, Xspress3_TriggerB *trig_b)
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_get_trigger_b( %d, %d ) = ", path, card);

    status = xsp3Api_get_trigger_b(path, card, trig_b);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;
}

int xsp3Api::get_dtcfactor(int path, u_int32_t *scaData, double *dtcFactor, double *dtcAllEvent, unsigned chan) 
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_get_dtcfactor( %d, %d ) = ", path, chan);

    status = xsp3Api_get_dtcfactor(path, scaData, dtcFactor, dtcAllEvent, chan);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;

}

int xsp3Api::get_generation(int path, int card) 
{
    int status;
    asynPrint(this->pasynUser, XSP3IF_DEBUG, "xsp3_get_generation( %d, %d ) = ", path, card);

    status = xsp3Api_get_generation(path, card);

    asynPrint(this->pasynUser, XSP3IF_DEBUG, "%d\n", status );

    return status;

}
