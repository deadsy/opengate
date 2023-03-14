# relay driver

from machine import Pin

class relay:
    def __init__(self, name, pin, state=False):
        self.state = state
        self.name = name
        self.ctrl_io = Pin(pin, Pin.OUT)
        self.set(self.state)

    def set(self, state):
        self.state = state
        self.ctrl_io.value(state)

    def on(self):
        self.set(True)

    def off(self):
        self.set(False)

    def __str__(self):
        return "%s %s" % (self.name, ("off", "on")[self.state])
