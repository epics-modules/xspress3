/*
 * xsp3SimElement.cpp
 *
 *  Created on: 7 Nov 2014
 *      Author: npr78
 */
#include "xsp3SimElement.h"
#include <cmath>
#include <cstdlib>


static int num_detectors=0;

xsp3SimElement::xsp3SimElement( int nspectra )
: threshold(0),
  num_spectra(nspectra),
  processDeadTimeAllEventGradient(0),
  processDeadTimeAllEventOffset(0),
  processDeadTimeInWindowOffset(0),
  processDeadTimeInWindowGradient(0)
{
    detector = num_detectors++;
}

xsp3SimElement::~xsp3SimElement( void )
{
}

void xsp3SimElement::generateRawSpectra( int frame, unsigned int start, unsigned int n_pts, uint32_t * buffer )
{
    if ( start+n_pts > num_spectra ) n_pts = num_spectra-start;

    switch (detector%3)
    {
    case 1:
    {
        // Delta functions
        for (unsigned int i=0; i < n_pts; i++)
        {
            if ((start+i+frame)%100 ==0)
                buffer[i]=100;
            else
                buffer[i]=0;
        }
        break;
    }
    case 2:
    {
        // Saw tooth
        for (unsigned int i=0; i < n_pts; i++)
        {
            buffer[i]=(start+i+frame)%100;
        }
        break;
    }
    default:
        // Sine wave with noise
        for (unsigned int i=0; i < n_pts; i++)
        {
            buffer[i] = (sin(static_cast<double>(start+i+frame)/90)+1)*100;
        }
    }
}
void xsp3SimElement::generateDTCSpectra( int frame, unsigned int start, unsigned int n_pts, double * buffer )
{
    if ( start+n_pts > num_spectra ) n_pts = num_spectra-start;

    uint32_t ibuffer[n_pts];

    generateRawSpectra( frame, start, n_pts, ibuffer );

    for (unsigned int i=0; i < n_pts; i++)
        buffer[i] = ibuffer[i];
}

uint32_t xsp3SimElement::generateRawROI( int frame, int win )
{
    int result=0;
    unsigned int start=window[win].low;
    unsigned int n_pts =window[win].high-window[win].low+1;

    if ( start+n_pts > num_spectra ) n_pts = num_spectra-start;

    uint32_t buffer[n_pts];
    generateRawSpectra( frame, start, n_pts, buffer );

    for (unsigned int i=0; i < n_pts; i++)
        result+=buffer[i];

    return result;
}

double xsp3SimElement::generateDTCROI( int frame, int win )
{
    double result=0;
    unsigned int start=window[win].low;
    unsigned int n_pts =window[win].high-window[win].low+1;

    if ( start+n_pts > num_spectra ) n_pts = num_spectra-start;

    double buffer[n_pts];
    generateDTCSpectra( frame, start, n_pts, buffer );

    for (unsigned int i=0; i < n_pts; i++)
        result+=buffer[i];

    return result;
}



