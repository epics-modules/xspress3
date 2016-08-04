# Required input: Number of channels

# Optional inputs: Number of ROIs per channel, port basename (may not work),
# same things as with builder

# Generate substitutions file with: xspress3.template,
# xspress3_highlevel.template and availableFrame if required (was just made for
# GDA so probably not). Per channel xspress3Channel.template,
# xspress3_updateFlag.template and as many _Xspress3NDROIStatN's as required

# Generate startup script with xspress3Config, NDProcessConfigure,
# NDFileHDF5Configure and per channel 2 x NDROIConfigure, 2 x
# NDStdArraysConfigure, NDAttrConfigure and 2 x NDROIStatConfigure. Finally
# dbpfs for the AttrNames and possibly configuration directory.

import argparse


def _parse_arguments():
    parser = argparse.ArgumentParser(
        description='Create a substitution file and startup '
        'script for an xspress3 IOC')
    parser.add_argument('n_channels', help='Number of xspress3 channels',
                        type=int)
    parser.add_argument('-r', '--rois', help='Number of ROIs per channel',
                        type=int)
    parser.add_argument('-p', '--prefix1', help='First PV prefix')
    parser.add_argument('-s', '--prefix2', help='Second PV prefix')
    parser.add_argument('--cards',
                        help='Number of cards in the system, defaults to 1',
                        type=int)
    parser.add_argument('--max_frames',
                        help='Maximum frames per row, defaults to 16384',
                        type=int)
    parser.add_argument('-b', '--substitution',
                        help='Substitution file name')
    parser.add_argument('-t', '--startup_script',
                        help='Iocsh startup script file name')
    parser.add_argument('-i', '--post_init',
                        help='Iocsh post iocInit file name')
    return parser.parse_args()


class EPICSThing:
    def generate_config(self):
        pass

    def generate_post_init(self):
        pass

    def generate_substitutions(self):
        pass


class Xspress3(EPICSThing):
    def __init__(self, n_channels, rois_per_channel=4, prefix1='XSPRESS3',
                 prefix2=':', max_frames=16384, debug=0, simulation=False):
        self.n_channels = n_channels
        self.rois_per_channel = rois_per_channel
        self.prefix1 = prefix1
        self.prefix2 = prefix2
        self.xsp_port = 'XSP3'
        self.xsp_port_addr = 0
        self.timeout = 5
        self.n_cards = 1
        self.base_ip = '192.168.0.1'
        self.max_frames = max_frames
        self.max_spectra = 4096
        self.max_buffers = 4096
        self.max_memory = -1
        self.debug = debug
        if simulation:
            self.simulation = 1
        else:
            self.simulation = 0
        self.queue_size = 4096
        self.blocking_callbacks = 0
        self._make_channels()

    def generate_config(self):
        print('# xspress3Config(portName, numChannels, numCards, baseIP, '
              'maxFrames, maxSpectra, maxMemory, debug, simTest)')
        print('xspress3Config({}, {}, {}, {}, {}, {}, {}, {}, {}, {})'.format(
            self.xsp_port, self.n_channels, self.n_cards, self.base_ip,
            self.max_frames, self.max_spectra, self.max_buffers,
            self.max_memory, self.debug, self.simulation))
        self._roidata_config()
        self._hdf5_config()
        self._proc_config()
        for channel in self.channels:
            channel.generate_config()

    def generate_substitutions(self):
        self._xspress3_template()
        self._xspress3_highlevel()
        for channel in self.channels:
            channel.generate_substitutions()

    def _xspress3_template(self):
        print('''
file xspress3.template
{{
pattern {{ P, R, PORT, ADDR, TIMEOUT, MAX_SPECTRA, MAX_FRAMES }}
        {{ "{}", "{}", "{}", {}, {}, {}, {} }}
}}
'''.format(self.prefix1, self.prefix2, self.xsp_port, self.xsp_port_addr,
           self.timeout, self.max_spectra, self.max_frames))

    def _xspress3_highlevel(self):
        print('''
file xspress3_highlevel.template
{{
pattern {{ P, R, HDF, PROC, XSP3_PORT, XSP3_ADDR, TIMEOUT, ADDR }}
        {{ "{}", "{}", "{}", "{}", "{}", {}, {}, {} }}
}}
'''.format(self.prefix1, self.prefix2,
           '{}{}HDF5:'.format(self.prefix1, self.prefix2),
           '{}{}PROC:'.format(self.prefix1, self.prefix2),
           self.xsp_port, self.xsp_port_addr, self.timeout,
           self.xsp_port_addr))

    def _roidata_config(self):
        print('''
# NDROIConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDROIConfigure("{}.ROIDATA", {}, {}, "{}", {}, {}, {})
'''.format(self.xsp_port, self.queue_size, self.blocking_callbacks,
           self.xsp_port, self.xsp_port_addr, self.max_buffers,
           self.max_memory))
            
    def _hdf5_config(self):
        print('''
# NDFileHDF5Configure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDFileHDF5Configure("{}.HDF5", {}, {}, "{}.ROIDATA", {}, {}, {})
'''.format(self.xsp_port, self.queue_size, self.blocking_callbacks,
           self.xsp_port, self.xsp_port_addr, self.max_buffers, self.max_memory))

    def _proc_config(self):
        print('''
# NDProcessConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDProcessConfigure("{}.PROC", {}, {}, "{}", {}, {}, {})
'''.format(self.xsp_port, self.queue_size, self.blocking_callbacks,
           self.xsp_port, self.xsp_port_addr, self.max_buffers, self.max_memory))

    def _make_channels(self):
        self.channels = [
            Xspress3Channel(channel, self.xsp_port, self.rois_per_channel)
            for channel in range(1, self.n_channels+1)]

    def generate_post_init(self):
        self._proc_post_init()
        self._roidata_post_init()
        self._hdf5_post_init()

    def _proc_post_init(self):
        print('''
dbpf({P}{R}PROC:EnableCallbacks Enable)
dbpf({P}{R}PROC:FilterType Sum)
dbpf({P}{R}PROC:EnableFilter Enable)
'''.format(P=self.prefix1, R=self.prefix2))

    def _hdf5_post_init(self):
        print('''
dbpf({P}{R}HDF5:EnableCallbacks Enable)
'''.format(P=self.prefix1, R=self.prefix2))

    def _roidata_post_init(self):
        print('''
dbpf({P}{R}ROIDATA:EnableCallbacks Enable)
'''.format(P=self.prefix1, R=self.prefix2))


class Xspress3Channel(EPICSThing):
    def __init__(self, channel_number, xsp_port='XSP3', rois_per_channel=8,
                 prefix1='XSPRESS3', prefix2=':'):
        self.channel_number = channel_number
        self.xsp_port = xsp_port
        self.rois_per_channel = rois_per_channel
        self.prefix1 = prefix1
        self.prefix2 = prefix2
        self.queue_size = 4096
        self.blocking_callbacks = 0
        self.ndarray_addr = 0
        self.max_buffers = 4096
        self.max_memory = 0
        self.n_elements = 4096
        self.timeout = 5
        self.port_addr = 0
        self.n_scalers = 8
        self.roi_stat_port = '{}.ROISTAT{}'.format(xsp_port, channel_number)
        self.roi_sumstat_port = '{}.ROISUMSTAT{}'.format(xsp_port,
                                                         channel_number)

    def generate_config(self):
        channel_roi = '{}.ROI{}'.format(self.xsp_port, self.channel_number)
        self._roi_config(channel_roi, self.xsp_port)
        self._stdarrays_config('{}.ARR{}'.format(self.xsp_port,
                                                 self.channel_number),
                               channel_roi)
        sum_roi = '{}.ROISUM{}'.format(self.xsp_port, self.channel_number)
        self._roi_config(sum_roi, self.xsp_port)
        self._stdarrays_config('{}.ARRSUM{}'.format(self.xsp_port,
                                                    self.channel_number),
                               sum_roi)
        self._attr_config('{}.C{}_SCAS'.format(self.xsp_port,
                                               self.channel_number),
                          self.xsp_port)
        self._roistat_config(self.roi_stat_port, channel_roi)
        self._roistat_config(self.roi_sumstat_port, sum_roi)

    def generate_substitutions(self):
        self._channel_substitutions()
        self._roistatn_substitutions()

    def _roi_config(self, port, ndarray_port):
        print('''
# NDROIConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxBuffers, maxMemory)
NDROIConfigure("{}", {}, {}, "{}", {}, {}, {})
'''.format(port, self.queue_size, self.blocking_callbacks, ndarray_port,
           self.ndarray_addr, self.max_buffers, self.max_memory))

    def _stdarrays_config(self, port, ndarray_port):
        print('''
# NDStdArraysConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxMemory)
NDStdArraysConfigure("{}", {}, {}, "{}", {}, {})
'''.format(port, self.queue_size, self.blocking_callbacks, ndarray_port,
           self.ndarray_addr, self.max_buffers, self.max_memory))

    def _attr_config(self, port, ndarray_port):
        print('''
# NDAttrConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxAttributes, maxBuffers, maxMemory)
NDAttrConfigure("{}", {}, {}, "{}", {}, {}, {}, {})
'''.format(port, self.queue_size, self.blocking_callbacks, ndarray_port,
           self.ndarray_addr, self.n_scalers, self.max_buffers,
           self.max_memory))

    def _roistat_config(self, port, ndarray_port):
        print('''
# NDROIStatConfigure(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr, maxROIs, maxBuffers, maxMemory)
NDROIStatConfigure("{}", {}, {}, "{}", {}, {}, {}, {})
'''.format(port, self.queue_size, self.blocking_callbacks, ndarray_port,
           self.ndarray_addr, self.rois_per_channel, self.max_buffers,
           self.max_memory))

    def _channel_substitutions(self):
        print('''
file xspress3Channel.template
{{
pattern {{ P, S, XSP3_PORT, ADDR, TIMEOUT, CHAN, NELEMENTS, INDEX }}
        {{ "{}", "{}", "{}", {}, {}, {}, {}, {} }}
}}
'''.format(self.prefix1, self.prefix2, self.xsp_port, self.port_addr,
           self.timeout, self.channel_number, self.n_elements,
           self.channel_number+1))

    def _roistatn_substitutions(self):
        print('''
file xspress3_NDROIStatN.template
{
pattern { P, S, CHAN, ROI, XSP3_PORT, NCHANS, ADDR, TIMEOUT }
''')
        for roistatn in range(1, self.rois_per_channel+1):
            print('{{ "{}", "{}", {}, {}, "{}", {}, {}, {}}}\n'.format(
                self.prefix1, self.prefix2, self.channel_number, roistatn,
                self.xsp_port, self.n_elements, roistatn-1, self.timeout))
        print('}\n')

    def generate_post_init(self):
        self._scas_post_init()

    def _scas_post_init(self):
        for sca in range(self.n_scalers):
            print('dbpf({}{}{}:AttrName, Chan{}Sca{}'.format(
                self.prefix1, self.prefix2, sca+1, self.channel_number, sca))


if __name__ == '__main__':
    args = _parse_arguments()
    print('{} channels, {} rois'.format(args.n_channels, args.rois))
    xsp3 = Xspress3(args.n_channels, args.rois)
    if args.substitution:
        subs_file = open(args.substitution, 'w').write
    else:
        subs_file = print
    if args.startup_script:
        start_file = open(args.startup_script, 'w').write
    else:
        start_file = print
    if args.post_init:
        post_init_file = open(args.post_init, 'w').write
    else:
        post_init_file = print
    print = start_file
    xsp3.generate_config()
    print = post_init_file
    xsp3.generate_post_init()
    print = subs_file
    xsp3.generate_substitutions()
    
