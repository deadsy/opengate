//-----------------------------------------------------------------------------
/*

OpenGate Application

*/
//-----------------------------------------------------------------------------

#include <string.h>

#include "og.h"

#define DEBUG
#include "logging.h"

//-----------------------------------------------------------------------------
// scroller

#if 0

static void scroll_puts(char *s, int col) {
	lcd_puts(&lcd, 0, col, s);
}

static struct scroll_ctrl scroll = {
	.s = "     Arbeit Macht Frei...   Arby's Makes Fries...",
	.width = LCD_COLS,
	.dirn = SCROLL_LEFT,
	.puts = scroll_puts,
};

#endif

//-----------------------------------------------------------------------------
// key events

// handle a key down event
static void key_dn_handler(struct opengate *og, struct event *e) {
	(void)og;
	DBG("key down %d\r\n", EVENT_KEY(e->type));
}

// handle a key up event
static void key_up_handler(struct opengate *og, struct event *e) {
	(void)og;
	DBG("key up %d\r\n", EVENT_KEY(e->type));
}

//-----------------------------------------------------------------------------

static void foo(void *arg) {
	(void)arg;
	DBG("foo!\r\n");
}

// main opengate loop
int og_run(struct opengate *og) {

	timer_alloc(TIMER_REPEAT, 1024, foo, NULL);

	while (1) {
		struct event e;
		if (!event_rd(&e)) {
			switch (EVENT_TYPE(e.type)) {
			case EVENT_TYPE_KEY_DN:
				key_dn_handler(og, &e);
				break;
			case EVENT_TYPE_KEY_UP:
				key_up_handler(og, &e);
				break;
			case EVENT_TYPE_TIMER:
				timer_tick();
				break;
			default:
				DBG("unknown event %08x %08x\r\n", e.type, e.ptr);
				break;
			}
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------

// initialise the opengate state
int og_init(struct opengate *og) {
	int rc = 0;

	memset(og, 0, sizeof(struct opengate));

	rc = timer_init();
	if (rc != 0) {
		DBG("timer_init failed %d\r\n", rc);
		goto exit;
	}

	rc = event_init();
	if (rc != 0) {
		DBG("event_init failed %d\r\n", rc);
		goto exit;
	}

 exit:
	return rc;
}

//-----------------------------------------------------------------------------
