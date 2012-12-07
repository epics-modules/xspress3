/**
 * Program to demonstrate how to setup and readout scaler data for Xspress3.
 *
 * Matthew Pearson, Nov 2012.
 */

#include "stdio.h"

#include "xspress3.h"

#define XSP3_IPADDR "192.168.0.1"
#define XSP3_MAXFRAMES 16384
#define XSP3_NUMCHANNELS 4
#define XSP3_MAXSPECTRA 4096
#define XSP3_CONFIGPATH "/home/mp49/xspress3_settings/"
#define MAX_CHECKDESC_POLLS 1000

int main(char *argv[], int argc)
{
  unsigned int poll = 0;
  unsigned int chan = 0;
  unsigned int sca = 0;
  unsigned int energy = 0;
  unsigned int frame = 0;
  unsigned int xsp3_handle = 0;
  unsigned int xsp3_status = 0;
  unsigned int num_frames = 0;
  unsigned int num_frames_to_read = 0;
  unsigned int last_num_frames = 0;
  double *pSCA = NULL;
  double *pSCA_OFFSET = NULL;
  double *pDUMP = NULL;
  unsigned int dump_offset = 0;
  unsigned int dump_offset_mca = 0;
  double *pMCA = NULL;

  pSCA = (double*)(calloc(XSP3_SW_NUM_SCALERS*XSP3_MAXFRAMES*XSP3_NUMCHANNELS, sizeof(double)));

  pMCA = (double*)(calloc(XSP3_MAXFRAMES*XSP3_NUMCHANNELS*XSP3_MAXSPECTRA, sizeof(double)));

  printf("Connect to the Xspress3...\n");
  xsp3_handle = xsp3_config(1, XSP3_MAXFRAMES, XSP3_IPADDR, -1, NULL, XSP3_NUMCHANNELS, 1, NULL, 1, 0);
  if (xsp3_handle < 0) {
    printf("ERROR calling xsp3_config. Return code: %d\n", xsp3_handle);
    return EXIT_FAILURE;
  }

  printf("Set up clocks register to use ADC clock...\n");
  xsp3_status = xsp3_clocks_setup(xsp3_handle, 0, XSP3_CLK_SRC_XTAL, XSP3_CLK_FLAGS_MASTER, 0);
  if (xsp3_status != XSP3_OK) {
    printf("ERROR calling xsp3_clocks_setup. Return code: %d\n", xsp3_status);
    return EXIT_FAILURE;
  }

  printf("Restoring the settings from the configuration files...\n");
  xsp3_status = xsp3_restore_settings(xsp3_handle, XSP3_CONFIGPATH, 0);
  if (xsp3_status != XSP3_OK) {
    printf("ERROR calling xsp3_restore_settings. Return code: %d\n", xsp3_status);
    return EXIT_FAILURE;
  }

  printf("Calling xsp3_format_run...\n");
  for (chan=0; chan<XSP3_NUMCHANNELS; ++chan) {
    xsp3_status = xsp3_format_run(xsp3_handle, chan, 0, 0, 0, 0, 0, 12);
    if (xsp3_status < XSP3_OK) {
      printf("ERROR calling xsp3_restore_settings. channel: %d, Return code: %d\n", chan, xsp3_status);
      return EXIT_FAILURE;
    } else {
      printf("  Channel: %d, Number of time frames configured: %d\n", chan, xsp3_status);
    }
  }

  printf("Set up playback data output...\n");
  xsp3_status = xsp3_set_run_flags(xsp3_handle, XSP3_RUN_FLAGS_PLAYBACK | XSP3_RUN_FLAGS_SCALERS | XSP3_RUN_FLAGS_HIST);
  if (xsp3_status < XSP3_OK) {
    printf("ERROR calling xsp3_set_run_flags. Return code: %d\n", xsp3_status);
    return EXIT_FAILURE;
  }

  printf("Set up trigger source to use external veto input...\n");
  xsp3_status = xsp3_set_glob_timeA(xsp3_handle, 0, XSP3_GLOB_TIMA_TF_SRC(XSP3_GTIMA_SRC_TTL_VETO_ONLY));
  if (xsp3_status != XSP3_OK) {
    printf("ERROR calling xsp3_set_glob_timeA. Return code: %d\n", xsp3_status);
    return EXIT_FAILURE;
  }

  printf("Starting histogramming...\n");
  xsp3_status = xsp3_histogram_start(xsp3_handle, 0);
  if (xsp3_status != XSP3_OK) {
    printf("ERROR calling xsp3_histogram_start. Return code: %d\n", xsp3_status);
    return EXIT_FAILURE;
  }

  printf("Now polling xsp3_scaler_check_desc and printing out any scaler data...\n");

  for (poll=0; poll<MAX_CHECKDESC_POLLS; ++poll) {

    xsp3_status = xsp3_scaler_check_desc(xsp3_handle, 0);
    if (xsp3_status < XSP3_OK) {
      printf("ERROR calling xsp3_scaler_check_desc. poll number: %d, Return code: %d\n", poll, xsp3_status);
      return EXIT_FAILURE;
    }

    printf("  Check_desc poll: %d, Number of frames: %d\n", poll, xsp3_status);

    if (xsp3_status > last_num_frames) {
     
      num_frames = xsp3_status;
      num_frames_to_read = num_frames - last_num_frames;
      printf("  last_num_frames: %d\n", last_num_frames);
      printf("  Reading out %d frames of scaler data...\n", num_frames_to_read);
      pSCA_OFFSET = pSCA+(last_num_frames*(XSP3_SW_NUM_SCALERS * XSP3_NUMCHANNELS));
      //xsp3_status = xsp3_scaler_read(xsp3_handle, pSCA_OFFSET, 0, 0, last_num_frames, XSP3_SW_NUM_SCALERS, XSP3_NUMCHANNELS, num_frames_to_read);
      //xsp3_status = xsp3_scaler_dtc_read(xsp3_handle, pSCA_OFFSET, 0, 0, last_num_frames, XSP3_SW_NUM_SCALERS, XSP3_NUMCHANNELS, num_frames_to_read);
      xsp3_status = xsp3_hist_dtc_read4d(xsp3_handle, pMCA, NULL, 0, 0, 0, last_num_frames, XSP3_MAXSPECTRA, 1, XSP3_NUMCHANNELS, num_frames_to_read);
      printf("Read out ok.\n");
      if (xsp3_status < XSP3_OK) {
	printf("ERROR calling xsp3_scaler_read. Return code: %d\n", xsp3_status);
	return EXIT_FAILURE;
      }
      
      /*printf("  Printing %d frames of scaler data...\n", num_frames_to_read);
      pDUMP = pSCA;
      for (frame=last_num_frames; frame<(num_frames_to_read+last_num_frames); frame++) {
	for (chan=0; chan<XSP3_NUMCHANNELS; chan++) {
	  for (sca=0; sca<XSP3_SW_NUM_SCALERS; sca++) {
	    //printf("  frame: %d, chan: %d, sca: %d, data[%d]: %d\n",
	    //   frame, chan, sca, dump_offset, *(pSCA_DUMP+dump_offset));
	    printf("  frame: %d, chan: %d, sca: %d, data[%d]: %f\n",
	       frame, chan, sca, dump_offset, *(pDUMP+dump_offset));
	    dump_offset++;
	  }
	}
	}*/

      /*pDUMP = pMCA;
      printf("  Printing elements of spectra data...\n");
      for (energy=0; energy<XSP3_MAXSPECTRA; energy++) {
	printf("  energy[%d]: %f\n", energy, *(pDUMP+dump_offset_mca));
	dump_offset_mca++;
	}*/
      
      last_num_frames = num_frames;
    }
    
  } /*end of poll loop*/

  printf("\nDone.\n");
  
  return EXIT_SUCCESS;

}
