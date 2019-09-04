/* Virtex 7 Clock header file
	Created: WIH 5/1/2017
*/
#ifndef VIRTEX7_CLOCKS_H_
#define VIRTEX7_CLOCKS_H_

#define XSP3_DMAS_ADC_CLK_BASE 	(0x7000>>2)

#define XSP3_V7_MMCM_SRR			(0)			//!< Software Reset Register, Write 0xA to force reset
#define XSP3_V7_MMCM_SR				(1)			//!< Status Register
#define XSP3_V7_MMCM_CLK_MON		(2)			//!< Clock Monitor and Error register
#define XSP3_V7_MMCM_IRQ_STAT		(3)			//!< Interupt Status
#define XSP3_V7_MMCM_IRQ_ENB		(4)			//!< Interupt Enable

#define XSP3_V7_MMCM_CCR0_DIVIDE		(0x200>>2)	//!<  Input and feedback divide ratios
#define XSP3_V7_MMCM_CCR1_FB_PHASE		(0x204>>2)	//!<  Feedback divide phase
#define XSP3_V7_MMCM_CCR2_CLK0_DIVIDE	(0x208>>2)	//!<  ClkOut0 divide 
#define XSP3_V7_MMCM_CCR3_CLK0_PHASE	(0x20C>>2)	//!<  ClkOut0 phase
#define XSP3_V7_MMCM_CCR4_CLK0_DUTY		(0x210>>2)	//!<  ClkOut0 duty

#define XSP3_V7_MMCM_CCR5_CLK1_DIVIDE	(0x214>>2)	//!<  ClkOut1 divide 
#define XSP3_V7_MMCM_CCR6_CLK1_PHASE	(0x218>>2)	//!<  ClkOut1 phase
#define XSP3_V7_MMCM_CCR7_CLK1_DUTY		(0x21C>>2)	//!<  ClkOut1 duty

#define XSP3_V7_MMCM_CCR8_CLK2_DIVIDE	(0x220>>2)	//!<  ClkOut2 divide 
#define XSP3_V7_MMCM_CCR9_CLK2_PHASE	(0x224>>2)	//!<  ClkOut2 phase
#define XSP3_V7_MMCM_CCR10_CLK2_DUTY	(0x228>>2)	//!<  ClkOut2 duty

#define XSP3_V7_MMCM_CCR11_CLK3_DIVIDE	(0x22C>>2)	//!<  ClkOut3 divide 
#define XSP3_V7_MMCM_CCR12_CLK3_PHASE	(0x230>>2)	//!<  ClkOut3 phase
#define XSP3_V7_MMCM_CCR13_CLK3_DUTY	(0x234>>2)	//!<  ClkOut3 duty

#define XSP3_V7_MMCM_CCR14_CLK4_DIVIDE	(0x238>>2)	//!<  ClkOut4 divide 
#define XSP3_V7_MMCM_CCR15_CLK4_PHASE	(0x23C>>2)	//!<  ClkOut4 phase
#define XSP3_V7_MMCM_CCR16_CLK4_DUTY	(0x240>>2)	//!<  ClkOut4 duty

#define XSP3_V7_MMCM_CCR17_CLK5_DIVIDE	(0x244>>2)	//!<  ClkOut5 divide 
#define XSP3_V7_MMCM_CCR18_CLK5_PHASE	(0x248>>2)	//!<  ClkOut5 phase
#define XSP3_V7_MMCM_CCR19_CLK5_DUTY	(0x24C>>2)	//!<  ClkOut5 duty

#define XSP3_V7_MMCM_CCR20_CLK6_DIVIDE	(0x250>>2)	//!<  ClkOut6 divide 
#define XSP3_V7_MMCM_CCR21_CLK6_PHASE	(0x254>>2)	//!<  ClkOut6 phase
#define XSP3_V7_MMCM_CCR22_CLK6_DUTY	(0x258>>2)	//!<  ClkOut6 duty

#define XSP3_V7_MMCM_CCR23_LOAD			(0x25C>>2)	//!<  Force load of selected values.

#define XSP3_V7_MMCM_DRP_BASE			(0x300>>2)	//!<  Base address of DRP registers to load directly from Clock Wizzard values.

#define XSP3_V7_MMCM_DRP_POWER			0			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Power register
#define XSP3_V7_MMCM_DRP_CLK0R1			1			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout0 Reg 1
#define XSP3_V7_MMCM_DRP_CLK0R2			2			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout0 Reg 2
#define XSP3_V7_MMCM_DRP_CLK1R1			3			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout1 Reg 1
#define XSP3_V7_MMCM_DRP_CLK1R2			4			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout1 Reg 2
#define XSP3_V7_MMCM_DRP_CLK2R1			5			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout2 Reg 1
#define XSP3_V7_MMCM_DRP_CLK2R2			6			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout2 Reg 2
#define XSP3_V7_MMCM_DRP_CLK3R1			7			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout3 Reg 1
#define XSP3_V7_MMCM_DRP_CLK3R2			8			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout3 Reg 2
#define XSP3_V7_MMCM_DRP_CLK4R1			9			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout4 Reg 1
#define XSP3_V7_MMCM_DRP_CLK4R2			10			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout4 Reg 2
#define XSP3_V7_MMCM_DRP_CLK5R1			11			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout5 Reg 1
#define XSP3_V7_MMCM_DRP_CLK5R2			12			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout5 Reg 2
#define XSP3_V7_MMCM_DRP_CLK6R1			13			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout6 Reg 1
#define XSP3_V7_MMCM_DRP_CLK6R2			14			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Clkout6 Reg 2
#define XSP3_V7_MMCM_DRP_DIVCLK			15			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Divide Clock
#define XSP3_V7_MMCM_DRP_CLKFBR1		16			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, ClkFB Reg 1
#define XSP3_V7_MMCM_DRP_CLKFBR2		17			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, ClkFB Reg 2
#define XSP3_V7_MMCM_DRP_LOCK1			18			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Lock 1
#define XSP3_V7_MMCM_DRP_LOCK2			19			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Lock 2
#define XSP3_V7_MMCM_DRP_LOCK3			20			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Lock 3
#define XSP3_V7_MMCM_DRP_FILTER1		21			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Filter 1
#define XSP3_V7_MMCM_DRP_FILTER2		22			//!<  Offset to add to XSP3_V7_MMCM_DRP_BASE, Filter 2

#define XSP3_V7_MMCM_DRP_LOAD			(0x35C>>2)	//!<  Force Load of DRP register values

#endif



