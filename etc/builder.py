import iocbuilder
import iocbuilder.modules.ADCore as ADCore
import iocbuilder.modules.asyn as asyn
from iocbuilder.arginfo import Simple


def override_makeTemplateInstance(ad_object, *args, **kwargs):
    """Create ad_object with no template.

    If a template is included in another template but you would like
    builder to take care of the startup script this will stop ad_object
    creating a template."""
    makeTemplateInstance = ADCore.makeTemplateInstance
    ADCore.makeTemplateInstance = lambda *args: None
    ad_thing = ad_object(*args, **kwargs)
    ADCore.makeTemplateInstance = makeTemplateInstance
    return ad_thing


@ADCore.includesTemplates(ADCore.ADBaseTemplate)
class _Xspress3Template(iocbuilder.AutoSubstitution):
    TemplateFile = "xspress3.template"


class Xspress3(asyn.AsynPort):
    """Create an Xspress3 driver"""
    Dependencies = (ADCore.ADCore,)
    UniqueName = "PORT"
    _SpecificTemplate = _Xspress3Template
    LibFileList = ["img_mod", "xspress3", "xspress3Epics"]
    DbdFileList = ["xspress3Support"]

    def __init__(self, PORT, num_channels=1, num_cards=1,
                 base_IP="192.168.0.1", max_buffers=4096, max_memory=-1,
                 settings_directory="", debug=0, simulation=0, **kwargs):
        super(Xspress3, self).__init__(PORT)
        self.__dict__.update(locals())
        # Get the template arguments in the local scope without
        # duplicating documentation
        self.__dict__.update(**kwargs)
        ADCore.makeTemplateInstance(_Xspress3Template, locals(), kwargs)

    ArgInfo = (
        ADCore.ADBaseTemplate.ArgInfo +
        _SpecificTemplate.ArgInfo +
        iocbuilder.makeArgInfo(
            __init__,
            PORT=Simple("Name of the asyn port for this device", str),
            num_channels=Simple("Number of channels in the Xspress3", int),
            num_cards=Simple("Number of Xspress3 devices in the chain", int),
            base_IP=Simple("Xspress3 base address", str),
            max_buffers=Simple("Number of NDArray buffers to allocate", int),
            max_memory=Simple("Memory to allocate for driver and plugins",
                              int),
            settings_directory=Simple(
                "Path to the Xspress3 settings to upload to the device", str),
            debug=Simple("Turn on support library debugging", int),
            simulation=Simple("Run a simulation detector", int)))

    def Initialise(self):
        print("# xspress3Config(portName, numChannels, numCards, baseIP, "
              "maxFrames, maxSpectra, maxBuffers, maxMemory, debug, simTest)")
        print("xspress3Config('{PORT}', {num_channels}, {num_cards}, "
              "'{base_IP}', {MAX_FRAMES}, {MAX_SPECTRA}, {max_buffers}, "
              "{max_memory}, {debug}, {simulation})".format(
                  **self.__dict__))

    def PostIocInitialise(self):
        print("dbpf({}{}CONFIG_PATH '{}')".format(self.P, self.R,
                                                  self.settings_directory))


class _Xspress3AvailableFrameTemplate(iocbuilder.AutoSubstitution):
    TemplateFile = "xspress3_AvailableFrame.template"


class _Xspress3HighlevelTemplate(iocbuilder.AutoSubstitution):
    TemplateFile = "xspress3_highlevel.template"


class _Xspress3UpdateFlagTemplate(iocbuilder.AutoSubstitution):
    TemplateFile = "xspress3_updateFlag.template"


class _Xspress3NDROIStatNTemplate(iocbuilder.AutoSubstitution):
    TemplateFile = "xspress3_NDROIStatN.template"


class _Xspress3NDROIStat(ADCore.NDROIStat):
    """Create an ROIStat plugin for an Xspress3 channel.

    Override ADCore NDROIStat to use xspress3_NDROIStatN templates
    instead

    """
    def __init__(self, channel_num, xsp3_port, S, *args, **kwargs):
        self.channel_num = channel_num
        self.S = S
        self.xsp3_port = xsp3_port
        if "roi_prefix" in kwargs:
            self.roi_prefix = kwargs["roi_prefix"]
        else:
            self.roi_prefix = ""
        super(_Xspress3NDROIStat, self).__init__(*args, S=S, **kwargs)

    def _create_roi_stat_n_templates(self):
        for roi in range(self.MAX_ROIS):
            _Xspress3NDROIStatNTemplate(
                P=self.args["P"], S=self.S, NCHANS=self.NCHANS,
                XSP3_PORT=self.xsp3_port, ADDR=roi, TIMEOUT=self.TIMEOUT,
                CHAN=self.channel_num,
                ROI="{}{}".format(self.roi_prefix, roi+1))


class _Xspress3ChannelTemplate(iocbuilder.AutoSubstitution):
    TemplateFile = "xspress3Channel.template"


class _Xspress3Channel(iocbuilder.Device):
    """Create the plugins for a typical Xspress3"""
    def __init__(self, channel_num, num_rois, parent):
        super(_Xspress3Channel, self).__init__()
        self.channel_num = channel_num
        self.num_rois = num_rois
        self.parent = parent
        self.typical_args = {"P": parent.P, "TIMEOUT": parent.TIMEOUT, "CHAN":
                             channel_num, "INDEX": channel_num-1}
        self._create_channel()
        # All of the templates are created in the channel template so
        # override makeTemplateInstance in ADCore to stop plugins from
        # creating more database
        override_makeTemplateInstance(self._create_roi_stats)
        override_makeTemplateInstance(self._create_update_flag)

    def _create_channel(self):
        _Xspress3ChannelTemplate(S=":", R=":", XSP3_PORT=self.parent.PORT,
                                 ADDR=self.parent.ADDR, PORT="",
                                 NELEMENTS=self.parent.max_buffers,
                                 **self.typical_args)

    def _create_update_flag(self):
        p = self.parent.P
        r = ":"
        _Xspress3UpdateFlagTemplate(
            P=p, R=r, CHAN=self.channel_num,
            PREV=(-2 if self.channel_num == 1 else
                  "{}{}C{}:AverageId CPP".format(p, r, self.channel_num-1)))

    def _create_roi_stats(self):
        _Xspress3NDROIStat(
            self.channel_num, self.parent.PORT, ":",
            "{}.ROISTAT{}".format(self.parent.PORT, self.channel_num),
            "{}.ROI{}".format(self.parent.PORT, self.channel_num),
            ADDR=0, NCHANS=self.parent.max_buffers, P=self.parent.P,
            TIMEOUT=self.parent.TIMEOUT, QUEUE=self.parent.max_buffers,
            BUFFERS=self.parent.max_buffers, MAX_ROIS=self.num_rois)
        roi_func = _Xspress3NDROIStat._create_roi_stat_n_templates
        _Xspress3NDROIStat._create_roi_stat_n_templates = lambda x: None
        _Xspress3NDROIStat(
            self.channel_num, self.parent.PORT, ":",
            "{}.ROISUMSTAT{}".format(self.parent.PORT, self.channel_num),
            "{}.ROISUM{}".format(self.parent.PORT, self.channel_num),
            roi_prefix="SUM", ADDR=0, NCHANS=self.parent.max_buffers,
            P=self.parent.P, TIMEOUT=self.parent.TIMEOUT,
            QUEUE=self.parent.max_buffers, BUFFERS=self.parent.max_buffers,
            MAX_ROIS=self.num_rois)
        _Xspress3NDROIStat._create_roi_stat_n_templates = roi_func

    def Initialise(self):
        roi_port = "{}.ROI{}".format(self.parent.PORT, self.channel_num)
        makeTemplateInstance = ADCore.makeTemplateInstance
        ADCore.makeTemplateInstance = lambda *args: None
        ADCore.NDROI(roi_port, self.parent.PORT,
                     QUEUE=self.parent.max_buffers).Initialise()
        ADCore.NDStdArrays(
            "{}.ARR{}".format(self.parent.PORT, self.channel_num),
            roi_port, QUEUE=self.parent.max_buffers).Initialise()
        roisum_port = "{}.ROISUM{}".format(self.parent.PORT, self.channel_num)
        ADCore.NDROI(roisum_port, self.parent.PORT+".PROC",
                     QUEUE=self.parent.max_buffers).Initialise()
        ADCore.NDStdArrays(
            "{}.ARRSUM{}".format(self.parent.PORT, self.channel_num),
            roisum_port, QUEUE=self.parent.max_buffers).Initialise()
        ADCore.NDAttribute(
            "{}.C{}_SCAS".format(self.parent.PORT, self.channel_num),
            self.parent.PORT, NDARRAY_ADDR=0, MAX_ATTRIBUTES=8,
            QUEUE=self.parent.max_buffers,
            TIMEOUT=self.parent.TIMEOUT, P=self.parent.P,
            R=":C{}_SCAS".format(self.channel_num)).Initialise()
        ADCore.makeTemplateInstance = makeTemplateInstance

    def PostIocInitialise(self):
        for scaler in range(8):
            print("dbpf({}, Chan{}Sca{})".format(
                "{}:C{}_SCAS:{}:AttrName".format(self.parent.P,
                                                 self.channel_num,
                                                 scaler+1),
                self.channel_num, scaler))


class Xspress3WithPlugins(Xspress3):
    """Create an Xspress3 driver and common plugins.

    Create an hdf file writer, processing plugin and the defined number
    of Xspress3Channels.

    """
    def __init__(self, num_rois, P, PORT, ADDR, *args, **kwargs):
        xspress3 = super(Xspress3WithPlugins, self)
        xspress3.__init__(PORT, *args, P=P, ADDR=ADDR, **kwargs)
        self.P = P
        self.PORT = PORT
        self.num_rois = num_rois
        self.typical_args = {"P": P, "QUEUE": self.max_buffers, "NDARRAY_ADDR":
                             ADDR, "BUFFERS": self.max_buffers, "MEMORY":
                             self.max_memory, "ADDR": 0, "TIMEOUT": 5}
        override_makeTemplateInstance(self._create_proc)
        override_makeTemplateInstance(self._create_hdf_writer)
        self._create_highlevel()
        self._create_channels()
        self._create_available_frame()

    ArgInfo = (
        Xspress3.ArgInfo +
        iocbuilder.makeArgInfo(
            num_rois=Simple("Number of ROIStats per channel", int)))

    def _create_channels(self):
        for channel in range(1, self.num_channels+1):
            _Xspress3Channel(channel, self.num_rois, self)

    def _create_hdf_writer(self):
        prefix_r = ":HDF5:"
        hdf = ADCore.NDFileHDF5(self.PORT+".HDF5", self.PORT+".ROIDATA",
                                R=prefix_r, **self.typical_args)

        def post_ioc_initialise():
            print("dbpf({}{}EnableCallbacks Enable)".format(self.P, prefix_r))
        hdf.PostIocInitialise = post_ioc_initialise

    def _create_proc(self):
        prefix_r = ":PROC:"
        proc = ADCore.NDProcess(self.PORT+".PROC", self.PORT, R=prefix_r,
                                **self.typical_args)

        def post_ioc_initialise():
            print("dbpf({}{}EnableCallbacks Enable)".format(self.P, prefix_r))
            print("dbpf({}{}FilterType Sum)".format(self.P, prefix_r))
            print("dbpf({}{}EnableFilter Enable)".format(self.P, prefix_r))
        proc.PostIocInitialise = post_ioc_initialise

    def _create_highlevel(self):
        hdf_port = "{}.ROIDATA".format(self.PORT)
        _Xspress3HighlevelTemplate(P=self.P, R=":", HDF=self.P+":HDF5:",
                                   PROC=self.P+":PROC:",
                                   XSP3_PORT=self.PORT,
                                   XSP3_ADDR=self.typical_args["ADDR"],
                                   TIMEOUT=self.typical_args["TIMEOUT"],
                                   ADDR=self.typical_args["ADDR"])
        roi_data = override_makeTemplateInstance(
            ADCore.NDROI, hdf_port, self.PORT, R=":ROIDATA:",
            QUEUE=self.typical_args["QUEUE"])

        def post_ioc_initialise():
            print("dbpf({}:ROIDATA:EnableCallbacks Enable)".format(self.P))
        roi_data.PostIocInitialise = post_ioc_initialise

    def _create_available_frame(self):
        _Xspress3AvailableFrameTemplate(P=self.P, R=":",
                                        CHANNELS=self.num_channels)
