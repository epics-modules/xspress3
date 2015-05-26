#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Fixtures
#include <boost/test/unit_test.hpp>

#include <stdio.h>
#include "xspress3Epics.h"

#define DIM0 10
#define DIM1 10
#define MAX_SPECTRA 10
#define NUM_CHANNELS 10

// asynPorts seem to hang around even after the xspress destructor so use a new port name for each test suite.
char asynPortHack = 1;

struct xspress3Det
{
    Xspress3 xsp;
 
    xspress3Det() : xsp(&asynPortHack, NUM_CHANNELS, 2, "127.0.0.1", 10, 10, 10, 10, -1, 0, 1)
    {
        BOOST_TEST_MESSAGE("setup xspress");
        xsp.connect();
        xsp.setWindow(0, 5, 0, 4);
        asynPortHack++;
    }
 
    ~xspress3Det()
    {
        BOOST_TEST_MESSAGE("teardown xspress");
    }
};

BOOST_FIXTURE_TEST_SUITE(support, xspress3Det)

BOOST_AUTO_TEST_CASE(createSCAArrayNDUInt32)
{
    void* pSCA;
    BOOST_CHECK(xsp.createSCAArray(pSCA, NDUInt32) == false);
}

BOOST_AUTO_TEST_CASE(createSCAArrayNDFloat64)
{
    void* pSCA;
    BOOST_CHECK(xsp.createSCAArray(pSCA, NDFloat64) == false);
}

BOOST_AUTO_TEST_CASE(createMCAArrayNDUInt32)
{
    size_t dims[2] = {DIM0, DIM1};
    NDArray* pMCA;
    BOOST_CHECK(xsp.createMCAArray(dims, pMCA, NDUInt32) == false);
}

BOOST_AUTO_TEST_CASE(createMCAArrayNDFloat64)
{
    size_t dims[2] = {DIM0, DIM1};
    NDArray* pMCA;
    BOOST_CHECK(xsp.createMCAArray(dims, pMCA, NDFloat64) == false);
}

BOOST_AUTO_TEST_CASE(readFrameDouble)
{
    size_t dims[2] = {DIM0, DIM1};
    double *pMCAData, *pSCA;
    pSCA = (double*)malloc(XSP3_SW_NUM_SCALERS * NUM_CHANNELS * 8);
    pMCAData = (double*)malloc(DIM0 * DIM1 * 8);
    BOOST_CHECK(xsp.readFrame(pSCA, pMCAData, 1, MAX_SPECTRA) == false);
    free(pSCA);
    free(pMCAData);
}

BOOST_AUTO_TEST_CASE(readFrameUInt)
{
    u_int32_t SCA[XSP3_SW_NUM_SCALERS], MCAData[DIM0 * DIM1];
    BOOST_CHECK(xsp.readFrame(&SCA[0], &MCAData[0], 1, MAX_SPECTRA) == false);
}

BOOST_AUTO_TEST_SUITE_END()

