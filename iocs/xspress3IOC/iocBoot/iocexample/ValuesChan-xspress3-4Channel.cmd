# Load Default values for a single channel

dbpf("$(PREFIX)C$(CHAN)SCA:EnableCallbacks", "Enable")
dbpf("$(PREFIX)C$(CHAN)SCA1:AttrName", "CHAN$(CHAN)SCA0")
dbpf("$(PREFIX)C$(CHAN)SCA2:AttrName", "CHAN$(CHAN)SCA1")
dbpf("$(PREFIX)C$(CHAN)SCA3:AttrName", "CHAN$(CHAN)SCA2")
dbpf("$(PREFIX)C$(CHAN)SCA4:AttrName", "CHAN$(CHAN)SCA3")
dbpf("$(PREFIX)C$(CHAN)SCA5:AttrName", "CHAN$(CHAN)SCA4")
dbpf("$(PREFIX)C$(CHAN)SCA6:AttrName", "CHAN$(CHAN)SCA5")
dbpf("$(PREFIX)C$(CHAN)SCA7:AttrName", "CHAN$(CHAN)SCA6")
dbpf("$(PREFIX)C$(CHAN)SCA8:AttrName", "CHAN$(CHAN)SCA7")

dbpf("$(PREFIX)ROI$(CHAN):EnableX", "Enable")
dbpf("$(PREFIX)ROI$(CHAN):EnableY", "Enable")
dbpf("$(PREFIX)ROI$(CHAN):EnableZ", "Disable")
dbpf("$(PREFIX)ROI$(CHAN):SizeY", "1")
dbpf("$(PREFIX)ROISUM$(CHAN):EnableX", "Enable")
dbpf("$(PREFIX)ROISUM$(CHAN):EnableY", "Enable")
dbpf("$(PREFIX)ROISUM$(CHAN):SizeY", "1")

dbpf("$(PREFIX)ROI$(CHAN):EnableCallbacks", "Enable")
dbpf("$(PREFIX)MCA$(CHAN):EnableCallbacks", "Enable")
dbpf("$(PREFIX)ROISUM$(CHAN):EnableCallbacks", "Enable")
dbpf("$(PREFIX)MCASUM$(CHAN):EnableCallbacks", "Enable")
dbpf("$(PREFIX)MCA$(CHAN)ROI:EnableCallbacks", "Enable")

dbpf("$(PREFIX)MCA$(CHAN):ArrayData.LOPR", 0.5)
