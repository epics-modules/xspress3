from iocbuilder import Device, AutoSubstitution, ModuleVersion
from iocbuilder.arginfo import *

from iocbuilder import ModuleBase
from iocbuilder.modules.areaDetector import AreaDetector,_ADBase
from iocbuilder.modules.calc import Calc
from iocbuilder.modules.spectraPlugins import SpectraPlugins,NDPluginAttribute

class highlevel(AutoSubstitution):
     """Template containing the records for an NDPluginAttribute"""
     TemplateFile = 'xspress3_highlevel.template'
   

class _xspress3(AutoSubstitution):
    """Template containing the records for an xspress3"""
    TemplateFile = 'xspress3.template'

class _xspress3Channel(AutoSubstitution):
    """Template containing the records for an xspress3Channel"""
    TemplateFile = 'xspress3Channel.template'

class xspress3Channel(ModuleBase):
    """Library dependencies for xspress3"""

    Dependencies = (SpectraPlugins,)
    _SpecificTemplate=_xspress3Channel
    AutoInstantiate = True

    def __init__(self, **args):
        # Init the superclass (_ADBase)
        self.__super.__init__(**args)

        # Store the args
        if self._SpecificTemplate:
            self.template = self._SpecificTemplate(**filter_dict(args,
                                                                 self._SpecificTemplate.ArgInfo.Names()))
            self.__dict__.update(self.template.args)

        self.__dict__.update(locals())

        # Make a shallow copy and remove the arguments we don't want
        NDPluginArgs = args.copy()
        del NDPluginArgs['P']
        del NDPluginArgs['R']
        del NDPluginArgs['PORT']
        del NDPluginArgs['CHAN']
        NDPluginArgs['QUEUE']=4096
        NDPluginArgs['BUFFERS']=-1
        NDPluginArgs['MEMORY']=-1
        NDPluginArgs['TIMEOUT']=1
        NDPluginArgs['ADDR']=0
        CHAN = args['CHAN']

        for i in range(8):
             NDPluginAttribute( ATTR_NAME="CHAN%dSCA%d"%(CHAN, i),
                                PORT="XSP3.C%d_SCA%d"%(CHAN, i),
                                P=args['P']+args['R'],
                                R="C%d_SCA%d:"%(CHAN, i),
                                NDARRAY_ADDR=0,
                                Enabled=1,
                                **NDPluginArgs)
                                
        for i in range( 1,17 ):
             Enabled=1 if i<5 else 0
             NDPluginAttribute( ATTR_NAME="CHAN%dROI%d"%(CHAN, i),
                                PORT="XSP3.C%d_ROI%d"%(CHAN, i),
                                P=args['P']+args['R'],
                                R="C%d_ROI%d:"%(CHAN, i),
                                NDARRAY_ADDR=0,
                                Enabled=Enabled,
                                **NDPluginArgs )
  
#    def Initialise(self):

    ArgInfo = (_xspress3Channel.ArgInfo + 
               makeArgInfo(__init__,
                           R    = Simple('Record Suffix', str),
                           CHAN = Simple('The channel number', int),
                           NELEMENTS = Simple('The number of elements', int),
                           NDARRAY_PORT = Simple('Xspress3 Asyn port', str)))

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

