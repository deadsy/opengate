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


def to_float(s):
    if s == b"":
        return None
    return float(s)


def to_date(s):
    assert len(s) == 6, "bad date"
    d = int(s[0:2])
    m = int(s[2:4])
    y = int(s[4:6])
    return (y, m, d)


def decode_gps(s):
    if b"+CGPSINFO:" not in s:
        return None  # no data
    x = s.strip().split(b":")[1].strip()
    if len(x) == x.count(b","):
        return None  # all commas
    x = x.split(b",")
    assert len(x) == 9, "bad info"
    info = {}
    # latitude
    assert len(x[0]) == 11, "bad lat"
    lat_deg = x[0][0:2]
    lat_min = x[0][2:]
    lat = float(lat_deg) + float(lat_min) * (1.0 / 60.0)
    assert x[1] in b"NS", "bad NS"
    lat *= (-1.0, 1.0)[x[1] == b"N"]
    info["lat"] = lat
    # longitude
    assert len(x[2]) == 12, "bad lon"
    lon_deg = x[2][0:3]
    lon_min = x[2][3:]
    lon = float(lon_deg) + float(lon_min) * (1.0 / 60.0)
    assert x[3] in b"EW", "bad EW"
    lon *= (-1.0, 1.0)[x[3] == b"E"]
    info["lon"] = lon
    # date/time
    info["date"] = to_date(x[4])
    info["time"] = to_float(x[5])
    # other
    info["alt"] = to_float(x[6])
    info["speed"] = to_float(x[7])
    info["course"] = to_float(x[8])
    return info


class modem:
    def __init__(self, uart, pwr):
        self.uart = uart
        self.pwr_io = Pin(pwr, Pin.OUT)
        self.info = None
        self.sim = None

    def reset(self):  # reset the modem
        self.pwr_io.value(0)
        time.sleep_ms(100)
        self.pwr_io.value(1)
        time.sleep_ms(500)
        self.pwr_io.value(0)

    def cmd(self, cmd, timeout=10):  # send an AT command
        self.uart.write("AT%s\r" % cmd)
        rsp = b""
        while timeout > 0:
            rx = self.uart.read()
            if rx is not None:
                rsp += rx
                if rsp.endswith(b"OK\r\n"):
                    break
                if rsp.endswith("\r\n") and "ERROR" in rsp:
                    break
            timeout -= 1
        return rsp

    def set_echo(self, state):  # set the command echo
        rsp = self.cmd(("E0", "E1")[state])
        check_response(rsp)

    def set_cmee(self, mode):  # set the error code return type
        rsp = self.cmd("+CMEE=%d" % mode)
        check_response(rsp)

    def get_modem_info(self):  # get modem information
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

    def get_sim_info(self):  # get sim card info
        info = {}
        info["ICCID"] = self.get_iccid()
        info["IMSI"] = self.get_imsi()
        info["SUBNUM"] = self.get_subnum()
        return info

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

    def set_fun(self, fun):  # set phone functionality
        assert fun in (0, 1, 4, 5, 6, 7), "bad fun"
        rsp = self.cmd("+CFUN=%d" % fun)
        check_response(rsp)

    def gps(self, on, mode=1):  # gps start/stop
        assert type(on) == bool, "bad on/off"
        assert mode in (1, 2, 3), "bad mode"
        rsp = self.cmd("+CGPS=%d,%d" % ((0, 1)[on], mode))
        check_response(rsp)

    def cpin(self):
        rsp = self.cmd("+CPIN?")
        check_response(rsp)
        print(rsp)

    def get_subnum(self):  # get subscriber number (alpha, num, type)
        rsp = self.cmd("+CNUM")
        check_response(rsp)
        x = rsp.strip().split(b"\r\n")[0]
        x = x.split(b":")[1].strip().split(b",")
        a = x[0].strip(b'"').decode("utf-8")
        n = x[1].strip(b'"').decode("utf-8")
        t = int(x[2])
        return (a, n, t)

    def get_network_operator(self):
        rsp = self.cmd("+COPS?")
        check_response(rsp)
        print(rsp)
        x = rsp.strip().split(b"\r\n")[0]
        x = x.split(b":")[1].strip().split(b",")
        info = {}
        info["mode"] = int(x[0])
        info["format"] = int(x[1])
        info["operator"] = x[2].strip(b'"').decode("utf-8")
        info["act"] = int(x[3])
        return info

    def get_sms_sca(self):  # get sms service center address
        rsp = self.cmd("+CSCA?")
        check_response(rsp)
        print(rsp)

    def get_sms_format(self):  # get sms message format
        rsp = self.cmd("+CMGF?")
        check_response(rsp)
        x = rsp.strip().split(b"\r\n")[0]
        x = x.split(b":")[1].strip()
        return int(x)

    def set_sms_format(self, mode):  # set sms message format
        assert mode in (0, 1), "bad mode"
        # rsp = self.cmd("AT+CMGF=%d" % mode)
        rsp = self.cmd("AT+CMGF=?")
        print(rsp)
        check_response(rsp)

    def gps_info(self):  # return gps data
        rsp = self.cmd("+CGPSINFO=1")
        check_response(rsp)
        x = rsp.strip().split(b"\r\n")[0]
        return decode_gps(x)

    def setup(self):  # initial modem setup
        self.set_echo(False)  # echo off
        self.set_cmee(2)  # string error codes
        self.set_fun(1)  # full functionality
        self.info = self.get_modem_info()
        self.sim = self.get_sim_info()

    def __str__(self):
        s = []
        for k, v in self.info.items():
            s.append("%s: %s" % (k, v))
        for k, v in self.sim.items():
            s.append("%s: %s" % (k, v))
        return "\n".join(s)
