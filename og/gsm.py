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
    buf = bytearray()
    for c in s:
        idx = defSet.find(c)
        if idx != -1:
            buf.append(idx)
            continue
        idx = extSet.find(c)
        if idx != -1:
            buf.append(0x1B)
            buf.append(idx)
    return buf


def pack7(buf, src):  # pack a 7-bit string
    bits = 0
    data = 0
    for c in src:
        data |= c << bits
        bits += 7
        while bits >= 8:
            buf.append(data & 0xFF)
            data >>= 8
            bits -= 8
    if bits > 0:
        buf.append(data & 0xFF)


def encodeMessage(buf, src):  # encode a string to a 7-bit sms message
    buf.append(len(src))
    pack7(buf, defString(src))


def encodeNumber(buf, src):  # encode a phone number in octet form
    src = src.strip()
    assert len(src) != 0, "bad number"
    if src[0] == "+":  # international number
        buf.append(len(src) - 1)
        buf.append(0x91)
        src = src[1:]
    else:
        buf.append(len(src))
        buf.append(0x81)
    if len(src) & 1 != 0:
        src += "f"
    for i in range(0, len(src), 2):
        x = "%c%c" % (src[i + 1], src[i])
        buf.append(int(x, 16))


def smsMessage(dst, msg):  # build an sms message
    buf = bytearray()
    buf.append(0x11)
    buf.append(0)
    encodeNumber(buf, dst)
    buf.append(0)
    buf.append(0)
    buf.append(0xAA)
    encodeMessage(buf, msg)
    return buf
