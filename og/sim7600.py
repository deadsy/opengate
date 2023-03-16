# sim7600 driver

import utime as time
from machine import Pin


class modem_error(Exception):
    def __init__(self, msg):
        self.msg = msg
        super().__init__(self.msg)


def check_response(s):
    if s is None or len(s) == 0:
        raise modem_error("no response")
    if "+CME ERROR" in s:
        x = s.strip().split(b":")
        msg = x[1].strip().decode("utf-8")
        raise modem_error(msg)
    if s.endswith("ERROR\r\n"):
        raise modem_error("error")


class modem:
    def __init__(self, uart, pwr_pin, fc_pin):
        self.uart = uart
        self.pwr_io = Pin(pwr_pin, Pin.OUT)
        self.fc_io = Pin(fc_pin, Pin.OUT)
        self.info = None

    def flight_control(self, state):
        self.fc_io.value(state)

    def power(self, state):
        self.pwr_io.value(state)

    def cmd(self, cmd):
        self.uart.write("AT%s\r" % cmd)
        rsp = b""
        while True:
            rx = self.uart.read()
            if rx is not None:
                rsp += rx
                if rsp.endswith(b"OK\r\n"):
                    break
                if rsp.endswith("\r\n") and "ERROR" in rsp:
                    break
        return rsp

    def set_echo(self, state):  # set the command echo
        rsp = self.cmd(("E0", "E1")[state])
        check_response(rsp)

    def get_info(self):  # get modem information
        rsp = self.cmd("I")
        check_response(rsp)
        info = {}
        for s in rsp.split(b"\r\n"):
            x = s.split(b":")
            if len(x) == 2:
                t = x[0].strip()
                v = x[1].strip()
                info[t.decode("utf-8")] = v.decode("utf-8")
        return info

    def set_cmee(self, mode):  # set the error code return type
        rsp = self.cmd("+CMEE=%d" % mode)
        check_response(rsp)

    def get_iccid(self):  # get the sim card ICCID
        rsp = self.cmd("+CICCID")
        check_response(rsp)
        x = rsp.strip().split(b"\r\n")[0]
        x = x.split(b":")[1].strip()
        return x.decode("utf-8")

    def get_imsi(self):  # get the sim card IMSI
        rsp = self.cmd("+CIMI")
        check_response(rsp)
        x = rsp.strip().split(b"\r\n")[0]
        return x.decode("utf-8")

    def get_sq(self):  # get signal quality (rssi, ber)
        rsp = self.cmd("+CSQ")
        check_response(rsp)
        x = rsp.strip().split(b"\r\n")[0]
        x = x.split(b":")[1].strip().split(b",")
        assert len(x) == 2, "bad sq decode"
        return (int(x[0]), int(x[1]))

    def setup(self):  # initial modem setup
        self.flight_control(False)
        self.power(False)
        time.sleep_ms(100)
        self.power(True)
        time.sleep_ms(100)
        self.set_echo(False)  # echo off
        self.set_cmee(2)  # string error codes
        self.info = self.get_info()
        self.info["ICCID"] = self.get_iccid()
        self.info["IMSI"] = self.get_imsi()

    def __str__(self):
        s = []
        for k, v in self.info.items():
            s.append("%s: %s" % (k, v))
        return "\n".join(s)
