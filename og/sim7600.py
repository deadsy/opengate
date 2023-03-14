# sim7600 driver

from machine import Pin


class modem:
    def __init__(self, uart, pwr, fc):
        self.uart = uart
        self.pwr_io = Pin(pwr, Pin.OUT)
        self.fc_io = Pin(fc, Pin.OUT)

    def flight_control(self, state):
        self.fc_io.value(state)

    def power(self, state):
        self.pwr_io.value(state)

    def send_cmd(self, cmd):
        self.serial.write("AT%s\r" % cmd)

    def __str__(self):
        s = []
        s.append("%s" % self.uart)
        return "\n".join(s)
