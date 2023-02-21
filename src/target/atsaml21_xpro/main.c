//-----------------------------------------------------------------------------
/*

SAML21 Xplained Pro Board (SAML21J18A)

*/
//-----------------------------------------------------------------------------

#include <sam.h>
#include <soc.h>

#define DEBUG
#include "logging.h"

//-----------------------------------------------------------------------------
// gpio configuration

#define IO_LED0 GPIO_NUM(PORTB, 10)
#define IO_SW0  GPIO_NUM(PORTA, 2)

static const struct gpio_info gpios[] = {
	{IO_LED0, GPIO_OUT, 0, 0, 0},
	{IO_SW0, GPIO_IN, 1, 0, GPIO_PULLEN | GPIO_INEN},	// pull-up
};

#define NUM_GPIOS (sizeof(gpios) / sizeof(struct gpio_info))

//-----------------------------------------------------------------------------
// 1 millisecond global tick counter

#define SYSTICK_PRIO 15U

// 64 bits to avoid wrap-around (32 bits = 50 days at 1ms/tick)
static volatile uint64_t ticks;

void SysTick_Handler(void) {
	// blink the green led every 512 ms
	if ((ticks & 511) == 0) {
		gpio_toggle(IO_LED0);
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

uint64_t get_ticks(void) {
	// save/disable systick interrupt
	uint32_t save = SysTick->CTRL & SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
	// atomic read of uint64_t
	uint64_t n = ticks;
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
	uint64_t tickend = get_ticks() + (uint64_t) delay;
	while (get_ticks() < tickend) ;
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

	int i = 0;
	while (1) {
		DBG("loop %d\r\n", i);
		msDelay(1000);
		i++;
	}

 exit:
	// loop forever
	while (1) ;
	return 0;
}

//-----------------------------------------------------------------------------
