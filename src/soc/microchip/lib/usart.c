//-----------------------------------------------------------------------------
/*

USART Driver for the ATSAM Devices

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "soc.h"

//-----------------------------------------------------------------------------

#define INC_MOD(x, s) (((x) + 1) & ((s) - 1))

//-----------------------------------------------------------------------------

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

//  __IO  uint32_t  SERCOM_CTRLA;       /**< Offset: 0x00 (R/W  32) USART Control A */
//  __IO  uint32_t  SERCOM_CTRLB;       /**< Offset: 0x04 (R/W  32) USART Control B */
//  __IO  uint16_t  SERCOM_BAUD;        /**< Offset: 0x0C (R/W  16) USART Baud Rate */
//  __IO  uint8_t   SERCOM_RXPL;        /**< Offset: 0x0E (R/W  8) USART Receive Pulse Length */
//  __IO  uint8_t   SERCOM_INTENCLR;    /**< Offset: 0x14 (R/W  8) USART Interrupt Enable Clear */
//  __IO  uint8_t   SERCOM_INTENSET;    /**< Offset: 0x16 (R/W  8) USART Interrupt Enable Set */
//  __IO  uint8_t   SERCOM_INTFLAG;     /**< Offset: 0x18 (R/W  8) USART Interrupt Flag Status and Clear */
//  __IO  uint16_t  SERCOM_STATUS;      /**< Offset: 0x1A (R/W  16) USART Status */
//  __I   uint32_t  SERCOM_SYNCBUSY;    /**< Offset: 0x1C (R/   32) USART Synchronization Busy */
//  __IO  uint16_t  SERCOM_DATA;        /**< Offset: 0x28 (R/W  16) USART Data */
//  __IO  uint8_t   SERCOM_DBGCTRL;     /**< Offset: 0x30 (R/W  8) USART Debug Control */

//-----------------------------------------------------------------------------

int usart_init(struct usart_drv *usart, struct usart_cfg *cfg) {

	memset(usart, 0, sizeof(struct usart_drv));
	usart->regs = (sercom_usart_registers_t *) cfg->base;
	usart->irq = usart_irq(cfg->base);

	return 0;
}

//-----------------------------------------------------------------------------
