//-----------------------------------------------------------------------------
/*

SAML21 Xplained Pro Board (SAML21J18A)

*/
//-----------------------------------------------------------------------------

#include "soc.h"
#include "debounce.h"
#include "keyscan.h"
#include "hd44780.h"

#define DEBUG
#include "logging.h"

//-----------------------------------------------------------------------------
// gpio configuration

#define IO_LED0 GPIO_NUM(PORTB, 10)
#define IO_SW0  GPIO_NUM(PORTA, 2)

#define IO_KEY_ROW0  GPIO_NUM(PORTA, 4)
#define IO_KEY_COL0  GPIO_NUM(PORTA, 10)

#define IO_LCD_CLK  GPIO_NUM(PORTA, 14)
#define IO_LCD_RS  GPIO_NUM(PORTA, 15)
#define IO_LCD_D0  GPIO_NUM(PORTB, 0)

// num, dir, out, mux, cfg
static const struct gpio_info gpios[] = {
	// led
	{IO_LED0, GPIO_OUT, 0, 0, 0},
	// push button
	{IO_SW0, GPIO_IN, 1, 0, GPIO_PULLEN | GPIO_INEN},	// pull-up
	// keyscan rows (output)
	{IO_KEY_ROW0 + 0, GPIO_OUT, 0, 0, 0},
	{IO_KEY_ROW0 + 1, GPIO_OUT, 0, 0, 0},
	{IO_KEY_ROW0 + 2, GPIO_OUT, 0, 0, 0},
	{IO_KEY_ROW0 + 3, GPIO_OUT, 0, 0, 0},
	// keyscan cols (input)
	{IO_KEY_COL0 + 0, GPIO_IN, 0, 0, GPIO_INEN | GPIO_PULLEN},	// pull-down
	{IO_KEY_COL0 + 1, GPIO_IN, 0, 0, GPIO_INEN | GPIO_PULLEN},	// pull-down
	{IO_KEY_COL0 + 2, GPIO_IN, 0, 0, GPIO_INEN | GPIO_PULLEN},	// pull-down
	{IO_KEY_COL0 + 3, GPIO_IN, 0, 0, GPIO_INEN | GPIO_PULLEN},	// pull-down

	// lcd control
	{IO_LCD_CLK, GPIO_OUT, 0, 0, 0},
	{IO_LCD_RS, GPIO_OUT, 0, 0, 0},
	// lcd data
	{IO_LCD_D0 + 0, GPIO_OUT, 0, 0, 0},
	{IO_LCD_D0 + 1, GPIO_OUT, 0, 0, 0},
	{IO_LCD_D0 + 2, GPIO_OUT, 0, 0, 0},
	{IO_LCD_D0 + 3, GPIO_OUT, 0, 0, 0},
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
	gpio_set(IO_KEY_ROW0 + row);
}

static void clr_row(int row) {
	gpio_clr(IO_KEY_ROW0 + row);
}

#define KEY_COL_PORT GPIO_PORT(IO_KEY_COL0)
#define KEY_COL_SHIFT GPIO_PIN(IO_KEY_COL0)
#define KEY_COL_MASK ((1 << KEY_COLS) - 1)

static uint32_t read_col(void) {
	// cols on PA 10,11, 12, 13
	return (gpio_rd_input(KEY_COL_PORT) >> KEY_COL_SHIFT) & KEY_COL_MASK;
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
// LCD

#define LCD_ROWS 1
#define LCD_COLS 20

static void lcd_clk_hi(void) {
	gpio_set(IO_LCD_CLK);
}

static void lcd_clk_lo(void) {
	gpio_clr(IO_LCD_CLK);
}

static void lcd_rs_hi(void) {
	gpio_set(IO_LCD_RS);
}

static void lcd_rs_lo(void) {
	gpio_clr(IO_LCD_RS);
}

#define LCD_DATA_PORT GPIO_PORT(IO_LCD_D0)
#define LCD_DATA_SHIFT GPIO_PIN(IO_LCD_D0)

static void lcd_wr(uint8_t val) {
	gpio_rmw(LCD_DATA_PORT, val << LCD_DATA_SHIFT, 15 << LCD_DATA_SHIFT);
}

static struct hd44780_ctrl lcd = {
	.mode = HD44780_MODE4,
	.rows = LCD_ROWS,
	.cols = LCD_COLS,
	.clk_hi = lcd_clk_hi,
	.clk_lo = lcd_clk_lo,
	.rs_hi = lcd_rs_hi,
	.rs_lo = lcd_rs_lo,
	.wr = lcd_wr,
};

//-----------------------------------------------------------------------------
// 1 millisecond global tick counter

#define SYSTICK_PRIO 15U

void SysTick_Handler(void) {
	uint32_t ticks = getTick();
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

	rc = hd44780_init(&lcd);
	if (rc != 0) {
		DBG("hd44780_init failed %d\r\n", rc);
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
