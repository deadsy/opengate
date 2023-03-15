#!/usr/bin/python3


class gpio:
    def __init__(self, n):
        self.n = n

    def __str__(self):
        return "GPIO_%02d" % self.n


class hdr:
    def __init__(self, pin, gpio=None, name=None):
        self.pin = pin
        self.gpio = gpio
        self.name = name

    def __str__(self):
        s = []
        s.append("(%d)" % self.pin)
        if self.gpio is not None:
            s.append(str(self.gpio))
        if self.name is not None:
            s.append(self.name)
        return " ".join(s)


pico = [gpio(i) for i in range(30)]
rpi = [gpio(i) for i in range(30)]


rpi_hdr = (
    hdr(1, None, "3v3"),
    hdr(3, rpi[2], None),
    hdr(5, rpi[3], None),
    hdr(7, rpi[4], None),
    hdr(9, None, "GND"),
    hdr(11, rpi[17], None),
    hdr(13, rpi[27], None),
    hdr(15, rpi[22], None),
    hdr(17, None, "3v3"),
    hdr(19, rpi[10], None),
    hdr(21, rpi[9], None),
    hdr(23, rpi[11], None),
    hdr(25, None, "GND"),
    hdr(27, None, "ID_SD"),
    hdr(29, rpi[5], None),
    hdr(31, rpi[6], None),
    hdr(33, rpi[13], None),
    hdr(35, rpi[19], None),
    hdr(37, rpi[26], None),
    hdr(39, None, "GND"),
    hdr(2, None, "5v"),
    hdr(4, None, "5v"),
    hdr(6, None, "GND"),
    hdr(8, rpi[14], None),
    hdr(10, rpi[15], None),
    hdr(12, rpi[18], None),
    hdr(14, None, "GND"),
    hdr(16, rpi[23], None),
    hdr(18, rpi[24], None),
    hdr(20, None, "GND"),
    hdr(22, rpi[25], None),
    hdr(24, rpi[8], None),
    hdr(26, rpi[7], None),
    hdr(28, None, "ID_SC"),
    hdr(30, None, "GND"),
    hdr(32, rpi[12], None),
    hdr(34, None, "GND"),
    hdr(36, rpi[16], None),
    hdr(38, rpi[20], None),
    hdr(40, rpi[21], None),
)

pico_hdr = (
    hdr(1, pico[0], None),
    hdr(2, pico[1], None),
    hdr(3, None, "GND"),
    hdr(4, pico[2], None),
    hdr(5, pico[3], None),
    hdr(6, pico[4], None),
    hdr(7, pico[5], None),
    hdr(8, None, "GND"),
    hdr(9, pico[6], None),
    hdr(10, pico[7], None),
    hdr(11, pico[8], None),
    hdr(12, pico[9], None),
    hdr(13, None, "GND"),
    hdr(14, pico[10], None),
    hdr(15, pico[11], None),
    hdr(16, pico[12], None),
    hdr(17, pico[13], None),
    hdr(18, None, "GND"),
    hdr(19, pico[14], None),
    hdr(20, pico[15], None),
    hdr(21, pico[16], None),
    hdr(22, pico[17], None),
    hdr(23, None, "GND"),
    hdr(24, pico[18], None),
    hdr(25, pico[19], None),
    hdr(26, pico[20], None),
    hdr(27, pico[21], None),
    hdr(28, None, "GND"),
    hdr(29, pico[22], None),
    hdr(30, None, "RUN"),
    hdr(31, pico[26], None),
    hdr(32, pico[27], None),
    hdr(33, None, "GND"),
    hdr(34, pico[28], None),
    hdr(35, None, "ADC Vref"),
    hdr(36, None, "3v3"),
    hdr(37, None, "3v3 Enable"),
    hdr(38, None, "GND"),
    hdr(39, None, "Vsys"),
    hdr(40, None, "Vbus"),
)


def lookup_str(xhdr, pin):
    for h in xhdr:
        if h.pin == pin:
            return str(h)
    return None


def get_name(xhdr, pin):
    for h in xhdr:
        if h.pin == pin:
            return h.name
    return None


def lookup_pins(xhdr, name):
    return tuple([h.pin for h in xhdr if h.name == name])


pico_gnd = lookup_pins(pico_hdr, "GND")
pico_3v3 = lookup_pins(pico_hdr, "3v3")
pico_5v = lookup_pins(pico_hdr, "Vbus")

rpi_gnd = lookup_pins(rpi_hdr, "GND")
rpi_3v3 = lookup_pins(rpi_hdr, "3v3")
rpi_5v = lookup_pins(rpi_hdr, "5v")

# pi hat expansion https://shop.sb-components.co.uk/products/raspberry-pi-pico-hat-expansion
rpi_to_pico = (
    ((rpi_gnd), (pico_gnd)),
    ((rpi_5v), (pico_5v)),
    ((rpi_3v3), (pico_3v3)),
    ((3,), (26,)),
    ((5,), (27,)),
    ((7,), (9,)),
    ((11,), (10,)),
    ((13,), (11,)),
    ((15,), (12,)),
    ((17,), (36,)),
    ((19,), (5,)),
    ((21,), (6,)),
    ((23,), (4,)),
    ((27,), None),
    ((29,), (14,)),
    ((31,), (15,)),
    ((33,), (16,)),
    ((35,), (17,)),
    ((37,), (19,)),
    ((8,), (1,)),
    ((10,), (2,)),
    ((12,), (34,)),
    ((16,), (32,)),
    ((18,), (31,)),
    ((22,), (29,)),
    ((24,), (7,)),
    ((26,), (25,)),
    ((28,), None),
    ((32,), (24,)),
    ((36,), (22,)),
    ((38,), (21,)),
    ((40,), (20,)),
)


def lookup_map(mapping, pin):
    for a, b in mapping:
        if a is not None and pin in a:
            return b
    return None


def rev_map(mapping):
    m = []
    for a, b in mapping:
        m.append((b, a))
    return tuple(m)


def dump(mapping, from_hdr, to_hdr, n):
    for i in range(1, 1 + n):
        to = lookup_map(mapping, i)
        s = []
        s.append("%s -> " % lookup_str(from_hdr, i))
        if to is None:
            s.append("nc")
        elif len(to) == 1:
            s.append(lookup_str(to_hdr, to[0]))
        else:
            s.append("%s %s" % (str(to), get_name(to_hdr, to[0])))
        print("".join(s))


def main():
    print("**** rpi to pico ****")
    dump(rpi_to_pico, rpi_hdr, pico_hdr, 40)
    print("**** pico to rpi ****")
    dump(rev_map(rpi_to_pico), pico_hdr, rpi_hdr, 40)


main()
