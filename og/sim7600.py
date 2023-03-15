# sim7600 driver

import utime as time
from machine import Pin


class modem_error(Exception):
    def __init__(self, msg):
        self.msg = msg
        super().__init__(self.msg)


def ok_check(s):
    if s is None:
        raise modem_error("response is None")
    if not s.endswith("OK\r\n"):
        raise modem_error("no ok in reponse")


class modem:
    def __init__(self, uart, pwr, fc):
        self.uart = uart
        self.pwr_io = Pin(pwr, Pin.OUT)
        self.fc_io = Pin(fc, Pin.OUT)
        self.info = {}

    def flight_control(self, state):
        self.fc_io.value(state)

    def power(self, state):
        self.pwr_io.value(state)

    def cmd(self, cmd):
        self.uart.write("AT" + cmd + "\r")
        rsp = b""
        while True:
            rx = self.uart.read()
            if rx is not None:
                rsp += rx
                if rsp.endswith("OK\r\n"):
                    break
        return rsp

    def set_echo(self, state):
        rsp = self.cmd(("E0", "E1")[state])
        ok_check(rsp)

    def get_info(self):
        rsp = self.cmd("I")
        ok_check(rsp)
        for s in rsp.split(b"\r\n"):
            x = s.split(b":")
            if len(x) == 2:
                t = x[0].strip()
                v = x[1].strip()
                self.info[t.decode("utf-8")] = v.decode("utf-8")

    def start(self):
        self.flight_control(False)
        self.power(False)
        time.sleep_ms(100)
        self.power(True)
        time.sleep_ms(100)
        self.set_echo(False)
        self.get_info()

    def __str__(self):
        s = []
        s.append(str(self.uart))
        s.append(str(self.info))
        return "\n".join(s)
