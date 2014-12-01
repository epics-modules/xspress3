/*
 * xsp3TimeRegister.h
 *
 *  Created on: 6 Nov 2014
 *      Author: npr78
 */

#ifndef XSP3TimeRegister_H_
#define XSP3TimeRegister_H_
#include <stdint.h>

class xsp3TimeRegister {

public:
    enum xsp3Trigger { Software=0, Internal=1, IDC=2, TTL_veto=3, TTL_both=4, LVDS_veto=5, LVDS_both=6 };

    xsp3TimeRegister( uint32_t timeRegister=0 );
    ~xsp3TimeRegister();

    void set(uint32_t);
    uint32_t get( void );

    xsp3Trigger trigger;
    bool invert_f0;
    bool invert_veto;
    int debounce;
};

#endif /* XSP3TimeRegister_H_ */
