#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Fixtures
#include <boost/test/unit_test.hpp>

#include <stdio.h>
#include "xspress3.h"
#include "xspress3EpicsTest.h"

#define MAX_SPECTRA 4096
#define NUM_CHANNELS 10

// asynPorts seem to hang around even after the xspress destructor so use a new port name for each test suite.
char asynPortHack = 1;

Xspress3Det::Xspress3Det()
{
    xsp = new Xspress3Test(&asynPortHack, NUM_CHANNELS);
    //xsp = new Xspress3Test(&asynPortHack, NUM_CHANNELS, 1, "192.168.0.1", 1000, 1000, 4096, -1, -1, 1, 0);
    BOOST_CHECK(xsp->connect() == XSP3_OK);
}

bool Xspress3Det::createMCAArray(NDDataType_t dataType)
{
    size_t dims[2] = {MAX_SPECTRA, NUM_CHANNELS};
    NDArray *pMCA;
    return this->xsp->createMCAArray(dims, pMCA, dataType);
}

bool Xspress3Det::readFrameUInt()
{
    u_int32_t SCA[XSP3_SW_NUM_SCALERS * NUM_CHANNELS], MCAData[MAX_SPECTRA * NUM_CHANNELS];
    return this->xsp->readFrame(&SCA[0], &MCAData[0], 0, MAX_SPECTRA);
}

bool Xspress3Det::readFrameDouble()
{
    double *SCA, *MCAData;
    bool result;
    SCA = new double[XSP3_SW_NUM_SCALERS * NUM_CHANNELS];
    MCAData = new double[MAX_SPECTRA * NUM_CHANNELS];
    result = this->xsp->readFrame(SCA, MCAData, 0, MAX_SPECTRA);
    delete[] SCA;
    delete[] MCAData;
    return result;
}

void Xspress3Det::pushSingleEvent()
{
    this->xsp->pushEvent(this->xsp->startEvent);
}

void Xspress3Det::fillEventQueue()
{
    const int aLotOfEvents = 18;
    // Take the lock so the events won't be serviced by another thread
    this->xsp->lock();
    for (int nEvent=0; nEvent<aLotOfEvents; nEvent++) {
        printf("Event %d\n", nEvent);
        this->xsp->pushEvent(this->xsp->startEvent);
    }
    this->xsp->unlock();
}

void Xspress3Det::writeOutScasUInt()
{
    u_int32_t *pSCA;
    pSCA = new u_int32_t[XSP3_SW_NUM_SCALERS * NUM_CHANNELS];
    this->xsp->writeOutScas(pSCA, NUM_CHANNELS);
    delete[] pSCA;
}

void Xspress3Det::writeOutScasDouble()
{
    double *pSCA;
    pSCA = new double[XSP3_SW_NUM_SCALERS * NUM_CHANNELS];
    this->xsp->writeOutScas(pSCA, NUM_CHANNELS);
    delete[] pSCA;
}

void Xspress3Det::grabFrameDouble()
{
    this->xsp->dtcEnabled = true;
    this->xsp->maxSpectra = MAX_SPECTRA;
    this->xsp->grabFrame(0, 0);
}

void Xspress3Det::doALapDouble()
{
    this->xsp->dtcEnabled = true;
    this->xsp->maxSpectra = MAX_SPECTRA;
    BOOST_CHECK(this->xsp->xsp3->histogram_start(
                    this->xsp->xsp3_handle_, -1) == XSP3_OK);
    this->xsp->doALap(16, 4, 0);
}

struct xspress3Fixture
{
    Xspress3Det xspDet;
    Xspress3Det *pXspDet;
 
    xspress3Fixture()
    {
        pXspDet = &xspDet;
        asynPortHack++;
    }
 
    ~xspress3Fixture()
    {
        if (pXspDet == NULL)
            BOOST_TEST_MESSAGE("xsp not null");
    }
};

BOOST_FIXTURE_TEST_SUITE(support, xspress3Fixture)

BOOST_AUTO_TEST_CASE(createMCAArrayUInt32)
{
    BOOST_CHECK(xspDet.createMCAArray(NDUInt32) == true);
}

BOOST_AUTO_TEST_CASE(createMCAArrayFloat64)
{
    BOOST_CHECK(xspDet.createMCAArray(NDFloat64) == true);
}

BOOST_AUTO_TEST_CASE(readFrameDouble)
{
    BOOST_CHECK(xspDet.readFrameDouble() == false);
}

BOOST_AUTO_TEST_CASE(readFrameUInt)
{
    BOOST_CHECK(xspDet.readFrameUInt() == false);
}

BOOST_AUTO_TEST_CASE(pushEvent)
{
    BOOST_CHECK_NO_THROW(xspDet.pushSingleEvent());
}

BOOST_AUTO_TEST_CASE(fillEventQueue)
{
    try {
        xspDet.fillEventQueue();
        BOOST_CHECK(false);
    } catch(...) {
        BOOST_CHECK(true);
    }
}

BOOST_AUTO_TEST_CASE(writeOutScasUInt)
{
    BOOST_CHECK_NO_THROW(xspDet.writeOutScasUInt());
}

BOOST_AUTO_TEST_CASE(writeOutScasDouble)
{
    BOOST_CHECK_NO_THROW(xspDet.writeOutScasDouble());
}

BOOST_AUTO_TEST_CASE(grabFrameDouble)
{
    BOOST_CHECK_NO_THROW(xspDet.grabFrameDouble());
}

BOOST_AUTO_TEST_CASE(doALapDouble)
{
    BOOST_CHECK_NO_THROW(xspDet.doALapDouble());
}

BOOST_AUTO_TEST_SUITE_END()
    
BOOST_AUTO_TEST_CASE(iocshConfigXspress3)
{
    BOOST_CHECK(xspress3Config("xspc", 10, 2, "127.0.0.1", 16384, 4096,
                               16384, -1, 0, 1) == asynSuccess);
}
