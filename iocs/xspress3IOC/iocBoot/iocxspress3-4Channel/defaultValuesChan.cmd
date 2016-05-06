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
