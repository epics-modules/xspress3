from os.path import dirname, abspath
from iocbuilder import Device, AutoSubstitution, ModuleVersion
from iocbuilder.arginfo import *

from iocbuilder import ModuleBase
from iocbuilder.modules.areaDetector import AreaDetector,_ADBase,NDAttributes, NDROI, NDStdArrays
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

    Dependencies = (SpectraPlugins,AreaDetector,)
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
        NDPluginArgs['P']=args['P']+args['R']
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
                                R="C%d_SCA%d:"%(CHAN, i),
                                NDARRAY_ADDR=0,
                                Enabled=1,
                                **NDPluginArgs)

        for i in range( 1,17 ):
             Enabled=1 if i<5 else 0
             NDPluginAttribute( ATTR_NAME="CHAN%dROI%d"%(CHAN, i),
                                PORT="XSP3.C%d_ROI%d"%(CHAN, i),
                                R="C%d_ROI%d:"%(CHAN, i),
                                NDARRAY_ADDR=0,
                                Enabled=Enabled,
                                **NDPluginArgs )

        del NDPluginArgs["NDARRAY_PORT"]
        NDROI( PORT="%s.ROI%d"%(args["PORT"],CHAN),    R="ROI%d:"%(CHAN),    NDARRAY_PORT=args["PORT"],     NDARRAY_ADDR=0, **NDPluginArgs )
        NDROI( PORT="%s.ROISUM%d"%(args["PORT"],CHAN), R="ROISUM%d:"%(CHAN), NDARRAY_PORT=args["PORT"]+".PROC",NDARRAY_ADDR=0, **NDPluginArgs )
        NDStdArrays( PORT="%s.ARR%d"%(args["PORT"],CHAN),    R="ARR%d:"%(CHAN),    
                     NDARRAY_PORT="%s.ROI%d"%(args["PORT"],CHAN), NDARRAY_ADDR=0,
                     TYPE="Float64", FTVL="DOUBLE", 
                     **NDPluginArgs )
        NDStdArrays( PORT="%s.ARRSUM%d"%(args["PORT"],CHAN), R="ARRSUM%d:"%(CHAN),
                     NDARRAY_PORT="%s.ROISUM%d"%(args["PORT"],CHAN), NDARRAY_ADDR=0,
                     TYPE="Float64", FTVL="DOUBLE", 
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
    LibFileList = ['img_mod','xspress3','xspress3Epics', ]
    SysLibFileList = ['rt']
    DbdFileList = ['xspress3','xspress3Support']
    _SpecificTemplate=_xspress3
    AutoInstantiate = True

    def __init__(self,
                 BUFFERS=0, MEMORY=0, CHANNELS = 8, CARDS=1, BASE_IP="192.168.0.1", 
                 MAX_DRIVER_FRAMES=0, DEBUG=0, SIM=1, SETTINGS="", DATAPATH="", **args):
        # Init the superclass (_ADBase)
        self.__super.__init__(**args)
        # Store the args
        self.__dict__.update(locals())
 
        fileparams = [("RingCurrent",  "EPICS_PV", "SR-DI-DCCT-01:SIGNAL", "DOUBLE", "Ring Current"), 
                      ("ImageCounter", "PARAM",    "ARRAY_COUNTER" ,       "INT",    "Image counter"),
                      ("Model",        "PARAM",    "MODEL",                "STRING", "Model"),
                      ("Manufacturer", "PARAM",    "MANUFACTURER",         "STRING", "Manufacturer") ]
                   
        for (name, ptype, source, dtype, desc) in fileparams:
             NDAttributes(port=self, 
                          source=source, attrname=name, 
                          type=ptype, datatype=dtype, 
                          description=desc)

        limitparams=[( "LLM", "Low Limit"),
                     ( "HLM", "High Limit") ]
        chanparams =[( "_DTC_FLAGS", "INT",    "DTC Flags" ),
                     ( "_DTC_AEG",   "DOUBLE", "DTC All Good Event Grad" ),
                     ( "_DTC_AEO",   "DOUBLE", "DTC All Good Event Offset" ),
                     ( "_DTC_IWG",   "DOUBLE", "DTC In Window Grad" ),
                     ( "_DTC_IWO",   "DOUBLE", "DTC In Window Offset" ) ]

        for CHAN in range(1,CHANNELS+1):
             xspress3Channel(P=self.P, R=self.R, PORT=self.PORT, ADDR=CHAN-1, TIMEOUT=self.TIMEOUT, 
                             CHAN=CHAN, NDARRAY_PORT=self.PORT, NELEMENTS=4096 )

             for i in range(8):
                  NDAttributes(port=self,
                               attrname="CHAN%dSCA%d"%(CHAN, i),
                               addr=CHAN-1,
                               type="PARAM",
                               source="XSP3_CHAN_SCA%d"%(i),
                               datatype="DOUBLE",
                               description="Chan %d SCA %d"%(CHAN, i) )

             for ( abbr, dtype, text ) in chanparams:
                  NDAttributes(port=self,
                               attrname="CHAN%d%s"%(CHAN, abbr.replace("_","")),
                               addr=CHAN-1,
                               type=dtype,
                               source="XSP3_CHAN%s"%(abbr),
                               datatype="DOUBLE",
                               description="Chan %d %s"%(CHAN, text) )
                 

             for i in range( 1,17 ):
                  NDAttributes(port=self,
                               attrname="CHAN%dROI%d"%(CHAN, i),
                               addr=CHAN-1,
                               type="PARAM",
                               source="XSP3_CHAN_ROI%d"%(i),
                               datatype="DOUBLE",
                               description="Chan %d ROI %d"%(CHAN, i) )
                  for ( abbr, text ) in limitparams:
                      NDAttributes(port=self,
                                   attrname="CHAN%dROI%d%s"%(CHAN, i,abbr),
                                   addr=CHAN-1,
                                   type="PARAM",
                                   source="XSP3_CHAN_ROI%d_%s"%(i,abbr),
                                   datatype="INT",
                                   description="Chan %d ROI %d %s"%(CHAN, i, text) )


    def Initialise(self):
        print """
##################################################
# Start Xspress3 driver

# Parameters
# portName The Asyn port name to use
# numChannels The max number of channels (eg. 8)
# numCards The number of Xspress3 systems (normally 1)
# baseIP The base address used by the Xspress3 1Gig and 10Gig interfaces (eg. "192.168.0.1")
# Max Frames Used for XSPRESS3 API Configuration (depends on Xspress3 firmware config).
# Max Driver Frames Used to limit how big EPICS driver arrays are). Needs to match database MAX_FRAMES_DRIVER_RBV.
# maxSpectra The maximum size of each spectra (eg. 4096)
# maxBuffers Used by asynPortDriver (set to -1 for unlimited)
# maxMemory Used by asynPortDriver (set to -1 for unlimited)
# debug This debug flag is passed to xsp3_config in the Xspress API (0 or 1)
# simTest 0 or 1. Set to 1 to run up this driver in simulation mode. """

        print \
            'xspress3Config("%(PORT)s", %(CHANNELS)d, %(CARDS)d, "%(BASE_IP)s", ' \
            '%(MAX_FRAMES)s, %(MAX_DRIVER_FRAMES)s, %(MAX_SPECTRA)s, %(BUFFERS)d, %(MEMORY)d, %(DEBUG)d, %(SIM)d )' % self.__dict__

    def PostIocInitialise(self):
        # This is really horrible, but until I can get the defaults sorted, this is the best I can do 
        temp_dict=self.__dict__.copy()
        temp_dict["XSP3_TOP"] = dirname(dirname(abspath(__file__)))
        print """
#Set up some of the more complicated HDF PVs
dbpf("%(P)s:HDF5:FileTemplate", "%%s%%s%%d.hdf5")
dbpf("%(P)s:HDF5:FileWriteMode", "Stream")
dbpf("%(P)s:HDF5:EnableCallbacks", "Enable")

#Set up the PROC plugin spectra summing
dbpf("%(P)s:PROC:EnableFilter", "Enable")
dbpf("%(P)s:PROC:FilterType", "Sum")
dbpf("%(P)s:PROC:EnableCallbacks", "Enable")

# Enable spectra for channel 1
dbpf("%(P)s:C1_PluginControlVal", "Enabled")

#Set up the ROI parameters
dbpf("%(P)s:ROIDATA:EnableX", "Yes")
dbpf("%(P)s:ROIDATA:EnableY", "Yes")

#Set the default energy and channel range for the ROIDATA plugin.
dbpf("%(P)s:ROIDATA:SizeX", "4096")
dbpf("%(P)s:ROIDATA:SizeY", "%(CHANNELS)s")

# Xspress 3 configuration
dbpf("%(P)s:CONFIG_PATH", "%(SETTINGS)s")
dbpf("%(P)s:RUN_FLAGS","0")
dbpf("%(P)s:NUM_CHANNELS","%(CHANNELS)d")
dbpf("%(P)s:NumImages", "100")

# hdf5 output
dbpf("%(P)s:HDF5:FilePath","%(DATAPATH)s")
dbpf("%(P)s:HDF5:FileName","test")
dbpf("%(P)s:ArrayCallbacks","Enable")
dbpf("%(P)s:ROIDATA:EnableCallbacks","Enable")

# connect to Xspress3
dbpf("%(P)s:CONNECT","1")

# set TTL trigger
dbpf("%(P)s:TriggerMode","TTL Veto Only")
"""% temp_dict

        for CHAN in range(1,self.CHANNELS+1):
             temp_dict["CHAN"]=CHAN
             temp_dict["addr"]=CHAN-1
             print """
dbpf("%(P)s:ROI%(CHAN)d:EnableX", "Yes")
dbpf("%(P)s:ROI%(CHAN)d:EnableY", "Yes")
dbpf("%(P)s:ROI%(CHAN)d:MinY", "%(addr)d")
dbpf("%(P)s:ROI%(CHAN)d:SizeY", "1")
dbpf("%(P)s:ROISUM%(CHAN)d:EnableX", "Yes")
dbpf("%(P)s:ROISUM%(CHAN)d:EnableY", "Yes")
dbpf("%(P)s:ROISUM%(CHAN)d:MinY", "%(addr)d")
dbpf("%(P)s:ROISUM%(CHAN)d:SizeY", "1")
dbpf("%(P)s:C%(CHAN)d_PluginControlValExtraROI", "0")
"""% temp_dict

#Disable channels >2 by default. These can be enabled by the
#user if needed.
             if CHAN > 1:
                  print """dbpf("%(P)s:C%(CHAN)d_PluginControlVal", "0")"""% temp_dict

    ArgInfo = (_xspress3.ArgInfo +
               _ADBase.ArgInfo + 
               makeArgInfo(__init__,
                           BUFFERS = Simple('Maximum number of NDArray buffers to be created for '
                                            'plugin callbacks', int),
                           MEMORY = Simple('Max memory to allocate, should be maxw*maxh*nbuffer '
                                           'for driver and all attached plugins', int),
                           MAX_DRIVER_FRAMES= Simple('Maximum number of frames supported by the driver', int),
                           CHANNELS= Simple('Input array queue size', int),
                           CARDS   = Simple('The number of Xspress3 systems (normally 1)', int),
                           BASE_IP = Simple('Base IP address', str),
                           DEBUG   = Simple('Passed through to the Xspress3 via xsp3_config in the Xspress API', int),
                           SIM     = Simple('Set to 1 to run driver in simulation mode', int),
                           SETTINGS= Simple('File systems path to settings directory', str),
                           DATAPATH= Simple('File systems path to default data directory', str))
               )

