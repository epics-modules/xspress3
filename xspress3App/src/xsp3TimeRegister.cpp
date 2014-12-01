/*
 * xsp3TimeRegister.cpp
 *
 *  Created on: 6 Nov 2014
 *      Author: npr78
 */
#include "xsp3TimeRegister.h"
#include "xspress3.h"

xsp3TimeRegister::xsp3TimeRegister(uint32_t timeRegister) :
    trigger(Software),
    invert_f0(0),
    invert_veto(0),
    debounce(0)
{
    this->set( timeRegister );
}

xsp3TimeRegister::~xsp3TimeRegister( void )
{
}

void xsp3TimeRegister::set( uint32_t timeRegister )
{
    trigger = (xsp3Trigger) XSP3_GLOB_TIMA_TF_SRC(timeRegister);
    invert_f0 = ( timeRegister & XSP3_GLOB_TIMA_F0_INV );
    invert_veto = ( timeRegister & XSP3_GLOB_TIMA_VETO_INV );
    debounce = (timeRegister & 0xFF00) >> 16;
}

uint32_t xsp3TimeRegister::get( void )
{
    uint32_t timeRegister = (uint32_t) trigger;

    if (invert_f0)   timeRegister |= XSP3_GLOB_TIMA_F0_INV;
    if (invert_veto) timeRegister |= XSP3_GLOB_TIMA_VETO_INV;
    timeRegister |= XSP3_GLOB_TIMA_DEBOUNCE(debounce);

    return timeRegister;
}

