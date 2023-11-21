//-----------------------------------------------------------------------------
/*

Clock Initialisation

*/
//-----------------------------------------------------------------------------

#include "soc.h"
#include "util.h"

//-----------------------------------------------------------------------------

#define MHz 1000000

//-----------------------------------------------------------------------------

// power manager set performance level
static void pm_set_performance(int level) {
	// clear the ready flag
	PM_REGS->PM_INTFLAG = (1 << 0 /*PLRDY*/);
	uint8_t val = (0 << 7 /*PLDIS*/);	// Performance Level Disable
	val |= ((level & 3) << 0 /*PLSEL*/);	// Performance Level Select
	PM_REGS->PM_PLCFG = val;
	// wait for ready
	while ((PM_REGS->PM_INTFLAG & (1 << 0 /*PLRDY*/)) == 0) ;
}

//-----------------------------------------------------------------------------

// wait for gclk sync
static inline void wait4_gclk_sync(void) {
	while (GCLK_REGS->GCLK_SYNCBUSY != 0) ;
}

static void gclk_init(int idx, uint32_t src, uint32_t div) {
	uint32_t val = 0;
	val |= ((div & 0xffff) << 16 /*DIV*/);	// Division Factor
	val |= (0 << 13 /*RUNSTDBY*/);	// Run in Standby
	val |= (0 << 12 /*DIVSEL*/);	// Divide Selection
	val |= (0 << 11 /*OE*/);	// Output Enable
	val |= (0 << 10 /*OOV*/);	// Output Off Value
	val |= (1 << 9 /*IDC*/);	// Improve Duty Cycle
	val |= (1 << 8 /*GENEN*/);	// Generic Clock Generator Enable
	val |= ((src & 0x1f) << 0 /*SRC*/);	// Source Select
	GCLK_REGS->GCLK_GENCTRL[idx] = val;
	wait4_gclk_sync();
}

static void gclk_output_enable(int idx) {
	GCLK_REGS->GCLK_GENCTRL[idx] |= (1 << 11 /*OE*/);
	wait4_gclk_sync();
}

// set the gclk generator for a peripheral channel
static void gclk_pch_init(int idx, int gen) {
	uint32_t val;
	val = (0 << 7 /*WRTLOCK*/);	// Write Lock
	val |= (1 << 6 /*CHEN*/);	// Channel Enable
	val |= ((gen & 15) << 0 /*GEN*/);	// Generic Clock Generator
	GCLK_REGS->GCLK_PCHCTRL[idx] = val;
}

//-----------------------------------------------------------------------------

static inline uint32_t get_calibration(void) {
	volatile uint32_t *reg = (volatile uint32_t *)0x00806020U;
	return *reg;
}

// wait for dfll48m register access
static inline void wait4_dfll48m_rdy(void) {
	while ((OSCCTRL_REGS->OSCCTRL_STATUS & (1 << 8 /*DFLLRDY*/)) == 0) ;
}

static void dfll48m_init(void) {
	uint32_t val;
	wait4_dfll48m_rdy();
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
	wait4_dfll48m_rdy();
	// set up the multiplier, coarse and fine steps
	val = (31 << 26 /*CSTEP*/);	// Coarse Maximum Step, (64/2 - 1)
	val |= (511 << 16 /*FSTEP*/);	// Fine Maximum Step, (1024/2 - 1)
	val |= (1465 << 0 /*MUL*/);	// DFLL Multiply Factor, (48000000/32768)
	OSCCTRL_REGS->OSCCTRL_DFLLMUL = val;
	wait4_dfll48m_rdy();
	// setup DFLLVAL using calibration values
	uint32_t coarse = (get_calibration() >> 26) & 63;
	val |= (0 << 16 /*DIFF*/);	// Multiplication Ratio Difference
	val |= (coarse << 10 /*COARSE*/);	// Coarse Value
	val |= (512 << 0 /*FINE*/);	// Fine Value
	OSCCTRL_REGS->OSCCTRL_DFLLVAL = val;
	wait4_dfll48m_rdy();
	// switch to closed loop mode and enable waitlock
	val = (1 << 11 /*WAITLOCK*/);	// Wait Lock
	val |= (1 << 2 /*MODE*/);	// Operating Mode Selection
	OSCCTRL_REGS->OSCCTRL_DFLLCTRL |= val;
	wait4_dfll48m_rdy();
}

//-----------------------------------------------------------------------------

static void xosc32k_init(void) {
	uint32_t val;
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
}

//-----------------------------------------------------------------------------

void clock_init(void) {

	// We are going to 48MHz, set the performance level to PL2
	pm_set_performance(2);

	// set flash wait states for 48 MHz
	reg_rmw(&NVMCTRL_REGS->NVMCTRL_CTRLB, (15 << 1 /*RWS*/), (1 << 1 /*RWS*/));

	// enable XOSC32K clock
	xosc32k_init();

	// set XOSC32K as source of GCLK1
	gclk_init(1, GCLK_SOURCE_XOSC32K, 1);

	// set GCLK1 as the DFLL48M reference (PCHTRL0)
	gclk_pch_init(0, 1);

	// enable DFLL48M clock
	dfll48m_init();

	// switch Generic Clock Generator 0 to DFLL48M, CPU will run at 48MHz.
	gclk_init(0, GCLK_SOURCE_DFLL48M, 1);
	SystemCoreClock = 48 * MHz;

	// set DFLL48M as source of GCLK2
	gclk_init(2, GCLK_SOURCE_DFLL48M, 48);
	gclk_output_enable(2);
}

//-----------------------------------------------------------------------------
