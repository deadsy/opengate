//-----------------------------------------------------------------------------
/*

SAML21 Xplained Pro Board (SAML21J18A)

*/
//-----------------------------------------------------------------------------

#include <sam.h>
#include <soc.h>

//-----------------------------------------------------------------------------
// gpio configuration

#define IO_LED0 GPIO_NUM(PORTB, 10)
#define IO_SW0  GPIO_NUM(PORTA, 2)

static const struct gpio_info gpios[] = {
	{IO_LED0, GPIO_OUT, 0, 0, 0},
	{IO_SW0, GPIO_IN, 1, 0, GPIO_PULLEN | GPIO_INEN}, // pull-up
};

#define NUM_GPIOS (sizeof(gpios) / sizeof(struct gpio_info))

//-----------------------------------------------------------------------------
// 1 millisecond global tick counter

#define SYSTICK_PRIO 15U

static volatile uint64_t ticks;

void SysTick_Handler(void) {
	// blink the green led every 512 ms
	if ((ticks & 511) == 0) {
		gpio_toggle(IO_LED0);
	}
	ticks ++;
}

static void initTicks(uint32_t prio) {
  // Configure the SysTick to have interrupt in 1ms time basis
  SysTick_Config(SystemCoreClock/1000U);
  // Configure the SysTick IRQ priority
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), prio, 0));
}

void msDelay(uint32_t delay) {
  // Add a period to guarantee a minimum wait
  if (delay < UINT32_MAX)  {
     delay ++;
  }
  uint64_t tickend = ticks + (uint64_t)delay;
  while(ticks < tickend);
}

//-----------------------------------------------------------------------------

int main(void) {
  int rc;

  initTicks(SYSTICK_PRIO);

	rc = gpio_init(gpios, NUM_GPIOS);
	if (rc != 0) {
		//DBG("gpio_init failed %d\r\n", rc);
		goto exit;
	}

exit:
  // loop forever
  while(1);
  return 0;
}

//-----------------------------------------------------------------------------

