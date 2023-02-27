//-----------------------------------------------------------------------------
/*

GPIO Control for the ATSAM Devices

*/
//-----------------------------------------------------------------------------

#ifndef GPIO_H
#define GPIO_H

#ifndef SOC_H
#warning "please include this file using the toplevel soc.h"
#endif

//-----------------------------------------------------------------------------

#include <stddef.h>

//-----------------------------------------------------------------------------

#define PORTA 0
#define PORTB 1

#define IO_NUM(port, pin) ((((port) & 1) << 5) | (((pin) & 31) << 0))

#define IO_PIN(n) ((n) & 31)
#define IO_PORT(n) (((n) >> 5) & 1)

//-----------------------------------------------------------------------------

#define GPIO_IN 0		// input
#define GPIO_OUT 1		// ouput

// pin configuration
#define GPIO_DRVSTR (1 << 6)	// Output Driver Strength Selection
#define GPIO_PULLEN (1 << 2)	// Pull Enable
#define GPIO_INEN (1 << 1)	// Input Enable
#define GPIO_PMUXEN (1 << 0)	// Peripheral Multiplexer Enable

//-----------------------------------------------------------------------------

struct gpio_info {
	int num;		// gpio number
	uint8_t dir;		// direction
	uint8_t out;		// output
	uint8_t mux;		// mux number
	uint8_t cfg;		// configuration
};

//-----------------------------------------------------------------------------
// generic api functions

static inline void gpio_clr(int port, uint32_t bits) {
	PORT_REGS->GROUP[port].PORT_OUTCLR = bits;
}

static inline void gpio_set(int port, uint32_t bits) {
	PORT_REGS->GROUP[port].PORT_OUTSET = bits;
}

static inline void gpio_toggle(int port, uint32_t bits) {
	PORT_REGS->GROUP[port].PORT_OUTTGL = bits;
}

static inline void gpio_clr_pin(int n) {
	gpio_clr(IO_PORT(n), 1 << IO_PIN(n));
}

static inline void gpio_set_pin(int n) {
	gpio_set(IO_PORT(n), 1 << IO_PIN(n));
}

static inline void gpio_toggle_pin(int n) {
	gpio_toggle(IO_PORT(n), 1 << IO_PIN(n));
}

static inline void gpio_rmw(int port, uint32_t set, uint32_t clr) {
	uint32_t save = disableIRQ();
	uint32_t val = PORT_REGS->GROUP[port].PORT_OUT;
	val &= ~clr;
	val |= set;
	PORT_REGS->GROUP[port].PORT_OUT = val;
	restoreIRQ(save);
}

static inline int gpio_rd(int n) {
	return (PORT_REGS->GROUP[IO_PORT(n)].PORT_IN >> IO_PIN(n)) & 1;
}

static inline int gpio_rd_inv(int n) {
	return (~(PORT_REGS->GROUP[IO_PORT(n)].PORT_IN) >> IO_PIN(n)) & 1;
}

static inline uint32_t gpio_rd_input(int port) {
	return PORT_REGS->GROUP[port].PORT_IN;
}

static inline void gpio_dirn_in(int n) {
	PORT_REGS->GROUP[IO_PORT(n)].PORT_DIRCLR = 1 << IO_PIN(n);
}

static inline void gpio_dirn_out(int n) {
	PORT_REGS->GROUP[IO_PORT(n)].PORT_DIRSET = 1 << IO_PIN(n);
}

int gpio_init(const struct gpio_info *info, size_t n);

//-----------------------------------------------------------------------------

#endif				// GPIO_H

//-----------------------------------------------------------------------------
