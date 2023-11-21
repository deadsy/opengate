# gsm functions

defSet = (
    "@£$¥èéùìòÇ\nØø\rÅåΔ_ΦΓΛΩΠΨΣΘΞ\x1bÆæßÉ !\"#¤%&'()*+,-./0123456789:;<=>?"
    "¡ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÑÜ§¿abcdefghijklmnopqrstuvwxyzäöñüà"
)

gsmChar = {}


def init():  # init the gsm character lookup
    global gsmChar
    # basic gsm-7 characters
    for i, c in enumerate(defSet):
        gsmChar[c] = (i,)
    # extended characters
    gsmChar["^"] = (0x1B, 0x14)
    gsmChar["{"] = (0x1B, 0x28)
    gsmChar["}"] = (0x1B, 0x29)
    gsmChar["\\"] = (0x1B, 0x2F)
    gsmChar["["] = (0x1B, 0x3C)
    gsmChar["~"] = (0x1B, 0x3D)
    gsmChar["]"] = (0x1B, 0x3E)
    gsmChar["|"] = (0x1B, 0x40)
    gsmChar["€"] = (0x1B, 0x65)


def convString(s):
    buf = bytearray()
    defChar = gsmChar["?"]
    for c in s:
        for x in gsmChar.get(c, defChar):
            buf.append(x)
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


def encodeMessage(buf, msg):  # encode a string to a 7-bit sms message
    tmp = convString(msg)
    buf.append(len(tmp))
    pack7(buf, tmp)


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
