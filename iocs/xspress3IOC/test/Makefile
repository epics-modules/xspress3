CC = gcc
BIN = test_check_desc
SOURCE = test_check_desc.c

XSPRESS3_API += /dls_sw/work/R3.14.11/support/xspress3_api/det-software
BOOST += /dls_sw/prod/tools/RHEL6-x86_64/boost/1-48-0

USR_INCLUDES += -I$(XSPRESS3_API)/none_vme/xspress3/include/
USR_INCLUDES += -I$(XSPRESS3_API)/none_vme/xspress3/xspress3FemApi/include/
USR_INCLUDES += -I$(XSPRESS3_API)/libs/include/

USR_LDFLAGS += -L$(BOOST)/prefix/lib/
USR_LDFLAGS += -L$(XSPRESS3_API)/libs/libs.linux.x86_64/lib/

LIBS += -lxspress3
LIBS += -lXspress3FemApi
LIBS += -lFemClient
LIBS += -lboost_thread
LIBS += -lboost_system
LIBS += -ltutils
LIBS += -limg_mod
LIBS += -lrt

all:
	$(CC) -o $(BIN) $(USR_INCLUDES) $(USR_LDFLAGS) $(SOURCE) $(LIBS)

clean:
	if [ -f $(BIN) ]; then rm -f $(BIN); fi