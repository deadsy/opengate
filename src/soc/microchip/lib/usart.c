//-----------------------------------------------------------------------------
/*

USART Driver for the ATSAM Devices

 SERCOM_USART_CTRLA_SWRST(value)
 SERCOM_USART_CTRLA_ENABLE(value)
 SERCOM_USART_CTRLA_MODE(value)
 SERCOM_USART_CTRLA_RUNSTDBY(value)
 SERCOM_USART_CTRLA_IBON(value)
 SERCOM_USART_CTRLA_SAMPR(value)
 SERCOM_USART_CTRLA_TXPO(value)
 SERCOM_USART_CTRLA_RXPO(value)
 SERCOM_USART_CTRLA_SAMPA(value)
 SERCOM_USART_CTRLA_FORM(value)
 SERCOM_USART_CTRLA_CMODE(value)
 SERCOM_USART_CTRLA_CPOL(value)
 SERCOM_USART_CTRLA_DORD(value)
 SERCOM_USART_CTRLB_CHSIZE(value)
 SERCOM_USART_CTRLB_SBMODE(value)
 SERCOM_USART_CTRLB_COLDEN(value)
 SERCOM_USART_CTRLB_SFDE(value)
 SERCOM_USART_CTRLB_ENC(value)
 SERCOM_USART_CTRLB_PMODE(value)
 SERCOM_USART_CTRLB_TXEN(value)
 SERCOM_USART_CTRLB_RXEN(value)
 SERCOM_USART_BAUD_BAUD(value)
 SERCOM_USART_BAUD_FRAC_BAUD(value)
 SERCOM_USART_BAUD_FRAC_FP(value)
 SERCOM_USART_BAUD_FRACFP_BAUD(value)
 SERCOM_USART_BAUD_FRACFP_FP(value)
 SERCOM_USART_BAUD_USARTFP_BAUD(value)
 SERCOM_USART_RXPL_RXPL(value)
 SERCOM_USART_INTENCLR_DRE(value)
 SERCOM_USART_INTENCLR_TXC(value)
 SERCOM_USART_INTENCLR_RXC(value)
 SERCOM_USART_INTENCLR_RXS(value)
 SERCOM_USART_INTENCLR_CTSIC(value)
 SERCOM_USART_INTENCLR_RXBRK(value)
 SERCOM_USART_INTENCLR_ERROR(value)
 SERCOM_USART_INTENSET_DRE(value)
 SERCOM_USART_INTENSET_TXC(value)
 SERCOM_USART_INTENSET_RXC(value)
 SERCOM_USART_INTENSET_RXS(value)
 SERCOM_USART_INTENSET_CTSIC(value)
 SERCOM_USART_INTENSET_RXBRK(value)
 SERCOM_USART_INTENSET_ERROR(value)
 SERCOM_USART_INTFLAG_DRE(value)
 SERCOM_USART_INTFLAG_TXC(value)
 SERCOM_USART_INTFLAG_RXC(value)
 SERCOM_USART_INTFLAG_RXS(value)
 SERCOM_USART_INTFLAG_CTSIC(value)
 SERCOM_USART_INTFLAG_RXBRK(value)
 SERCOM_USART_INTFLAG_ERROR(value)
 SERCOM_USART_STATUS_PERR(value)
 SERCOM_USART_STATUS_FERR(value)
 SERCOM_USART_STATUS_BUFOVF(value)
 SERCOM_USART_STATUS_CTS(value)
 SERCOM_USART_STATUS_ISF(value)
 SERCOM_USART_STATUS_COLL(value)
 SERCOM_USART_SYNCBUSY_SWRST(value)
 SERCOM_USART_SYNCBUSY_ENABLE(value)
 SERCOM_USART_SYNCBUSY_CTRLB(value)
 SERCOM_USART_DATA_DATA(value)
 SERCOM_USART_DBGCTRL_DBGSTOP(value)

//  __IO  uint32_t  SERCOM_CTRLA;       < Offset: 0x00 (R/W  32) USART Control A
//  __IO  uint32_t  SERCOM_CTRLB;       < Offset: 0x04 (R/W  32) USART Control B
//  __IO  uint16_t  SERCOM_BAUD;        < Offset: 0x0C (R/W  16) USART Baud Rate
//  __IO  uint8_t   SERCOM_RXPL;        < Offset: 0x0E (R/W  8) USART Receive Pulse Length
//  __IO  uint8_t   SERCOM_INTENCLR;    < Offset: 0x14 (R/W  8) USART Interrupt Enable Clear
//  __IO  uint8_t   SERCOM_INTENSET;    < Offset: 0x16 (R/W  8) USART Interrupt Enable Set
//  __IO  uint8_t   SERCOM_INTFLAG;     < Offset: 0x18 (R/W  8) USART Interrupt Flag Status and Clear
//  __IO  uint16_t  SERCOM_STATUS;      < Offset: 0x1A (R/W  16) USART Status
//  __I   uint32_t  SERCOM_SYNCBUSY;    < Offset: 0x1C (R/   32) USART Synchronization Busy
//  __IO  uint16_t  SERCOM_DATA;        < Offset: 0x28 (R/W  16) USART Data
//  __IO  uint8_t   SERCOM_DBGCTRL;     < Offset: 0x30 (R/W  8) USART Debug Control

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "soc.h"

//-----------------------------------------------------------------------------

#define INC_MOD(x, s) (((x) + 1) & ((s) - 1))

//-----------------------------------------------------------------------------

// enable the clock to the usart module
static void usart_clock_enable(uint32_t base) {

	mclk_registers_t *mclk = (mclk_registers_t *) MCLK_BASE_ADDRESS;

	if (base == SERCOM0_BASE_ADDRESS) {
		mclk->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM0(1);
	} else if (base == SERCOM1_BASE_ADDRESS) {
		mclk->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM1(1);
	} else if (base == SERCOM2_BASE_ADDRESS) {
		mclk->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM2(1);
	} else if (base == SERCOM3_BASE_ADDRESS) {
		mclk->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM3(1);
	} else if (base == SERCOM4_BASE_ADDRESS) {
		mclk->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM4(1);
	} else if (base == SERCOM5_BASE_ADDRESS) {
		mclk->MCLK_APBDMASK |= MCLK_APBDMASK_SERCOM5(1);
	}
//SERCOM0_GCLK_ID_CORE
//SERCOM0_GCLK_ID_SLOW

}

// return the irq used for this usart module
static int usart_irq(uint32_t base) {

	if (base == SERCOM0_BASE_ADDRESS) {
		return SERCOM0_IRQn;
	} else if (base == SERCOM1_BASE_ADDRESS) {
		return SERCOM1_IRQn;
	} else if (base == SERCOM2_BASE_ADDRESS) {
		return SERCOM2_IRQn;
	} else if (base == SERCOM3_BASE_ADDRESS) {
		return SERCOM3_IRQn;
	} else if (base == SERCOM4_BASE_ADDRESS) {
		return SERCOM4_IRQn;
	} else if (base == SERCOM5_BASE_ADDRESS) {
		return SERCOM5_IRQn;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// stdio functions

void usart_putc(struct usart_drv *usart, char c) {
	int tx_wr_inc = INC_MOD(usart->tx_wr, TXBUF_SIZE);
	// wait for space
	while (tx_wr_inc == usart->tx_rd) ;
	// put the character in the tx buffer
	NVIC_DisableIRQ(usart->irq);
	usart->txbuf[usart->tx_wr] = c;
	usart->tx_wr = tx_wr_inc;
	NVIC_EnableIRQ(usart->irq);
	// enable the data register empty interrupt
	usart->regs->SERCOM_INTENSET = SERCOM_USART_INTENSET_DRE(1);
}

void usart_flush(struct usart_drv *usart) {
	while (usart->tx_wr != usart->tx_rd) ;
}

// return non-zero if we have rx data
int usart_tstc(struct usart_drv *usart) {
	return usart->rx_rd != usart->rx_wr;
}

char usart_getc(struct usart_drv *usart) {
	// wait for a character
	while (usart_tstc(usart) == 0) ;
	NVIC_DisableIRQ(usart->irq);
	char c = usart->rxbuf[usart->rx_rd];
	usart->rx_rd = INC_MOD(usart->rx_rd, RXBUF_SIZE);
	NVIC_EnableIRQ(usart->irq);
	return c;
}

//-----------------------------------------------------------------------------

// read serial data into a buffer, return the number of bytes read
size_t usart_rxbuf(struct usart_drv *usart, uint8_t * buf, size_t n) {
	size_t i = 0;
	if ((usart->rx_rd == usart->rx_wr) || (n == 0)) {
		return 0;
	}
	NVIC_DisableIRQ(usart->irq);
	while (usart->rx_rd != usart->rx_wr) {
		buf[i++] = usart->rxbuf[usart->rx_rd];
		usart->rx_rd = INC_MOD(usart->rx_rd, RXBUF_SIZE);
		if (i == n) {
			break;
		}
	}
	NVIC_EnableIRQ(usart->irq);
	return i;
}

//-----------------------------------------------------------------------------

#define RX_ERRORS (SERCOM_USART_STATUS_COLL(1) | \
  SERCOM_USART_STATUS_ISF(1) | \
  SERCOM_USART_STATUS_BUFOVF(1) | \
  SERCOM_USART_STATUS_FERR(1) | \
  SERCOM_USART_STATUS_PERR(1))

void usart_isr(struct usart_drv *usart) {

	uint8_t flags = usart->regs->SERCOM_INTFLAG;

	// rx errors
	if (flags & SERCOM_USART_INTFLAG_ERROR(1)) {
		// clear the flag
		usart->regs->SERCOM_INTFLAG = SERCOM_USART_INTFLAG_ERROR(1);
		uint16_t status = usart->regs->SERCOM_STATUS;
		if (status & RX_ERRORS) {
			// clear the status flags
			usart->regs->SERCOM_STATUS = RX_ERRORS;
			usart->rx_errors++;
		}
	}
	// receive
	if (flags & SERCOM_USART_INTFLAG_RXC(1)) {
		// read the data (and clear the flag)
		uint8_t c = usart->regs->SERCOM_DATA;
		int rx_wr_inc = INC_MOD(usart->rx_wr, RXBUF_SIZE);
		if (rx_wr_inc != usart->rx_rd) {
			usart->rxbuf[usart->rx_wr] = c;
			usart->rx_wr = rx_wr_inc;
		} else {
			// rx buffer overflow
			usart->rx_errors++;
		}
	}
	// transmit
	if (flags & SERCOM_USART_INTFLAG_DRE(1)) {
		if (usart->tx_rd != usart->tx_wr) {
			// write the data (and clear the flag)
			usart->regs->SERCOM_DATA = usart->txbuf[usart->tx_rd];
			usart->tx_rd = INC_MOD(usart->tx_rd, TXBUF_SIZE);
		} else {
			// no more tx data, disable the data register empty interrupt
			usart->regs->SERCOM_INTENCLR = SERCOM_USART_INTENCLR_DRE(1);
		}
	}

}

//-----------------------------------------------------------------------------

static int usart_reset(struct usart_drv *usart) {
	// assert the CTRLA.SWRST line
	usart->regs->SERCOM_CTRLA = SERCOM_USART_CTRLA_SWRST(1);
	// wait for reset completion
	int timeout = 5;
	while (timeout > 0) {
		int r0 = usart->regs->SERCOM_CTRLA & SERCOM_USART_CTRLA_SWRST(1);
		int r1 = usart->regs->SERCOM_SYNCBUSY & SERCOM_USART_SYNCBUSY_SWRST(1);
		if (!r0 && !r1) {
			return 0;
		}
		msDelay(1);
		timeout--;
	}
	return -1;
}

static int usart_enable(struct usart_drv *usart) {
	usart->regs->SERCOM_CTRLA |= SERCOM_USART_CTRLA_ENABLE(1);
	// wait for enable completion
	int timeout = 5;
	while (timeout > 0) {
		int en = usart->regs->SERCOM_SYNCBUSY & SERCOM_USART_SYNCBUSY_ENABLE(1);
		if (!en) {
			return 0;
		}
		msDelay(1);
		timeout--;
	}
	return -1;
}

//-----------------------------------------------------------------------------

int usart_init(struct usart_drv *usart, struct usart_cfg *cfg) {
	uint32_t val;
	int rc;

	memset(usart, 0, sizeof(struct usart_drv));
	usart->regs = (sercom_usart_registers_t *) cfg->base;
	usart->irq = usart_irq(cfg->base);

	// enable the clocks to the usart
	usart_clock_enable(cfg->base);

	// disable and reset the usart
	rc = usart_reset(usart);
	if (rc != 0) {
		return -1;
	}
	// CTRLA
	val = SERCOM_USART_CTRLA_SWRST(0);	// Software Reset
	val |= SERCOM_USART_CTRLA_ENABLE(0);	// Enable
	val |= SERCOM_USART_CTRLA_MODE(0);	// Operating Mode, 0 == external clock
	val |= SERCOM_USART_CTRLA_RUNSTDBY(0);	// Run during Standby
	val |= SERCOM_USART_CTRLA_IBON(0);	// Immediate Buffer Overflow Notification
	val |= SERCOM_USART_CTRLA_SAMPR(0);	// Sample, 0 == 16x over sampling
	val |= SERCOM_USART_CTRLA_TXPO(cfg->txpo & 3);	// Transmit Data Pinout
	val |= SERCOM_USART_CTRLA_RXPO(cfg->rxpo & 3);	// Receive Data Pinout
	val |= SERCOM_USART_CTRLA_SAMPA(0);	// Sample Adjustment, 0 == 7,8,9
	val |= SERCOM_USART_CTRLA_FORM(0);	// Frame Format, 0 == no parity
	val |= SERCOM_USART_CTRLA_CMODE(0);	// Communication Mode, 0 == asynchronous
	val |= SERCOM_USART_CTRLA_CPOL(0);	// Clock Polarity
	val |= SERCOM_USART_CTRLA_DORD(1);	// Data Order, 1 == lsb first
	usart->regs->SERCOM_CTRLA = val;

	// CTRLB
	val = SERCOM_USART_CTRLB_CHSIZE(0);	// Character Size, 0 == 8 bits
	val |= SERCOM_USART_CTRLB_SBMODE(0);	// Stop Bit Mode, 0 == 1 stop bit
	val |= SERCOM_USART_CTRLB_COLDEN(0);	// Collision Detection Enable, 0 == not enabled
	val |= SERCOM_USART_CTRLB_SFDE(0);	// Start of Frame Detection Enable, 0 == disabled
	val |= SERCOM_USART_CTRLB_ENC(0);	// Encoding Format, 0 == not encoded
	val |= SERCOM_USART_CTRLB_PMODE(0);	// Parity Mode (no parity)
	val |= SERCOM_USART_CTRLB_TXEN(1);	// Transmitter Enable, 1 == enabled
	val |= SERCOM_USART_CTRLB_RXEN(1);	// Receiver Enable, 1 == enabled
	usart->regs->SERCOM_CTRLB = val;

	// enable the usart
	rc = usart_enable(usart);
	if (rc != 0) {
		return -2;
	}

	return 0;
}

//-----------------------------------------------------------------------------
