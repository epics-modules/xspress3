#include <iostream>

//Xspress3 API header
#include "xspress3.h"

int main(char *argv[], int argc)
{
  int xsp3_handle_ = 0;
  int xsp3_status = 0;
  int xsp3_num_channels = 4;
  int xsp3_max_polls = 100;
  int xsp3_num_frames = 0;

  printf("Testing calling xsp3_scaler_check_desc...\n");

  printf("Calling xsp3_config...\n");
  xsp3_handle_ = xsp3_config(1, 16384, "192.168.0.1", -1, NULL, xsp3_num_channels, 1, NULL, 1, 0);
  if (xsp3_handle_ < 0) {
    printf("ERROR: xsp3_handle_: %d\n", xsp3_handle_);
    return EXIT_FAILURE;
  }

  printf("Calling xsp3_clocks_setup...\n");
  xsp3_status = xsp3_clocks_setup(xsp3_handle_, 0, XSP3_CLK_SRC_INT, XSP3_CLK_FLAGS_MASTER, 0);
  if (xsp3_status != XSP3_OK) {
    printf("ERROR calling xsp3_clocks_setup.\n", xsp3_handle_);
    return EXIT_FAILURE;
  }
  
  printf("Calling xsp3_restore_settings...\n");
  xsp3_status = xsp3_restore_settings(xsp3_handle_, "/home/mp49/xspress3_settings/", 0);
  if (xsp3_status != XSP3_OK) {
    printf("ERROR calling xsp3_restore_settings.\n", xsp3_handle_);
    return EXIT_FAILURE;
  }

  printf("Calling xsp3_format_run...\n");
  for (int chan=0; chan<xsp3_num_channels; ++chan) {
    xsp3_status = xsp3_format_run(xsp3_handle_, chan, 0, 0, 0, 0, 0, 12);
    if (xsp3_status < XSP3_OK) {
      printf("ERROR calling xsp3_restore_settings.\n", xsp3_handle_);
    return EXIT_FAILURE;
    } else {
      printf("  Channel: %d, Number of time frames configured: %d\n", chan, xsp3_status);
    }
  }

  printf("Calling xsp3_set_run_flags...\n");
  xsp3_status = xsp3_set_run_flags(xsp3_handle_, XSP3_RUN_FLAGS_PLAYBACK | XSP3_RUN_FLAGS_SCALERS | XSP3_RUN_FLAGS_HIST);
  if (xsp3_status < XSP3_OK) {
    printf("ERROR calling xsp3_set_run_flags.\n", xsp3_handle_);
    return EXIT_FAILURE;
  }

  printf("Calling xsp3_set_glob_timeA...\n");
  xsp3_status = xsp3_set_glob_timeA(xsp3_handle_, 0, XSP3_GLOB_TIMA_TF_SRC(XSP3_GTIMA_SRC_TTL_VETO_ONLY));
  if (xsp3_status != XSP3_OK) {
    printf("ERROR calling xsp3_set_glob_timeA.\n", xsp3_handle_);
    return EXIT_FAILURE;
  }

  printf("Calling xsp3_histogram_start...\n");
  xsp3_status = xsp3_histogram_start(xsp3_handle_, 0);
  if (xsp3_status != XSP3_OK) {
    printf("ERROR calling xsp3_histogram_start.\n", xsp3_handle_);
    return EXIT_FAILURE;
  }

  printf("Now starting to poll xsp3_scaler_check_desc...\n");

  for (int poll=0; poll<xsp3_max_polls; ++poll) {
    xsp3_status = xsp3_scaler_check_desc(xsp3_handle_, 0);
    if (xsp3_status < XSP3_OK) {
      printf("ERROR calling xsp3_scaler_check_desc.\n", xsp3_handle_);
      printf("  poll: %d\n", poll);
      return EXIT_FAILURE;
    } else {
      xsp3_num_frames = xsp3_status;
      printf("  poll: %d, xsp3_num_frames: %d\n", poll, xsp3_num_frames);
    }
  }

  printf("\nDone.\n");
  
  return EXIT_SUCCESS;

}
