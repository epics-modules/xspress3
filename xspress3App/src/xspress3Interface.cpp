#include "xspress3Interface.h"

/* Uncomment this line to enable debugging */
/* #define XSP3IF_DEBUG 1 */

int xsp3IF_clocks_setup(int path, int card, int clk_src, int flags, int tp_type)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_clocks_setup( %d, %d, %d, %x, %d ) = ", path, card, clk_src, flags, tp_type );

    status = xsp3_clocks_setup(path, card, clk_src, flags, tp_type);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_close(int path)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_close( %d ) = ", path );

    status = xsp3_close( path );

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_config(int ncards, int num_tf, char* baseIPaddress, int basePort, char* baseMACaddress, int nchan, int createmodule, char* modname, int debug, int card_index)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_config( %d, %d, %s, %d, %s, %d, %d, %s, %d, %d ) = ", ncards, num_tf, baseIPaddress, basePort, baseMACaddress, nchan, createmodule, modname, debug, card_index );

    status =  xsp3_config( ncards, num_tf, baseIPaddress, basePort, baseMACaddress, nchan, createmodule, modname, debug, card_index );

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_config_tcp(char femHostName[][XSP3_MAX_IP_CHARS], int femPort, int card, int chan, int debug)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_config_tcp( %s..., %d, %d, %d, %d ) = ",
                 femHostName[0], femPort, card, chan, debug );

    status = xsp3_config_tcp( femHostName, femPort, card, chan, debug);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

 
int xsp3IF_config_udp(int path, int card, char *femMACaddress, char*femIPaddress, int femPort, char* hostIPaddress, int hostPort)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_config_udp( %d, %d, %s, %s, %d, %s, %d ) = ", path, card, femMACaddress, femIPaddress, femPort, hostIPaddress, hostPort );

    status = xsp3_config_udp( path, card, femMACaddress, femIPaddress, femPort, hostIPaddress, hostPort);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_config_histogram_udp(int path, int card, char *hostIPaddress, int hostPort, char *femIPaddress, int femPort)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3IF_config_histogram_udp( %d, %d, %s, %d, %s, %d ) = ", path, card, hostIPaddress, hostPort, femIPaddress, femPort);

    status = xsp3_config_histogram_udp( path, card, hostIPaddress, hostPort, femIPaddress, femPort);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_config_scaler(int path)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_config_scaler( %d ) = ", path );

    status = xsp3_config_scaler( path);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_format_run(int path, int chan, int aux1_mode, int res_thres, int aux2_cont, int disables, int aux2_mode, int nbits_eng)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_format_run( %d, %d, %d, %d, %d, %d, %d, %d ) = ", path, chan, aux1_mode, res_thres, aux2_cont, disables, aux2_mode, nbits_eng);

    status = xsp3_format_run( path, chan, aux1_mode, res_thres, aux2_cont, disables, aux2_mode, nbits_eng);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_getDeadtimeCorrectionParameters(int path, int chan, int *flags,
                                           double *processDeadTimeAllEventGradient,
                                           double *processDeadTimeAllEventOffset, 
                                           double *processDeadTimeInWindowOffset, 
                                           double *processDeadTimeInWindowGradient)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_getDeadtimeCorrectionParameters( %d, %d, &%d,",
                 path, chan, *flags );

    status = xsp3_getDeadtimeCorrectionParameters( path, chan, flags,
                                                   processDeadTimeAllEventGradient,
                                                   processDeadTimeAllEventOffset,
                                                   processDeadTimeInWindowOffset,
                                                   processDeadTimeInWindowGradient );

    if (XSP3IF_DEBUG) 
        fprintf( stderr, " &%f, &%f, &%f, &%f ) = %d\n",
                 *processDeadTimeAllEventGradient,
                 *processDeadTimeAllEventOffset,
                 *processDeadTimeInWindowOffset,
                 *processDeadTimeInWindowGradient,
                 status );

    return status;
}

char* xsp3IF_get_error_message()
{
    char *message;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3IF_get_error_message() = " );

    message = xsp3_get_error_message();

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%s\n", message );

    return message;
}

int xsp3IF_get_good_thres(int path, int chan, u_int32_t *good_thres)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_get_good_thres( %d, %d ", path, chan );

    status = xsp3_get_good_thres( path, chan, good_thres);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, ", &%d ) = %d\n", *good_thres, status );

    return status;
}

int xsp3IF_get_window(int path, int chan, int win, u_int32_t *low, u_int32_t *high)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_get_window( %d, %d, %d ", path, chan, win );

    status = xsp3_get_window( path, chan, win, low, high);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, ", &%d, &%d ) = %d\n", *low, *high, status );

    return status;
}

int xsp3IF_hist_dtc_read4d(int path, double *hist_buff, double *scal_buff, unsigned eng, unsigned aux, unsigned chan, unsigned tf,
                         unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf)
{
    int status;

    status = xsp3_hist_dtc_read4d( path, hist_buff, scal_buff, eng, aux, chan, tf,
                                   num_eng, num_aux, num_chan, num_tf);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_hist_dtc_read4d( %d, [%f, %f, %f...], %p, %u, %u, %u, %u, %u, %u, %u, %u, ) = ", 
                 path,
                 hist_buff[0],hist_buff[1],hist_buff[2],
                 scal_buff, eng, aux, chan, tf,
                 num_eng, num_aux, num_chan, num_tf);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_histogram_clear(int path, int first_chan, int num_chan, int first_frame, int num_frames)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_histogram_clear( %d, %d, %d, %d, %d ) = ", path, first_chan, num_chan, first_frame, num_frames);

    status = xsp3_histogram_clear( path, first_chan, num_chan, first_frame, num_frames);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_histogram_continue(int path, int card)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_histogram_continue( %d, %d ) = ", path, card );

    status = xsp3_histogram_continue( path, card);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_histogram_is_busy(int path, int chan)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_histogram_is_busy( %d, %d ) = ", path, chan );

    status = xsp3_histogram_is_busy(path, chan);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_histogram_read4d(int path, u_int32_t *buffer, unsigned eng, unsigned aux, unsigned chan, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_histogram_read4d( %d, &%u..., %u, %u, %u, %u, %u, %u, %u, %u, ) = ", 
                 path, *buffer, eng, aux, chan, tf, num_eng, num_aux, num_chan, num_tf);

    status = xsp3_histogram_read4d( path, buffer, eng, aux, chan, tf, num_eng, num_aux, num_chan, num_tf);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_histogram_start(int path, int card)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_histogram_start( %d, %d ) = ", path, card );

    status = xsp3_histogram_start( path, card);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_histogram_start_count_enb(int path, int card, int count_enb)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_histogram_count_enb( %d, %d, %d ) = ", path, card, count_enb );

    status = xsp3_histogram_start_count_enb( path, card, count_enb);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;

}

int xsp3IF_histogram_stop(int path, int card)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_histogram_stop( %d, %d ) = ", path, card );

    status = xsp3_histogram_stop( path, card);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_restore_settings(int path, char *dir_name, int force_mismatch)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_restore_settings( %d, %s, %d ) = ", path, dir_name, force_mismatch );

    status = xsp3_restore_settings( path, dir_name, force_mismatch );

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_save_settings(int path, char *dir_name)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_save_settings( %d, %s ) = ", path, dir_name );

    status = xsp3_save_settings( path, dir_name );

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_scaler_check_desc(int path, int card)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_scaler_check_desc( %d, %d ) = ", path, card );

    status = xsp3_scaler_check_desc( path, card);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_scaler_dtc_read(int path, double *dest, unsigned scaler, unsigned chan, unsigned t, unsigned n_scalers, unsigned n_chan, unsigned dt)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_scaler_dtc_read( %d, &%f..., %u, %u, %u, %u, %u, %u ) = ", path, *dest, scaler, chan, t, n_scalers, n_chan, dt);

    status = xsp3_scaler_dtc_read( path, dest, scaler, chan, t, n_scalers, n_chan, dt);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_set_glob_timeA(int path, int card, u_int32_t time)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_set_glob_timeA( %d, %d, %x ) = ", path, card, time);

    status = xsp3_set_glob_timeA( path, card, time);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_set_glob_timeFixed(int path, int card, u_int32_t time)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_set_glob_timeFixed( %d, %d, %u ) = ", path, card, time);

    status = xsp3_set_glob_timeFixed( path,  card, time);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_set_good_thres(int path, int chan, u_int32_t good_thres)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_set_good_thres( %d, %d, %u ) = ", path, chan, good_thres);

    status = xsp3_set_good_thres( path, chan, good_thres);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_set_run_flags(int path, int flags)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_set_run_flags( %d, 0x%X ) = ", path, flags);

    status = xsp3_set_run_flags( path, flags);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

int xsp3IF_set_window(int path, int chan, int win, int low, int high)
{
    int status;
    if (XSP3IF_DEBUG) 
        fprintf( stderr, "xsp3_set_window( %d, %d, %d, %d. %d ) = ", path, chan, win, low, high);

    status = xsp3_set_window( path, chan, win, low, high);

    if (XSP3IF_DEBUG) 
        fprintf( stderr, "%d\n", status );

    return status;
}

