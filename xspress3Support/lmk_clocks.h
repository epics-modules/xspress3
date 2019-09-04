
/************************** Constant Definitions *****************************/
#define LMK03200_REGMAXNO 15

#define OFF 0
#define ON 1

//defines for clkout_mux field
#define BYPASSED 0
#define DIVIDED 1
#define DELAYED 2
#define DIVIDED_AND_DELAYED 3

//defines for clkout_dly field
#define CLKDEL_0ps 0
#define CLKDEL_150ps 1
#define CLKDEL_300ps 2
#define CLKDEL_450ps 3
#define CLKDEL_600ps 4
#define CLKDEL_750ps 5
#define CLKDEL_900ps 6
#define CLKDEL_1050ps 7
#define CLKDEL_1200ps 8
#define CLKDEL_1350ps 9
#define CLKDEL_1500ps 10
#define CLKDEL_1650ps 11
#define CLKDEL_1800ps 12
#define CLKDEL_1950ps 13
#define CLKDEL_2100ps 14
#define CLKDEL_2250ps 15

#define BIT1_MASK  0x1		/* bit 1 */
#define BIT2_MASK  0x3
#define BIT3_MASK  0x7
#define BIT4_MASK  0xF
#define BIT8_MASK  0xFF
#define BIT12_MASK  0xFFF
#define BIT18_MASK  0x3FFFF

/***************** Macros (Inline Functions) Definitions *********************/

//addr field, common to all regs
#define REG_OFF(regOff) (regOff & BIT4_MASK)

//reg0-7 fields. clkoutDiv is divided by 2 before written to the register
#define RESET(rst) (((rst) & BIT1_MASK)<<31)
#define DLD_MODE2(mode) (((mode) & BIT1_MASK)<<28)
#define ZERO_DELAY_MODE(mode) (((mode) & BIT1_MASK)<<27)
#define FB_MUX(sel)           (((sel)& BIT2_MASK)<<25)
#define CLKOUT_MUX(clkoutMux) (((clkoutMux) & BIT2_MASK)<<17)
#define CLKOUT_EN(clkoutEn)   (((clkoutEn) & BIT1_MASK)<<16)
#define CLKOUT_DIV(clkoutDiv) ((((clkoutDiv)>>1) & BIT8_MASK)<<8)
#define CLKOUT_DLY(clkoutDly) (((clkoutDly) & BIT4_MASK)<<4)
#define VCO_MUX(sel)          (((sel)& BIT2_MASK)<<25)

#define CLKOUT_MUX_BYPASSED		0
#define CLKOUT_MUX_DIVIDED		1
#define CLKOUT_MUX_DELAYED		2
#define CLKOUT_MUX_DIVDEL		3

#define VCO_MUX_VCO_DIVIDER	0
//reg8 default value
#define REG8_DEFAULT 0x10000900

//reg9 fields
#define REG9_DEFAULT 0xA0022A00
#define VBOOST(vbst) (vbst & BIT1_MASK)<<16

//reg11 fields
#define REG11_DEFAULT 0x00820000
#define DIV4(div4) (div4 & BIT1_MASK)<<15

//reg13 fields
#define REG13_DEFAULT 0x2800000
#define OSCIN_FREQ(oscinFreq) (((oscinFreq) & BIT8_MASK)<<14)
#define VCO_R4_LF(vcoR4Lf) (((vcoR4Lf) & BIT3_MASK)<<11)
#define VCO_R3_LF(vcoR3Lf) (((vcoR3Lf) & BIT3_MASK)<<8)
#define VCO_C3_C4_LF(vcoC3C4Lf) (((vcoC3C4Lf) & BIT4_MASK)<<4)

//reg14 fields
// lmk010X0
#define CLK_SELECT(sel0)	(((sel0)&1)<<29)
#define EN_FOUT(enFout) (((enFout) & BIT1_MASK)<<28)
#define EN_CLKOUT_GLOBAL(enClkoutGlobal) (((enClkoutGlobal) & BIT1_MASK)<<27)
#define POWERDOWN(powerDown) (((powerDown) & BIT1_MASK)<<26)
#define PLL_MUX(pllMux) (((pllMux) & BIT4_MASK)<<20)
#define PLL_R(pllR) (((pllR) & BIT12_MASK)<<8)
#define PLL_R_DEL(del) (((del) & BIT4_MASK)<<4)

//reg15 fields
#define PLL_CP_GAIN(pllCpGain) (((pllCpGain) & BIT2_MASK)<<30)
#define VCO_DIV(vcoDiv) (((vcoDiv) & BIT4_MASK)<<26)
#define PLL_N(pllN) (((pllN) & BIT18_MASK)<<8)
#define PLL_N_DEL(del) (((del) & BIT4_MASK)<<4)




