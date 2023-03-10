//-----------------------------------------------------------------------------
/*

SAML21 Xplained Pro Board (SAML21J18A)

*/
//-----------------------------------------------------------------------------

#include "soc.h"
#include "debounce.h"
#include "keyscan.h"
#include "util.h"
#include "og.h"

#define DEBUG
#include "logging.h"

//-----------------------------------------------------------------------------
// gpio configuration

#define IO_LED0 IO_NUM(PORTB, 10)
#define IO_SW0  IO_NUM(PORTA, 2)
#define IO_USART_TX IO_NUM(PORTA, 4)
#define IO_USART_RX IO_NUM(PORTA, 5)

// num, dir, out, mux, cfg
static const struct gpio_info gpios[] = {
	// led
	{IO_LED0, GPIO_OUT, 0, 0, 0},
	// push button
	{IO_SW0, GPIO_IN, 1, 0, GPIO_PULLEN | GPIO_INEN},	// pull-up
	// usart
	{IO_USART_TX, GPIO_IN, 0, 3, GPIO_PMUXEN},	// sercom0, tx, pad[0]
	{IO_USART_RX, GPIO_IN, 0, 3, GPIO_PMUXEN},	// sercom0, rx, pad[1]

	{IO_NUM(PORTA, 16), GPIO_IN, 0, 7, GPIO_PMUXEN},	// GCLK_IO[2]
};

#define NUM_GPIOS (sizeof(gpios) / sizeof(struct gpio_info))

//-----------------------------------------------------------------------------
// switch/button debouncing (called from the system tick isr)

#define PUSH_BUTTON_BIT 0

// handle a key down
static void debounce_on_handler(uint32_t bits) {
	if (bits & (1 << PUSH_BUTTON_BIT)) {
		event_wr(EVENT_TYPE_KEY_DN | 0U, NULL);
	}
}

// handle a key up
static void debounce_off_handler(uint32_t bits) {
	if (bits & (1 << PUSH_BUTTON_BIT)) {
		event_wr(EVENT_TYPE_KEY_UP | 0U, NULL);
	}
}

// map the gpio inputs to be debounced into the 32 bit debounce state
static uint32_t debounce_input(void) {
	return gpio_rd_inv(IO_SW0) << PUSH_BUTTON_BIT;
}

static struct debounce_ctrl debounce = {
	.on = debounce_on_handler,
	.off = debounce_off_handler,
	.input = debounce_input,
};

//-----------------------------------------------------------------------------
// usart

static struct usart_cfg usart0_cfg = {
	.base = SERCOM0_BASE_ADDRESS,
	.baud = 19200,
	.txpo = 2,		// use pad[0]
	.rxpo = 1,		// use pad[1]
};

static struct usart_drv usart0;

//-----------------------------------------------------------------------------
// application state

static struct opengate og;

//-----------------------------------------------------------------------------
// 1 millisecond global tick counter

#define SYSTICK_PRIO 15U

void SysTick_Handler(void) {
	uint32_t ticks = getTick();
	// blink the board led every 512 ms
	if ((ticks & 511) == 0) {
		gpio_toggle_pin(IO_LED0);
	}
	// slow tick for timers
	if ((ticks & (TIMER_TICK - 1)) == 0) {
		event_wr(EVENT_TYPE_TIMER, NULL);
	}
	// sample debounced inputs every 16 ms
	if ((ticks & 15) == 0) {
		debounce_isr(&debounce);
	}
	incTick();
}

// initialise a periodic system tick
static void systick_init(uint32_t count) {
	// Configure the SysTick time interval.
	SysTick_Config(count);
	// Configure the SysTick IRQ priority
	uint32_t group = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(group, SYSTICK_PRIO, 0));
}

//-----------------------------------------------------------------------------

int main(void) {
	int rc;

	// switch the cpu clock to 48MHz
	clock_init();

	// 1ms system tick
	systick_init(SystemCoreClock / 1000U);

	rc = log_init();
	if (rc != 0) {
		goto exit;
	}

	rc = gpio_init(gpios, NUM_GPIOS);
	if (rc != 0) {
		DBG("gpio_init failed %d\r\n", rc);
		goto exit;
	}

	rc = debounce_init(&debounce);
	if (rc != 0) {
		DBG("debounce_init failed %d\r\n", rc);
		goto exit;
	}

	rc = usart_init(&usart0, &usart0_cfg);
	if (rc != 0) {
		DBG("usart_init failed %d\r\n", rc);
		//goto exit;
	}

	rc = og_init(&og);
	if (rc != 0) {
		DBG("og_init failed %d\r\n", rc);
		goto exit;
	}

	DBG("init good\r\n");

	rc = og_run(&og);
	if (rc != 0) {
		DBG("og_run exited %d\r\n", rc);
		goto exit;
	}

 exit:
	// loop forever
	while (1) ;
	return 0;
}

//-----------------------------------------------------------------------------
