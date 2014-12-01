/*
 * xsp3SimData.h
 *
 *  Created on: 6 Nov 2014
 *      Author: npr78
 */

#ifndef XSP3SIMDATA_H_
#define XSP3SIMDATA_H_

#include <stdint.h>

typedef struct xsp3Window
{
    int low;
    int high;
} xsp3Window_t;

class xsp3SimElement
{
private:
    int detector;

public:
    xsp3SimElement( int numSpectra );
    ~xsp3SimElement( void );

    void generateRawSpectra( int frame, unsigned int start, unsigned int stop, uint32_t * buffer );
    void generateDTCSpectra( int frame, unsigned int start, unsigned int stop, double * buffer );
    uint32_t generateRawROI( int frame, int win );
    double generateDTCROI( int frame, int win );

    uint32_t threshold;
    xsp3Window_t window[2];
    unsigned int num_spectra;
    double processDeadTimeAllEventGradient;
    double processDeadTimeAllEventOffset;
    double processDeadTimeInWindowOffset;
    double processDeadTimeInWindowGradient;
};



#endif /* XSP3SIMDATA_H_ */
