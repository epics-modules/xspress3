XSPRESS3_API += /dls_sw/work/R3.14.11/support/xspress3_api/det-software
BOOST += /dls_sw/work/common/boost/1.49.0

USR_INCLUDES_Linux += -I$(XSPRESS3_API)/none_vme/xspress3/include/
USR_INCLUDES_Linux += -I$(XSPRESS3_API)/none_vme/xspress3/xspress3FemApi/include/
USR_INCLUDES_Linux += -I$(XSPRESS3_API)/libs/include/

USR_LDFLAGS += -L$(BOOST)/stage/lib/
USR_LDFLAGS += -L$(XSPRESS3_API)/libs/libs.linux.x86_64/lib/

SYS_PROD_LIBS += -lxspress3
SYS_PROD_LIBS += -lXspress3FemApi
SYS_PROD_LIBS += -lFemClient
SYS_PROD_LIBS += -lboost_thread
SYS_PROD_LIBS += -lboost_system
SYS_PROD_LIBS += -ltutils
SYS_PROD_LIBS += -limg_mod
SYS_PROD_LIBS += -lrt

all:
	g++ -o test_check_desc $(USR_INCLUDES_Linux) $(USR_LDFLAGS) test_check_desc.c $(SYS_PROD_LIBS)

clean:
	rm test_check_desc