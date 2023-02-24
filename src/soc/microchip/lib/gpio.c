//-----------------------------------------------------------------------------
/*

GPIO Control for the ATSAM Devices

*/
//-----------------------------------------------------------------------------

#include "soc.h"

//-----------------------------------------------------------------------------

// Set mux number for the IO pin
static void gpio_set_mux(int n, uint8_t mux) {
	int pin = GPIO_PIN(n);
	mux &= 15;
	// read/modify/write
	uint8_t val = PORT_REGS->GROUP[GPIO_PORT(n)].PORT_PMUX[pin >> 1];
	if (pin & 1) {
		// odd, upper 4 bits
		val = (val & 0x0f) | (mux << 4);
	} else {
		// even, lower 4 bits
		val = (val & 0xf0) | mux;
	}
	PORT_REGS->GROUP[GPIO_PORT(n)].PORT_PMUX[pin >> 1] = val;
}

// Set the IO pin configuration
static void gpio_set_cfg(int n, uint8_t cfg) {
	PORT_REGS->GROUP[GPIO_PORT(n)].PORT_PINCFG[GPIO_PIN(n)] = cfg;
}

//-----------------------------------------------------------------------------

// Configure a set of IO pins
int gpio_init(const struct gpio_info *info, size_t n) {
	for (unsigned int i = 0; i < n; i++) {
		const struct gpio_info *x = &info[i];
		(x->dir) ? gpio_dirn_out(x->num) : gpio_dirn_in(x->num);
		(x->out) ? gpio_set_pin(x->num) : gpio_clr_pin(x->num);
		gpio_set_mux(x->num, x->mux);
		gpio_set_cfg(x->num, x->cfg);
	}
	return 0;
}

//-----------------------------------------------------------------------------
