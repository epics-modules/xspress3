#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Fixtures
#include <boost/test/unit_test.hpp>

#include <stdio.h>
#include "xspress3Epics.h"
#include "xspress3.h"

#define MAX_SPECTRA 4096
#define NUM_CHANNELS 10

// asynPorts seem to hang around even after the xspress destructor so use a new port name for each test suite.
char asynPortHack = 1;

struct xspress3Det
{
    Xspress3 xsp;
    Xspress3 *pXsp;
 
    xspress3Det() : xsp(&asynPortHack, NUM_CHANNELS)
    {
        pXsp = &xsp;
        BOOST_TEST_MESSAGE("setup xspress");
        xsp.connect();
        xsp.setWindow(0, 5, 0, 4);
        asynPortHack++;
    }
 
    ~xspress3Det()
    {
        if (pXsp == NULL)
            BOOST_TEST_MESSAGE("xsp not null");
        BOOST_TEST_MESSAGE("teardown xspress");
    }
};

BOOST_FIXTURE_TEST_SUITE(support, xspress3Det)

BOOST_AUTO_TEST_CASE(createSCAArray)
{
    void *pSCA;
    BOOST_CHECK(xsp.createSCAArray(pSCA) == false);
}

BOOST_AUTO_TEST_CASE(createMCAArrayUInt32)
{
    size_t dims[2] = {MAX_SPECTRA, NUM_CHANNELS};
    NDArray *pMCA;
    BOOST_CHECK(xsp.createMCAArray(dims, pMCA, NDUInt32) == false);
}

BOOST_AUTO_TEST_CASE(createMCAArrayFloat64)
{
    size_t dims[2] = {MAX_SPECTRA, NUM_CHANNELS};
    NDArray *pMCA;
    BOOST_CHECK(xsp.createMCAArray(dims, pMCA, NDFloat64) == false);
}

BOOST_AUTO_TEST_CASE(readFrameDouble)
{
    size_t dims[2] = {MAX_SPECTRA, NUM_CHANNELS};
    double *pMCAData, *pSCA;
    pSCA = (double*)malloc(XSP3_SW_NUM_SCALERS * NUM_CHANNELS * 8);
    pMCAData = (double*)malloc(MAX_SPECTRA * NUM_CHANNELS * 8);
    BOOST_CHECK(xsp.readFrame(pSCA, pMCAData, 1, MAX_SPECTRA) == false);
    free(pSCA);
    free(pMCAData);
}

BOOST_AUTO_TEST_CASE(readFrameUInt)
{
    u_int32_t SCA[XSP3_SW_NUM_SCALERS], MCAData[MAX_SPECTRA * NUM_CHANNELS];
    BOOST_CHECK(xsp.readFrame(&SCA[0], &MCAData[0], 1, MAX_SPECTRA) == false);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(integration)
{
    Xspress3 xsp(&++asynPortHack, NUM_CHANNELS);
    xsp3Api *xsp3;
    NDArray *pMCA;
    double *pData;
    void *pSCA;
    size_t dims[2] = {MAX_SPECTRA, NUM_CHANNELS};
    xsp3 = xsp.getXsp3();
    xsp.connect();
    xsp.createMCAArray(dims, pMCA, NDFloat64);
    pData = (double*)pMCA->pData;
    xsp.createSCAArray(pSCA);
    xsp3->histogram_start(xsp.getXsp3Handle(), -1);
    xsp.readFrame(static_cast<double*>(pSCA), pData, 1, MAX_SPECTRA);
    BOOST_CHECK(pData[0] == 1);
    for (int i=0; i<MAX_SPECTRA; i++)
        BOOST_CHECK(pData[i] == (int)pData[i] % 100);
    free(pSCA);
    pMCA->release();
}

BOOST_AUTO_TEST_CASE(dataTask)
{
    xspress3Config(&++asynPortHack, NUM_CHANNELS, 1, "127.0.0.1", 16, 16, MAX_SPECTRA, -1, -1, 1, 1);
}
    
