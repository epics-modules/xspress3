#include "xsp3Simulator.h"
#include "xsp3SimElement.h"

xsp3Simulator::xsp3Simulator( asynUser * user, int max_detectors, int max_spectra ) :
    xsp3Api(user),
    num_detectors(max_detectors),
    runFlags(0),
    frame_time(0.0),
    num_frames(0),
    current_frame(0)
{
    detectors.reserve(max_detectors);
    for (int i=0; i< max_detectors; i++)
        detectors.push_back(xsp3SimElement(max_spectra));

    scanStart = epicsTime::getCurrent();
    this->handle=314158;
}
xsp3Simulator::~xsp3Simulator()
{
}

int xsp3Simulator::xsp3Api_clocks_setup(int path, int card, int clk_src, int flags, int tp_type)
{
   return XSP3_OK;
}

int xsp3Simulator::xsp3Api_close(int path)
{
    if (path != this->handle) return XSP3_ERROR;
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_config(int ncards, int num_tf, char* baseIPaddress, int basePort, char* baseMACaddress, int nchan, int createmodule, char* modname, int debug, int card_index)
{
    return this->handle;
}

int xsp3Simulator::xsp3Api_format_run(int path, int chan, int aux1_mode, int res_thres, int aux2_cont, int disables, int aux2_mode, int nbits_eng)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_getDeadtimeCorrectionParameters(int path, int chan, int *flags,
                                           double *processDeadTimeAllEventGradient,
                                           double *processDeadTimeAllEventOffset, 
                                           double *processDeadTimeInWindowOffset, 
                                           double *processDeadTimeInWindowGradient)
{
    *processDeadTimeAllEventGradient= detectors[chan].processDeadTimeAllEventGradient;
    *processDeadTimeAllEventOffset  = detectors[chan].processDeadTimeAllEventOffset;
    *processDeadTimeInWindowOffset  = detectors[chan].processDeadTimeInWindowOffset;
    *processDeadTimeInWindowGradient= detectors[chan].processDeadTimeInWindowGradient;

    return XSP3_OK;
}

char* xsp3Simulator::xsp3Api_get_error_message()
{
    return (char *) "Simulator is happy";
}

int xsp3Simulator::xsp3Api_get_good_thres(int path, int chan, uint32_t *good_thres)
{
    *good_thres = this->detectors[chan].threshold;
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_get_window(int path, int chan, int win, uint32_t *low, uint32_t *high)
{
    *low = this->detectors[chan].window[win].low;
    *high = this->detectors[chan].window[win].high;
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_hist_dtc_read4d(int path, double *hist_buff, double *scal_buff,
                                           unsigned eng, unsigned aux, unsigned chan, unsigned tf,
                                           unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf)
{
    for (unsigned int frame = tf; frame < tf + num_tf; frame++ )
    {
        for (unsigned int i = chan; i < chan + num_chan; i++)
        {
            detectors[i].generateDTCSpectra( frame, eng, num_eng, hist_buff );
            hist_buff += num_eng;

            if (scal_buff != NULL)
            {
                for (int scaler = 0; scaler < XSP3_SW_NUM_SCALERS; scaler++ )
                    scal_buff[scaler] = 0.0;

                scal_buff[5]=detectors[i].generateDTCROI(frame, 0 );
                scal_buff[6]=detectors[i].generateDTCROI(frame, 1 );

                scal_buff += XSP3_SW_NUM_SCALERS;
            }
        }
    }

    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_histogram_clear(int path, int first_chan, int num_chan, int first_frame, int num_frames)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_histogram_continue(int path, int card)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_histogram_pause(int path, int card)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_histogram_arm(int path, int card)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_histogram_is_any_busy(int path)
{
    //TODO: Occasionally return busy (1).
    return 0;
}

int xsp3Simulator::xsp3Api_histogram_read4d(int path, uint32_t *buffer, unsigned eng, unsigned aux, unsigned chan, unsigned tf, unsigned num_eng, unsigned num_aux, unsigned num_chan, unsigned num_tf)
{
    for (unsigned int frame = tf; frame < tf + num_tf; frame++ )
    {
        for (unsigned int i = chan; i < chan + num_chan; i++)
        {
            detectors[i].generateRawSpectra( frame, eng, num_eng, buffer );
            buffer += num_eng;
        }
    }
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_histogram_start(int path, int card)
{
    scanStart = epicsTime::getCurrent();
    current_frame=0;
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_histogram_stop(int path, int card)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_restore_settings(int path, char *dir_name, int force_mismatch)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_save_settings(int path, char *dir_name)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_scaler_check_progress(int path)
{
    if ( timeRegister.trigger == xsp3TimeRegister::Internal )
    {
        current_frame = (epicsTime::getCurrent() - scanStart)/frame_time ;
        if (current_frame > num_frames) current_frame = num_frames;
    } else {
        current_frame+= ((current_frame+1)%10);
    }
    return current_frame;
}

int xsp3Simulator::xsp3Api_set_glob_timeA(int path, int card, uint32_t time)
{
    timeRegister.set(time);
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_set_glob_timeFixed(int path, int card, uint32_t time)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_set_good_thres(int path, int chan, uint32_t good_thresh)
{
    detectors[chan].threshold = good_thresh;
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_set_run_flags(int path, int flags)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_set_window(int path, int chan, int win, int low, int high)
{
    detectors[chan].window[win].low = low;
    detectors[chan].window[win].high = high;
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_itfg_setup(int path, int card, int num_tf, uint32_t col_time, int trig_mode, int gap_mode)
{
    frame_time = (double) col_time/80E6;
    num_frames = num_tf;
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_itfg_setup2(int path, int card, int num_tf, uint32_t col_time, int trig_mode, int gap_mode, int acq_in_pause, int marker_period, int marker_frame)
{
    frame_time = (double) col_time/80E6;
    num_frames = num_tf;
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_itfg_start(int path, int card)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_itfg_stop(int path, int card)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_has_itfg(int path, int card )
{
    return 1;
}

int xsp3Simulator::xsp3Api_scaler_read(int path, uint32_t *dest, unsigned scaler, unsigned chan, unsigned t, unsigned n_scalers, unsigned n_chan, unsigned dt)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_get_trigger_b(int path, unsigned chan, Xspress3_TriggerB *trig_b)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_get_dtcfactor(int path, u_int32_t *scaData, double *dtcFactor, double *dtcAllEvent, unsigned chan)
{
    return XSP3_OK;
}

int xsp3Simulator::xsp3Api_get_generation(int path, int card)
{
    return 0;
}
