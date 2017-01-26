
xml_ = '''
  <Attribute addr="{0}" name="CHAN{1}SCA0"       source="XSP3_CHAN_SCA0"           type="PARAM"    datatype="DOUBLE" description="CHAN{1} ClockTicks" />
  <Attribute addr="{0}" name="CHAN{1}SCA1"       source="XSP3_CHAN_SCA1"           type="PARAM"    datatype="DOUBLE" description="CHAN{1} ResetTicks" />
  <Attribute addr="{0}" name="CHAN{1}SCA2"       source="XSP3_CHAN_SCA2"           type="PARAM"    datatype="DOUBLE" description="CHAN{1} ResetCounts" />
  <Attribute addr="{0}" name="CHAN{1}SCA3"       source="XSP3_CHAN_SCA3"           type="PARAM"    datatype="DOUBLE" description="CHAN{1} AllEvent" />
  <Attribute addr="{0}" name="CHAN{1}SCA4"       source="XSP3_CHAN_SCA4"           type="PARAM"    datatype="DOUBLE" description="CHAN{1} AllGood" />
  <Attribute addr="{0}" name="CHAN{1}SCA5"       source="XSP3_CHAN_SCA5"           type="PARAM"    datatype="DOUBLE" description="CHAN{1} Window1" />
  <Attribute addr="{0}" name="CHAN{1}SCA6"       source="XSP3_CHAN_SCA6"           type="PARAM"    datatype="DOUBLE" description="CHAN{1} Window2" />
  <Attribute addr="{0}" name="CHAN{1}SCA7"       source="XSP3_CHAN_SCA7"           type="PARAM"    datatype="DOUBLE" description="CHAN{1} Pileup" />
  <Attribute addr="{0}" name="CHAN{1}DTCFLAGS"   source="XSP3_CHAN_DTC_FLAGS"      type="INT"      datatype="DOUBLE" description="CHAN{1} DTC Flags" />
  <Attribute addr="{0}" name="CHAN{1}DTCAEG"     source="XSP3_CHAN_DTC_AEG"        type="PARAM"    datatype="DOUBLE" description="CHAN{1} DTC All Good Event Grad" />
  <Attribute addr="{0}" name="CHAN{1}DTCAEO"     source="XSP3_CHAN_DTC_AEO"        type="PARAM"    datatype="DOUBLE" description="CHAN{1} DTC All Good Event Offset" />
  <Attribute addr="{0}" name="CHAN{1}DTCIWG"     source="XSP3_CHAN_DTC_IWG"        type="PARAM"    datatype="DOUBLE" description="CHAN{1} DTC In Window Grad" />
  <Attribute addr="{0}" name="CHAN{1}DTCIWO"     source="XSP3_CHAN_DTC_IWO"        type="PARAM"    datatype="DOUBLE" description="CHAN{1} DTC In Window Offset" />
  <Attribute addr="{0}" name="CHAN{1}EventWidth" source="XSP3_EVENT_WIDTH"         type="PARAM"    datatype="DOUBLE" description="CHAN{1} Event Width" />
  <Attribute addr="{0}" name="CHAN{1}DTFactor"   source="XSP3_CHAN_DTFACTOR"       type="PARAM"    datatype="DOUBLE" description="CHAN{1} DT Factor" />
'''

st_ = '''#########################################
#Channel {1}
epicsEnvSet("CHAN",   "{1}")
epicsEnvSet("XADDR",  "{0}")
<SCAROI.cmd
'''

for  i in range(14):
   print st_.format(i, i+1)

