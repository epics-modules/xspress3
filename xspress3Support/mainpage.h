/**
 * @mainpage
 *
 * @author W.I.Helsby
 * @author G.R.Mant
 * @version 2.2.0
 * @date 26/6/2018
 * @tableofcontents
 * @section intro "Introduction"
 *
 * @image html "xspress3_drawing.png"
 * The XSPRESS3 system contains a Xilinx Virtex-5 FPGA with two embedded PowerPC processors. PPC1 manages the DMA engines. PPC1 runs the Xilinx micro kernel and communicates to the Intel 64 bit Linux
 * server PC by 1 GBit Ethernet, TCP sockets. Bulk data and event lists to be histogrammed are sent from the firmware to the Server PC by 10G Ethernet, UDP.
 * @n
 * The XSPRSSS3Mini system contains a Xilinx ZYNQ with dual ARM processors. The dual cores run Linux with a SMP kernel. The ZYNQ Programmable System (PS) DRAM is shared between the OS and the 
 * XSPRSSS3Mini firmware. Again the DMA engines are provided for playback and Scope mode. 
 * Full rate data is histogrammed in to a BRAM in the FPGA and transfer to the DRAM by DMA. These spectra are limited to 4096 pixels. Additionally there is a diagnostic histogram route which DMAs 
 * lists of events to a software histogram.
 * @n
 * XSPRESS4 is implemented in a Virtex-7 with control via a ZYNQ. The Zynq runs Linux and controls registers and a window on to the DRAM via an AXI Chip to Chip link. This allows control via a 1 G Ethernet Link.
 * Playback and Scope mode DMAs are provided to the 4 GBytes of SDAM connected to the Virtex-7, along with routes to allow bulk data to be transferred by 10 GEthernet. 
 * As with XSPRSS3, the list of output events are sent to the X86_64 server by 10 GEthernet UDP.
 * @n
 * The library will work will all 3 generations. By default the control port for XSPRESS3 is 30123. For XSPRESS3Mini and XSPRESS4 the port is 30124. The software tries ports 30123 and then 30124 
 * to detector whether it is the PPC or ZYNQ its is talking to. It then reads the revision register to determine which generation it is controlling.
 * The library tries to provide a common API to all three generations where possible.
 * 
 * @subsection software_overview Software Overview
 * The software API is via the libxspress3.a which provides C calling functions with names xsp3_*.
 * @subsubsection software_setup Setup procedure
 * The library supports running 1 or more XSPRESS3 systems. Each system can consists of 1 or more FEM cards. To connect to a system of 1 or more boards use the command {@link xsp3_config}
 * This returns a handle which should be used to identify the system to all subsequent calls.
 * Next setup the clock source. For XSPRESS3 normal operation the ADC and data processing clock is set to use the Crystal Oscillator module on the ADC board using 
 * xsp3_clocks_setup(path, -1, XSP3_CLK_SRC_XTAL, XSP3_CLK_FLAGS_MASTER | XSP3_CLK_FLAGS_NO_DITHER, 0)
 * For multi-box XSPRESS3 systems there is provision for a master XSPRESS3 box to supply clocks to the other boxes, but this has not been used yet.
 * The master board will be set to use the crystal and all the others will use a clock looped from this board with clk_src=XSP3_CLK_SRC_EXT.
 * @n
 * For XSPRESS3Mini there are two choices of ADC board clocks: CDCM61004 or LMK61E2.  These are set using:
 * xsp3_clocks_setup(path, -1, XSP3M_CLK_SRC_CDCM61004, XSP3_CLK_FLAGS_NO_DITHER, 0) or
 * xsp3_clocks_setup(path, -1, XSP3M_CLK_SRC_LMK61E2, XSP3_CLK_FLAGS_NO_DITHER, 0) respectively.
 * @n
 * For a single card XSPRESS4 there are two choices of ADC board clocks. These are currently selected by changing resistors, but the library does differential the two for future expansion.
 * xsp3_clocks_setup(path, -1, XSP3M_CLK_SRC_CDCM61004, XSP3_CLK_FLAGS_NO_DITHER, 0) or
 * xsp3_clocks_setup(path, -1, XSP3M_CLK_SRC_LMK61E2, XSP3_CLK_FLAGS_NO_DITHER, 0) respectively.
 * For a multi-card XSPRESS4 system it will be usual to use the mid-plane clock source. Currently both CDCM61004 and LMK61E2 are proposed
 * xsp3_clocks_setup(path, -1, XSP4_CLK_SRC_MIDPLN_CDCM61004, XSP3_CLK_FLAGS_NO_DITHER, 0) or
 * xsp3_clocks_setup(path, -1, XSP4_CLK_SRC_MIDPLN_LMK61E2, XSP3_CLK_FLAGS_NO_DITHER, 0)
 
 * For all generations to allow digital only testing it is possible to generate the processing clock from the FPGA on board clocks using:
 * xsp3_clocks_setup(path, -1, XSP3_CLK_SRC_INT, XSP3_CLK_FLAGS_NO_DITHER, 0)
 * See {@link xsp3_clocks_setup}
 * @n
 * The processing register settings and BRAM contents can then loaded using {@link xsp3_restore_settings}.
 * To hide the complexity of the clock setup, the library will now save the clock configuration in the settings directory. The settings and clock setup can be restored using 
 * {@link xsp3_restore_settings_and_clock()}.
 * This is now the preferred solution as it will work across all generations.
 * @n
 * The overall run flags should then be set to specify if whether playback data is to be used, whether scope mode debug data should be stored and whether scalers should be recorded. See {@link xsp3_restore_settings}
 * The timing control register allows the acquisition enable/disable and time frame number to come from either software control, the internal time frame generator 
 * or from external hardware - for accurately timed multi-frame experiments. See {@link  xsp3_set_glob_timeA} and {@link xsp3_set_glob_timeFixed}
 * @subsubsection software_user_setup User Settings
 * The system now needs to need set up to match user specifiable values. In particular the energy windows and RoI positions need to be set to match the energy of the element(s) of interest.
 * Two in window scalers are provided per channel, typically used to measure the counts in the main characteristic peak of 1 or 2 elements See {@link xsp3_set_window}
 * The Histogram data is usually set to collect the full spectrum in 4096 energy bins. Various debug modes are available, but not required for user data. 
 * To reduce the data volume, it is also possible to collect regions of interest around the peaks of interest. In this case the total size of each spectrum is reduced from 12 bits = 4096 bins to 11..1 bits
 * => 2048 .. 2 bins.
 * This feature was not used and so was configured out of some builds. Test with {@link xsp3_has_roi()}.
 * First the Spectra size is set using {@link xsp3_format_run} and then if required RoI are set using {@link xsp3_set_roi}
 * @subsubsection Data Collection
 * If the internal time frame generator is being used, this is setup using {@link xsp3_itfg_setup()}.
 * Once setup the histogramming memory is cleared using {@link xsp3_histogram_clear}. Then the system is started using {@link xsp3_histogram_start}
 * If the timing control has been set to a fixed frame from software control, counting then starts. Wait for the desired time and then counting disabled using {@link xsp3_histogram_stop}
 * It is possible to get the system to an armed start using {@link xsp3_histogram_arm} and then start counting with {@link xsp3_histogram_continue}.
 * With software timing it is possible to stop the current frame and then start counting into the next frame using {@link xsp3_histogram_pause()} and {@link xsp3_histogram_continue()}. @n
 * If the Internal time frame generator is used the system starts and the ITFG controls counting. It will start immediately unless trig_mode {@link XSP3_ITFG_TRIG_MODE} 
 * has been set to make it wait for an internal or external trigger. The ITFG will take a burst of frames, waiting for software or hardware triggers between frames if specified by the trig_mode. @n
 * If the timing control has been set to use the external timing inputs, counting is armed, but will occur only when the timing veto input is asserted.
 * If the external trigger is negated and then asserted again then acquisition moves onto the next frame. 
 * Progress of the current input time frame can be monitored using {@link xsp3_get_glob_time_statA()}. However, acquisition into memory will lag this slightly. 
 * If the internal time frame generator is being used, the progress is monitored using {@link xsp3_get_glob_time_statA()} and the status of the ITFG can be extracted from the upper bits using {@link XSP3_GLOBAL_TIME_STATUS_A}.
 * If timing if being controlled by hardware, the external timing generator must determine when the exposure is finished. After this call {@link xsp3_histogram_stop} to flush remaining data.
 
 * @subsubsection software_data_access Data Access
 * For XSPRESS3 and XSPRESS4 the histogram data is stored in /dev/shm shared memory on the Linux server. It can be read at any time, accepting that data read will be slightly stale 
 * while histogramming is running.
 * For early versions of XSPRESS3 the scalers are counted in VHDL scalars and transferred to DRAM in the FEM by DMA at the end of the frame. 
 * For current XSPRESS3 and XSPRESS4 versions, the scalars are accumulated into /dev/shm shared memory or calculated from the spectra as necessary.
 * In XSPRESS3Mini the normal spectra are collected into BRAM and transferred to ZYNQ PS DRAM by DMA at the end of the frame. The scalars are counted in VHDL scalars and transferred to DRAM by DMA.
 * To provide a common API to these implementations, user code should call {@link xsp3_scaler_check_progress_details()} or its simplified interface {@link xsp3_scaler_check_progress()}.
 * This will report the number of completed frames and it is safe to read data up to that point.
 * @n
 * Note that with the original XSPRESS3 version the intention was that while the experiment was running, realtime display data would be read out, accepting that it may be slightly stale. 
 * The intention was that at the end of the experiment {@link xsp3_histogram_stop} would be called to start flushing of the UDP data. 
 * After calling {@link xsp3_histogram_stop}, {@link xsp3_histogram_is_busy} or for a compatible interface for a multi-box system {@link xsp3_histogram_is_any_busy()} 
 * was to be polled until two consecutive calls returned not busy to check that the histogramming threads had flushed any network stack buffers. 
 * These functions are still supported for XSPRESS3 and XSPRRESS4, but have no meaning for XSPRESS3Mini. They are largely superseded by {@link xsp3_scaler_check_progress_details()}.
 * @n
 * However, to save time at the end of a many frame experiment, the EPICS and TANGO/LIAM implementations start reading early frames once the experiment has finished them.
 * For current XSPRSS3/XSPRESS4 and XSPRESS3Mini {@link xsp3_scaler_check_progress_details()} provides the information as to how far the system has got. Completed frames can safely be read.
 * For the original XSPRESS3 (scalar in FEM DRAM), {@link xsp3_scaler_check_progress_details()} measures progress by seeing how many time frames of scalars have been written to FEM-1 DRAM.
 * The scalars can safely be read out up to this frame. However, they may be some lag in the processing of the UDP data. Currently there is no way to verify when this has finished.
 * @n
 * The histogram data can then read read using any of {@link xsp3_histogram_read4d},  {@link xsp3_histogram_read3d} or  {@link xsp3_histogram_read_chan}
 * The raw scaler data is then read using {@link xsp3_scaler_read}
 *
 * @subsubsection circular_buffer_mode Circular Buffer Mode
 * To allow for experiments requiring very large numbers of time frames, the system can be operated in circular buffer mode. Normally the maximum number of time frames in an experiment is set when calling {@link xsp3_config()}
 * With a typical 16384 frames used in conventional 4096 bin spectra mode, this requires 256 MBytes of DRAM per channel. This can be increased significantly by installing more memory in the server and increasing the value passed to 
 * to {@link xsp3_config()}. There are limits in the library at 1 Meg frames, 16 GBytes per frame. 
 * @n
 * However the system can be used in circular buffer mode. The user software has to read the the data keeping up with incoming frame rate. It can lag, but only to the size of the number of frames in the circular buffer, 
 * typically 16384, and has to save the data to (networked) disk. This is enabled by calling {@link xsp3_set_run_flags()} including the flag XSP3_RUN_FLAGS_CIRCULAR_BUFFER
 * @n
 * The receive threads clear each time frame before they histograms into them. The receive threads also software extends the 24 bit firmware time frame to currently 64 bits. 
 * The bottom 32 bits of the extended time frame are used to address the histogram and scalar read functions. They are wrapped to read from the circular buffer. 
 * @n
 * There is also a time frames status buffer. This is an area in /dev/shm. It stores 4 off 32 bit unsigned integers per channel per circular buffer time frame.
 * One row per channel stores a used count which is incremented whenever a receive thread starts histogramming into a given frame. If when the receive thread wraps round to use that frame again, the used count has not been 
 * cleared by a call to {@link xsp3_histogram_circ_ack()}, the system detects wrap round. The new frame is collected and the old frame is over written. If this happens, it is recorded in various error status bits,
 * which can be seen by {@link xsp3_scaler_check_progress_details()} setting Xsp3ErrFlag_CircBuffOverRun in the error flags.
 * @n
 * The extended time frame stored in each circular buffer frame is stored in 2 rows of the time frame status module. 
 * The full status information for a frame/channel can be read using {@link xsp3_histogram_get_tf_status()}.
 * For use in particularly long raster scan experiments, there is an option to use TTL_IN(2..3) to give 2 markers in the data. These are saved in the 3rd row of the time frame status module and can be read back by 
 * {@link xsp3_histogram_get_tf_markers()}. This feature also exists in normal (single pass) buffer mode.
 * @n
 * The circular buffer mode is also available in XSPRESS3Mini, but the implementation is rather different. The circular buffer is within the Zynq DRAM, with the frames being written to the DRAM via AXIS DMA engines.
 * These are programmed to stop with an error rather overwrite the data. The API is preserved except that now calling {@link xsp3_histogram_circ_ack()} is mandatory. 
 * The API for {@link xsp3_histogram_get_tf_status} still exists, though it is now strongly encouraged to use {@link xsp3_histogram_get_tf_status_block()} as this is more efficient.
 *
 * @subsection registers Register Layout
 *
 * XSPRESS3 register space memory is mapped at 0xA0000000 on a 16 MByte boundary.
 * It is split into 16 “channels” using the top 4 bits of my address space.
 * Channel 0..7(8) will be physical channels and channel 15 is the global register.
 * The remaining channels 10..14 are reserved for future use.@n@n
 * Within a channel, the top next 4 bits down select a region.
 * Regions 0..8 (plus expansion) address block RAMs with varying sizes.
 * Region 15 contains 30 registers.
 * @snippet xspress3.h XSP3_REGIONS
 * Address Bits
 * @verbatim
 * 23..20	Channel Address
 * 19..16	Region (BRAM sel or Regs)
 * 15..2	BRAM addresses
 * 7 ..2	Register select
 * @endverbatim
 * The register layout of the channel registers is very similar in the XSPRESS3Mini and XSPRESS4 generations. 
 * The physical base address is 0x90000000 and the addressing scheme allows for up to 32 channels.
 * However, due to the virtual addresses in the Linux environment, the driver is passed a register offset rather than physical address.
 * The differences are accounted for in the API layer e.g. {@link xspress3FemSetInt()} so the functions {@link xsp3_read_reg()}and {@link xsp3_write_reg()} work for all generations.
 * @n
 * The global register layout is different between XSPRESS3 and XSPRESS3Mini/XSPRESS4. User code should not access these registers directly via {@link xsp3_read_glob_reg()} and {@link xsp3_write_glob_reg()}.
 * Instead the names accessors such as {@link xsp3_set_glob_timeA()} should be used.
 
 * @subsection network_config Network Configuration
 * The default network setup is (excluding the site network connection):
 *
 * @verbatim
 * 1GBit Copper network for control communication between the PC and the XSPRESS3/XSPRESS3Mini/XSPRESS4 box. 
 * With more than 1 XSPRESS3 box connected this network uses an Ethernet switch or bonded network interface cards.
 * A private network with 64 addresses allocated:
 * eth1      Link encap:Ethernet  HWaddr d4:ae:52:7d:5f:84
 *          inet addr:192.168.0.1  Bcast:192.168.0.63  Mask:255.255.255.192
 *          inet6 addr: fe80::d6ae:52ff:fe7d:5f84/64 Scope:Link
 *          UP BROADCAST RUNNING MULTICAST  MTU:9000  Metric:1
 *          RX packets:1567 errors:0 dropped:5766 overruns:0 frame:0
 *          TX packets:158 errors:0 dropped:0 overruns:0 carrier:0
 *          collisions:0 txqueuelen:1000
 *          RX bytes:173937 (169.8 KiB)  TX bytes:37252 (36.3 KiB)
 *          Interrupt:48 Memory:da000000-da012800
 *
 * A 10GBit Fibre network for data transfer, point to point with 4 addresses allocated. 
 * With more that 1 XSPRESS3/XSPRESS4 box there would be multiple 10G Ports on the PC with multiple 4 address range subnets
 *
 * ifconfig eth2
 * eth2      Link encap:Ethernet  HWaddr 00:07:43:05:7c:65
 *          inet addr:192.168.0.65  Bcast:192.168.0.67  Mask:255.255.255.252
 *          inet6 addr: fe80::207:43ff:fe05:7c65/64 Scope:Link
 *          UP BROADCAST RUNNING MULTICAST  MTU:9000  Metric:1
 *          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
 *          TX packets:702 errors:0 dropped:0 overruns:0 carrier:0
 *          collisions:0 txqueuelen:1000
 *          RX bytes:0 (0.0 B)  TX bytes:154963 (151.3 KiB)
 *          Interrupt:41 Memory:dd7fe000-dd7fefff
 *
 * Note the carefully picked subnet masks etc and the MTU 9000

 * We then have a script that should be executed automatically at boot.
 *
 * cat /etc/init.d/xspress3.sh
#!/bin/bash
#
# static-arp        This is to register a static ARP address in the arp table at boot
#
# Kept as simple as possible hopefully this will auto register the associated
# MAC with the private network address to allow the machine to communicate with the
# test boards for xspress3
#
# Derived from work by Duncan Russell, by William Helsby


PATH=/sbin:/bin:/usr/bin:/usr/sbin

arp -i eth2 -s 192.168.0.66 02:00:00:00:00:00
arp -i eth3 -s 192.168.0.70 02:00:00:00:00:02
arp -i eth4 -s 192.168.0.74 02:00:00:00:00:04
arp -i eth5 -s 192.168.0.78 02:00:00:00:00:06
#route -v add -host 192.168.0.66 eth2

# Setting default and max buffer sizes for networking.
sysctl -w net.core.rmem_max=1073741824
sysctl -w net.core.rmem_default=1073741824
 *
 * @endverbatim
 * @subsection errors Error Codes
 * The following error codes maybe returned;
 * @snippet xspress3.h XSP3_ERROR_CODES
* @section examples Example Programs
* A demonstration/example program has been written which uses {@link xsp3_scaler_check_progress_details()} to monitor progress and follow the experiment read data as completed frames become available.
* It can work with an external timing generator or the Internal TFG.
* @snippet test_check_desc.c XSP3_CHECK_PROGRESS_EXAMPLE
* @n
* @subsection circ_buff_example Circular Buffer Example Program
* An extensive test and demonstration program has been provided for circular buffer readout mode.
* @snippet test_circ_buffer.c XSP3_CIRC_BUFF_EXAMPLE

 * @section revisions "Revision History"
    1.0.0 Original Release. @n
	2.0.0  8/5/2014  API supporting event assembly processing in software and scalers in software.
		Note: User code should now check for histogramming threads busy using {@link xsp3_histogram_is_any_busy} rather than {@link xsp3_histogram_is_busy}
		as new function check all cards and all channels as necessary.
		User code should use {@link xsp3_scaler_check_progress} to check progress through time frames.@n
	2.1.0  17/1/2017  API adding supporting for XSPRESS3Mini and XSPRESS4. @n
	2.2.0  28/6/2018  API adding supporting for circular buffers in Xspress3Mini @n
		

 */

