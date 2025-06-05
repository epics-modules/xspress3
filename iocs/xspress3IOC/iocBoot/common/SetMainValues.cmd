#
dbpf("$(PREFIX)det1:RUN_FLAGS",    "0")
dbpf("$(PREFIX)det1:NUM_CHANNELS", "$(NUM_CHANNELS)")
dbpf("$(PREFIX)det1:NumImages",    1000)
dbpf("$(PREFIX)det1:AcquireTime",  1)
dbpf("$(PREFIX)det1:CONNECT",      "1")
dbpf("$(PREFIX)det1:CTRL_DTC",     "Disable")
dbpf("$(PREFIX)det1:TriggerMode",  "Internal")
dbpf("$(PREFIX)det1:EraseOnStart", "Yes")
dbpf("$(PREFIX)det1:FrameAdvance", 0)

#Enable Array Callbacks, set Attributes file
dbpf("$(PREFIX)det1:ArrayCallbacks",   "Enable")
dbpf("$(PREFIX)det1:NDAttributesFile", "XSP3.xml")

#Configure HDF Plugin
dbpf("$(PREFIX)HDF1:FileTemplate",     "%s%s%d.hdf5")
dbpf("$(PREFIX)HDF1:FileWriteMode",    "Stream")
dbpf("$(PREFIX)HDF1:EnableCallbacks",  "Enable")
dbpf("$(PREFIX)HDF1:Compression",      "zlib")
dbpf("$(PREFIX)HDF1:ZLevel",           "1")
dbpf("$(PREFIX)HDF1:AutoIncrement",    "Yes")

#Configure PROC plugin spectra summing
dbpf("$(PREFIX)Proc1:EnableFilter",    "Enable")
dbpf("$(PREFIX)Proc1:FilterType",      "Sum")
dbpf("$(PREFIX)Proc1:EnableCallbacks", "Enable")
