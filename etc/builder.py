from iocbuilder import Device, AutoSubstitution, ModuleVersion
from iocbuilder.arginfo import *

from iocbuilder.modules.areaDetector import AreaDetector,_ADBase
from iocbuilder.modules.calc import Calc

class highlevel(AutoSubstitution):
     """Template containing the records for an NDPluginAttribute"""
     TemplateFile = 'xspress3_highlevel.template'
   

class _xspress3(AutoSubstitution):
    """Template containing the records for an xspress3"""
    TemplateFile = 'xspress3.template'

class xspress3(_ADBase):
    """Library dependencies for xspress3"""
    Dependencies = (AreaDetector,Calc)
    # Device attributes
    MakefileStringList = ['BOOST=/dls_sw/prod/tools/RHEL6-x86_64/boost/1-48-0/prefix',
                          'USR_LDFLAGS += -L$(BOOST)/lib/']
    LibFileList = ['img_mod', 'FemClient','Xspress3FemApi','xspress3','xspress3Epics', ]
    SysLibFileList = ['boost_thread', 'boost_system','rt']
    DbdFileList = ['xspress3','xspress3Support']
    _SpecificTemplate=_xspress3
    AutoInstantiate = True
#    boost=ModuleVersion( "boost",
#                         home="/dls_sw/prod/tools/RHEL6-x86_64/boost", 
#                         version="1-48-0" )

    def __init__(self,
                 BUFFERS=0, MEMORY=0, CHANNELS = 8, CARDS=1, BASE_IP="192.168.0.1", 
                 DEBUG=0, SIM=1, **args):
        # Init the superclass (_ADBase)
        self.__super.__init__(**args)
        # Store the args
        self.__dict__.update(locals())

    def Initialise(self):
        print """
##################################################
# Start Xspress3 driver

# Parameters
# portName The Asyn port name to use
# numChannels The max number of channels (eg. 8)
# numCards The number of Xspress3 systems (normally 1)
# baseIP The base address used by the Xspress3 1Gig and 10Gig interfaces (eg. "192.168.0.1")
# maxFrames The maximum number of frames that can be acquired in one acquisition (eg. 16384) 
# maxSpectra The maximum size of each spectra (eg. 4096)
# maxBuffers Used by asynPortDriver (set to -1 for unlimited)
# maxMemory Used by asynPortDriver (set to -1 for unlimited)
# debug This debug flag is passed to xsp3_config in the Xspress API (0 or 1)
# simTest 0 or 1. Set to 1 to run up this driver in simulation mode. """

        print \
            'xspress3Config("%(PORT)s", %(CHANNELS)d, %(CARDS)d, "%(BASE_IP)s", ' \
            '%(MAX_FRAMES)s, %(MAX_SPECTRA)s, %(BUFFERS)d, %(MEMORY)d, %(DEBUG)d, %(SIM)d )' % self.__dict__

    ArgInfo = (_xspress3.ArgInfo +
               _ADBase.ArgInfo + 
               makeArgInfo(__init__,
                           BUFFERS = Simple('Maximum number of NDArray buffers to be created for '
                                            'plugin callbacks', int),
                           MEMORY = Simple('Max memory to allocate, should be maxw*maxh*nbuffer '
                                           'for driver and all attached plugins', int),
                           CHANNELS= Simple('Input array queue size', int),
                           CARDS   = Simple('The number of Xspress3 systems (normally 1)', int),
                           BASE_IP = Simple('Input array queue size', str),
                           DEBUG   = Simple('Passed through to the Xspress3 via xsp3_config in the Xspress API', int),
                           SIM     = Simple('Set to 1 to run driver in simulation mode', int))
               )

