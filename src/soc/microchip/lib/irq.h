//-----------------------------------------------------------------------------
/*

IRQ Control

*/
//-----------------------------------------------------------------------------

#ifndef IRQ_H
#define IRQ_H

#ifndef SOC_H
#warning "please include this file using the toplevel soc.h"
#endif

//-----------------------------------------------------------------------------

static inline uint32_t disableIRQ(void) {
	uint32_t mask = __get_PRIMASK();
	__disable_irq();
	return mask;
}

static inline void restoreIRQ(uint32_t mask) {
	if (mask == 0) {
		__enable_irq();
	}
}

//-----------------------------------------------------------------------------

#endif				// IRQ_H

//-----------------------------------------------------------------------------
