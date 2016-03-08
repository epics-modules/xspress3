import iocbuilder
import iocbuilder.modules.ADCore as ADCore
import iocbuilder.modules.asyn as asyn
from iocbuilder.arginfo import Simple


@ADCore.includesTemplates(ADCore.ADBaseTemplate)
class Xspress3Template(iocbuilder.AutoSubstitution):
    TemplateFile = "xspress3.template"


class Xspress3(asyn.AsynPort):
    """Create an Xspress3 driver"""
    Dependencies = (ADCore.ADCore,)
    UniqueName = "PORT"
    _SpecificTemplate = Xspress3Template
    LibFileList = ["img_mod", "xspress3", "xspress3Epics"]
    DbdFileList = ["xspress3Support"]

    def __init__(self, num_channels=1, num_cards=1, base_IP="192.168.0.1",
                 max_buffers=4096, max_memory=-1, debug=0, simulation=0,
                 **kwargs):
        super(Xspress3, self).__init__(kwargs["PORT"])
        self.__dict__.update(locals())
        # Make a dict that will populate the config string in Initialise()
        self.initialise_parameters = locals()
        self.initialise_parameters.update(kwargs)
        print self.initialise_parameters["PORT"]
        ADCore.makeTemplateInstance(Xspress3Template, {}, kwargs)

    ArgInfo = (
        ADCore.ADBaseTemplate.ArgInfo +
        _SpecificTemplate.ArgInfo +
        iocbuilder.makeArgInfo(
            __init__,
            num_channels=Simple("Number of channels in the Xspress3", int),
            num_cards=Simple("Number of Xspress3 devices in the chain", int),
            base_IP=Simple("Xspress3 base address", str),
            max_buffers=Simple("Number of NDArray buffers to allocate", int),
            max_memory=Simple("Memory to allocate for driver and plugins",
                              int),
            debug=Simple("Turn on support library debugging", int),
            simulation=Simple("Run a simulation detector", int)))

    def Initialise(self):
        print("# xspress3Config(portName, numChannels, numCards, baseIP, "
              "maxFrames, maxSpectra, maxBuffers, maxMemory, debug, simTest)")
        print("xspress3Config('{PORT}', {num_channels}, {num_cards}, "
              "'{base_IP}', {MAX_FRAMES}, {MAX_SPECTRA}, {max_buffers}, "
              "{max_memory}, {debug}, {simulation})".format(
                  **self.initialise_parameters))
