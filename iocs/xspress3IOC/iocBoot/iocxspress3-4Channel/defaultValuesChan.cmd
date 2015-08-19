# Load Default values for a single channel

dbpf("$(PREFIX)C$(CHAN)SCAs:EnableCallbacks", "Enable")
dbpf("$(PREFIX)C$(CHAN)SCAs:1:AttrName", "CHAN$(CHAN)SCA0")
dbpf("$(PREFIX)C$(CHAN)SCAs:2:AttrName", "CHAN$(CHAN)SCA1")
dbpf("$(PREFIX)C$(CHAN)SCAs:3:AttrName", "CHAN$(CHAN)SCA2")
dbpf("$(PREFIX)C$(CHAN)SCAs:4:AttrName", "CHAN$(CHAN)SCA3")
dbpf("$(PREFIX)C$(CHAN)SCAs:5:AttrName", "CHAN$(CHAN)SCA4")
dbpf("$(PREFIX)C$(CHAN)SCAs:6:AttrName", "CHAN$(CHAN)SCA5")
dbpf("$(PREFIX)C$(CHAN)SCAs:7:AttrName", "CHAN$(CHAN)SCA6")
dbpf("$(PREFIX)C$(CHAN)SCAs:8:AttrName", "CHAN$(CHAN)SCA7")
