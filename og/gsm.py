# gsm functions

defSet = (
    "@£$¥èéùìòÇ\nØø\rÅåΔ_ΦΓΛΩΠΨΣΘΞ\x1bÆæßÉ !\"#¤%&'()*+,-./0123456789:;<=>?"
    "¡ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÑÜ§¿abcdefghijklmnopqrstuvwxyzäöñüà"
)
extSet = (
    "````````````````````^```````````````````{}`````\\````````````[~]`"
    "|````````````````````````````````````€``````````````````````````"
)


def defString(s):  # encode a string using the sms default and extended character sets
    out = []
    for c in s:
        idx = defSet.find(c)
        if idx != -1:
            out.append(idx)
            continue
        idx = extSet.find(c)
        if idx != -1:
            out.extend((27, idx))
    return out


def pack7(s):  # pack a 7-bit string
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


def encodeMessage(s):  # encode a string to a 7-bit sms message
    out = []
    out.append(len(s))
    out.extend(pack7(defString(s)))
    return out


def encodeNumber(s):  # encode a phone number in octet form
    out = []
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


def smsMessage(dst, msg):  # build an sms message
    out = []
    out.append(0x11)
    out.append(0)
    out.extend(encodeNumber(dst))
    out.append(0)
    out.append(0)
    out.append(0xAA)
    out.extend(encodeMessage(msg))
    return out
