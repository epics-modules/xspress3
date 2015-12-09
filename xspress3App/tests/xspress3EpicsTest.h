#include "../src/xspress3Epics.h"

class Xspress3Det
{
  public:
    Xspress3Det();
    void connect();
    Xspress3 *xsp;
    bool createMCAArray(NDDataType_t dataType);
    bool readFrameUInt();
    bool readFrameDouble();
};
