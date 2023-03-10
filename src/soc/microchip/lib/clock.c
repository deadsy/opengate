//-----------------------------------------------------------------------------
/*

Clock Initialisation

*/
//-----------------------------------------------------------------------------

#include "soc.h"
#include "util.h"

//-----------------------------------------------------------------------------

static inline uint32_t get_calibration(void) {
	volatile uint32_t *reg = (volatile uint32_t *)0x00806020;
	return *reg;
}

// wait for dfll48m register access
static inline void wait4_dfll48m(void) {
	while ((OSCCTRL_REGS->OSCCTRL_STATUS & (1 << 8 /*DFLLRDY*/)) == 0) ;
}

// wait for dfll48m stable clock
static inline void wait4_dfll48m_clk(void) {
	uint32_t mask = (1 << 10 /*DFLLLCKF*/) | (1 << 11 /*DFLLLCKC*/);
	while ((OSCCTRL_REGS->OSCCTRL_STATUS & mask) != mask) ;
}

// wait for gclk sync
static inline void wait4_gclk_sync(void) {
	while (GCLK_REGS->GCLK_SYNCBUSY != 0) ;
}

void clock_init(void) {

	uint32_t mask, val;

	// 1) Set Flash wait states for 48 MHz
	mask = (15 << 1 /*RWS*/);
	val = (1 << 1 /*RWS*/);
	reg_rmw(&NVMCTRL_REGS->NVMCTRL_CTRLB, mask, val);

	// 2) Enable XOSC32K clock
	val = (0 << 12 /*WRTLOCK*/);	// Write Lock
	val |= (2 << 8 /*STARTUP*/);	// Oscillator Start-Up Time
	val |= (0 << 7 /*ONDEMAND*/);	// On Demand Control
	val |= (0 << 6 /*RUNSTDBY*/);	// Run in Standby
	val |= (0 << 4 /*EN1K */ );	// 1kHz Output Enable
	val |= (1 << 3 /*EN32K */ );	// 32kHz Output Enable
	val |= (1 << 2 /*XTALEN*/);	// Crystal Oscillator Enable
	val |= (0 << 1 /*ENABLE*/);	// Oscillator Enable
	OSC32KCTRL_REGS->OSC32KCTRL_XOSC32K = val;
	// enable the oscillator as a separate step
	OSC32KCTRL_REGS->OSC32KCTRL_XOSC32K |= (1 << 1 /*ENABLE*/);
	// wait for XOSC32K to stabilize
	while ((OSC32KCTRL_REGS->OSC32KCTRL_STATUS & (1 << 0 /*XOSC32KRDY */ )) == 0) ;

	// 3) Put XOSC32K as source of Generic Clock Generator 1
	val = (1 << 16 /*DIV*/);	// Division Factor
	val |= (0 << 13 /*RUNSTDBY*/);	// Run in Standby
	val |= (0 << 12 /*DIVSEL*/);	// Divide Selection
	val |= (0 << 11 /*OE*/);	// Output Enable
	val |= (0 << 10 /*OOV*/);	// Output Off Value
	val |= (1 << 9 /*IDC*/);	// Improve Duty Cycle
	val |= (1 << 8 /*GENEN*/);	// Generic Clock Generator Enable
	val |= (5 << 0 /*SRC*/);	// Source Select (XOSC32K)
	GCLK_REGS->GCLK_GENCTRL[1] = val;
	// wait for the write to complete
	wait4_gclk_sync();

	// 4) Set Generic Clock Generator 1 as the DFLL48M reference (PCHTRL0)
	val = (0 << 7 /*WRTLOCK*/);	// Write Lock
	val |= (1 << 6 /*CHEN*/);	// Channel Enable
	val |= (1 << 0 /*GEN*/);	// Generic Clock Generator
	GCLK_REGS->GCLK_PCHCTRL[0] = val;

	// 5) Enable DFLL48M clock
	wait4_dfll48m();
	// enable DFLL in open loop mode
	val = (0 << 11 /*WAITLOCK*/);	// Wait Lock
	val |= (0 << 10 /*BPLCKC*/);	// Bypass Coarse Lock
	val |= (0 << 9 /*QLDIS*/);	// Quick Lock Disable
	val |= (0 << 8 /*CCDIS*/);	// Chill Cycle Disable
	val |= (0 << 7 /*ONDEMAND*/);	// On Demand Control
	val |= (0 << 6 /*RUNSTDBY*/);	// Run in Standby
	val |= (0 << 5 /*USBCRM*/);	// USB Clock Recovery Mode
	val |= (0 << 4 /*LLAW*/);	// Lose Lock After Wake
	val |= (0 << 3 /*STABLE*/);	// Stable DFLL Frequency
	val |= (0 << 2 /*MODE*/);	// Operating Mode Selection
	val |= (1 << 1 /*ENABLE*/);	// DFLL Enable
	OSCCTRL_REGS->OSCCTRL_DFLLCTRL = val;
	wait4_dfll48m();
	// set up the multiplier, coarse and fine steps
	val = (31 << 26 /*CSTEP*/);	// Coarse Maximum Step, (64/2 - 1)
	val |= (511 << 16 /*FSTEP*/);	// Fine Maximum Step, (1024/2 - 1)
	val |= (1465 << 0 /*MUL*/);	// DFLL Multiply Factor, (48000000/32768)
	OSCCTRL_REGS->OSCCTRL_DFLLMUL = val;
	wait4_dfll48m();
	// setup DFLLVAL using calibration values
	uint32_t coarse = (get_calibration() >> 26) & 63;
	val |= (0 << 16 /*DIFF*/);	// Multiplication Ratio Difference
	val |= (coarse << 10 /*COARSE*/);	// Coarse Value
	val |= (0 << 0 /*FINE*/);	// Fine Value
	OSCCTRL_REGS->OSCCTRL_DFLLVAL = val;
	wait4_dfll48m();
	// switch to closed loop mode and enable waitlock
	val = (1 << 11 /*WAITLOCK*/);	// Wait Lock
	val |= (1 << 2 /*MODE*/);	// Operating Mode Selection
	OSCCTRL_REGS->OSCCTRL_DFLLCTRL |= val;
	wait4_dfll48m();
	wait4_dfll48m_clk();

#if 0

	// 6) Switch Generic Clock Generator 0 to DFLL48M. CPU will run at 48MHz.
	val = (1 << 16 /*DIV*/);	// Division Factor
	val |= (0 << 13 /*RUNSTDBY*/);	// Run in Standby
	val |= (0 << 12 /*DIVSEL*/);	// Divide Selection
	val |= (0 << 11 /*OE*/);	// Output Enable
	val |= (0 << 10 /*OOV*/);	// Output Off Value
	val |= (1 << 9 /*IDC*/);	// Improve Duty Cycle
	val |= (1 << 8 /*GENEN*/);	// Generic Clock Generator Enable
	val |= (7 << 0 /*SRC*/);	// Source Select (DFLL48M)
	GCLK_REGS->GCLK_GENCTRL[0] = val;
	// wait for the write to complete
	wait4_gclk_sync();

#endif

	// 3) Put DFLL48M as source of Generic Clock Generator 2
	val = (48 << 16 /*DIV*/);	// Division Factor
	val |= (0 << 13 /*RUNSTDBY*/);	// Run in Standby
	val |= (0 << 12 /*DIVSEL*/);	// Divide Selection
	val |= (1 << 11 /*OE*/);	// Output Enable
	val |= (0 << 10 /*OOV*/);	// Output Off Value
	val |= (1 << 9 /*IDC*/);	// Improve Duty Cycle
	val |= (1 << 8 /*GENEN*/);	// Generic Clock Generator Enable
	val |= (7 << 0 /*SRC*/);	// Source Select (DFLL48M)
	GCLK_REGS->GCLK_GENCTRL[2] = val;
	// wait for the write to complete
	wait4_gclk_sync();
}

//-----------------------------------------------------------------------------
