//-----------------------------------------------------------------------------
/*

GPIO Control for the ATSAM Devices

*/
//-----------------------------------------------------------------------------

#include "soc.h"

//-----------------------------------------------------------------------------

static void gpio_set_mux(int n, uint8_t mux) {
  port_registers_t *port = PORT_REGS;
  int pin = GPIO_PIN(n);
  uint8_t val = port->GROUP[GPIO_PORT(n)].PORT_PMUX[pin >> 1];
  mux &= 15;
  if (pin & 1) {
    // odd, upper 4 bits
    val = (val & 0x0f) | (mux << 4);
  } else {
    // even, lower 4 bits
    val = (val & 0xf0) | mux;
  }
  port->GROUP[GPIO_PORT(n)].PORT_PMUX[pin >> 1] = val;
}

static void gpio_set_cfg(int n, uint8_t cfg) {
    port_registers_t *port = PORT_REGS;
    port->GROUP[GPIO_PORT(n)].PORT_PINCFG[GPIO_PIN(n)] = cfg;
}

//-----------------------------------------------------------------------------

int gpio_init(const struct gpio_info *info, size_t n) {
	for (unsigned int i = 0; i < n; i++) {
		const struct gpio_info *x = &info[i];
    (x->dir) ? gpio_dirn_out(x->num) : gpio_dirn_in(x->num);
    (x->out) ? gpio_set(x->num) : gpio_clr(x->num);
    gpio_set_mux(x->num, x->mux);
    gpio_set_cfg(x->num, x->cfg);
	}
	return 0;
}

//-----------------------------------------------------------------------------
