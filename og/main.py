# open gate

from machine import Pin
import time

def main():
    led = Pin(25, Pin.OUT)
    x = 1
    while True:
        print("loop %d" % x)
        led.toggle()
        x += 1
        time.sleep(1)

main()
