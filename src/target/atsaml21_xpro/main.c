//-----------------------------------------------------------------------------
/*

SAML21 Xplained Pro Board (SAML21J18A)

*/
//-----------------------------------------------------------------------------

#include "soc.h"
#include "debounce.h"
#include "keyscan.h"

#define DEBUG
#include "logging.h"

//-----------------------------------------------------------------------------
// gpio configuration

#define IO_LED0 GPIO_NUM(PORTB, 10)
#define IO_SW0  GPIO_NUM(PORTA, 2)

static const struct gpio_info gpios[] = {
	// led
	{IO_LED0, GPIO_OUT, 0, 0, 0},
	// push button
	{IO_SW0, GPIO_IN, 1, 0, GPIO_PULLEN | GPIO_INEN},	// pull-up
	// keyscan rows
	{GPIO_NUM(PORTA, 4), GPIO_OUT, 0, 0, 0},
	{GPIO_NUM(PORTA, 5), GPIO_OUT, 0, 0, 0},
	{GPIO_NUM(PORTA, 6), GPIO_OUT, 0, 0, 0},
	{GPIO_NUM(PORTA, 7), GPIO_OUT, 0, 0, 0},
	// keyscan cols
	{GPIO_NUM(PORTA, 10), GPIO_IN, 0, 0, GPIO_INEN | GPIO_PULLEN},	// pull-down
	{GPIO_NUM(PORTA, 11), GPIO_IN, 0, 0, GPIO_INEN | GPIO_PULLEN},	// pull-down
	{GPIO_NUM(PORTA, 12), GPIO_IN, 0, 0, GPIO_INEN | GPIO_PULLEN},	// pull-down
	{GPIO_NUM(PORTA, 13), GPIO_IN, 0, 0, GPIO_INEN | GPIO_PULLEN},	// pull-down
};

#define NUM_GPIOS (sizeof(gpios) / sizeof(struct gpio_info))

//-----------------------------------------------------------------------------
// keyboard matrix scanning

#define KEY_ROWS 4
#define KEY_COLS 4

static void key_down(int key) {
	DBG("key down %d\r\n", key);
}

static void key_up(int key) {
	DBG("key up %d\r\n", key);
}

static void set_row(int row) {
	// rows on PA 4,5,6,7
	gpio_set(GPIO_NUM(PORTA, row + 4));
}

static void clr_row(int row) {
	// rows on PA 4,5,6,7
	gpio_clr(GPIO_NUM(PORTA, row + 4));
}

static uint32_t read_col(void) {
	// cols on PA 10,11, 12, 13
	return (gpio_rd_input(PORTA) >> 10) & 15;
}

static uint8_t key_state[KEY_ROWS * KEY_COLS];

static struct keyscan_ctrl keys = {
	.rows = KEY_ROWS,
	.cols = KEY_COLS,
	.state = key_state,
	.key_dn = key_down,
	.key_up = key_up,
	.clr_row = clr_row,
	.set_row = set_row,
	.read_col = read_col,
};

//-----------------------------------------------------------------------------
// switch/button debouncing (called from the system tick isr)

#define PUSH_BUTTON_BIT 0

// handle a key down
static void debounce_on_handler(uint32_t bits) {
	if (bits & (1 << PUSH_BUTTON_BIT)) {
		DBG("key down\r\n");
	}
}

// handle a key up
static void debounce_off_handler(uint32_t bits) {
	if (bits & (1 << PUSH_BUTTON_BIT)) {
		DBG("key up\r\n");
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
// 1 millisecond global tick counter

#define SYSTICK_PRIO 15U

static volatile uint32_t ticks;

void SysTick_Handler(void) {
	// blink the green led every 512 ms
	if ((ticks & 511) == 0) {
		gpio_toggle(IO_LED0);
	}
	// sample debounced inputs every 16 ms
	if ((ticks & 15) == 0) {
		debounce_isr(&debounce);
	}
	// scan the keyboard every 8 ms
	if ((ticks & 7) == 0) {
		keyscan_isr(&keys);
	}
	ticks++;
}

// initialise a periodic system tick
static void systick_init(uint32_t count) {
	// Configure the SysTick time interval.
	SysTick_Config(count);
	// Configure the SysTick IRQ priority
	uint32_t group = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(group, SYSTICK_PRIO, 0));
}

uint64_t get_ticks64(void) {
	static uint32_t ticks_hi, old_ticks;
	// save/disable systick interrupt
	uint32_t save = SysTick->CTRL & SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	// detect rollover
	if (ticks < old_ticks) {
		ticks_hi++;
	}
	old_ticks = ticks;
	uint64_t n = ((uint64_t) ticks_hi << 32) | (uint64_t) ticks;
	// restore systick interrupt
	SysTick->CTRL |= save;
	return n;
}

// millisecond delay
void msDelay(uint32_t delay) {
	// guarantee delay < actual_delay < delay+1
	if (delay < UINT32_MAX) {
		delay++;
	}
	uint32_t tickend = ticks + delay;
	while (ticks < tickend) ;
}

//-----------------------------------------------------------------------------

int main(void) {
	int rc;

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

	rc = keyscan_init(&keys);
	if (rc != 0) {
		DBG("keyscan_init failed %d\r\n", rc);
		goto exit;
	}

	unsigned int i = 0;
	while (1) {
		//DBG("loop %d\r\n", i);
		msDelay(1000);
		i++;
	}

 exit:
	// loop forever
	while (1) ;
	return 0;
}

//-----------------------------------------------------------------------------
