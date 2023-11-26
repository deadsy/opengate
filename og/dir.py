# user directory

import cfg

import utime as time
import uasyncio as asyncio


async def task(event_in, event_out):
    while True:
        try:
            key = await asyncio.wait_for_ms(event_in.get(), 100)
        except TimeoutError:
            pass
