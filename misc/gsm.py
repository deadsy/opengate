#!/usr/bin/python3

basicSet = "@£$¥èéùìòÇ\nØø\rÅåΔ_ΦΓΛΩΠΨΣΘΞ\x1bÆæßÉ !\"#¤%&'()*+,-./0123456789:;<=>?¡ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÑÜ`¿abcdefghijklmnopqrstuvwxyzäöñüà"
extSet = "````````````````````^```````````````````{}`````\\````````````[~]`|````````````````````````````````````€``````````````````````````"


def defString(s):
    out = bytearray()
    for c in s:
        idx = basicSet.find(c)
        if idx != -1:
            out.append(idx)
            continue
        idx = extSet.find(c)
        if idx != -1:
            out.extend((27, idx))
    return out


def pack7(s):
    bits = 0
    data = 0
    out = bytearray()
    for c in s:
        data |= c << bits
        bits += 7
        while bits >= 8:
            out.append(data & 0xFF)
            data >>= 8
            bits -= 8
    if bits > 0:
        out.append(data & 0xFF)
    return out


def encodeNumber(s):  # encode a phone number in byte form
    out = bytearray()
    s = s.strip()
    assert len(s) != 0, "bad number"
    if s[0] == "+":  # international number
        out.append(len(s) - 1)
        out.append(0x91)
        s = s[1:]
    else:
        out.append(len(s))
        out.append(0x81)
    if len(s) & 1 != 0:
        s += "f"
    for i in range(0, len(s), 2):
        x = "%c%c" % (s[i + 1], s[i])
        out.append(int(x, 16))
    return out


def dump(s):
    print(" ".join(["%02x" % c for c in s]))


def main():
    y = encodeNumber("0708251358")
    dump(y)

    y = encodeNumber("+46708251358")
    dump(y)

    y = encodeNumber("+14087102537")
    dump(y)

    x = pack7(defString("hellohello"))
    dump(x)

    x = pack7(defString("this is a string"))
    dump(x)


main()
