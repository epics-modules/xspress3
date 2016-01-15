#include "xspress3Epics.h"

class Xspress3Test : private Xspress3 {
    friend class Xspress3Det;
private:
    Xspress3Test(const char *portName, int numChannels);
};

class Xspress3Det
{
  public:
    Xspress3Det();
    void connect();
    Xspress3Test *xsp;
    bool createMCAArray(NDDataType_t dataType);
    bool readFrameUInt();
    bool readFrameDouble();
    void pushSingleEvent();
    void fillEventQueue();
    void writeOutScasUInt();
    void writeOutScasDouble();
    void grabFrameDouble();
    void doALapDouble();
};
