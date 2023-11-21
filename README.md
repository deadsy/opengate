# opengate
A homebrew open source gate entry system.

## Gate Systems

A gate system has 3 components.

* gate: the controlled barrier
* gate opener: the motor that opens/closes the gate
* gate entry system: the keypad/camera/telephone to control access

A person who wants access approaches the entry system and:

* enters a security code: if the code is correct they are granted access. 
* calls a designated person(s): if the called person grants access they send some dtmf tones to open the gate.


## Problem

Gate entry systems are expensive to buy, install and operate.

* Hardware $2000 
* Installation $2000
* Connectivity $50/month
* Service $30/month

In my case the gate system came with the property and uses a land line for connectivity.
The only reason I need a land line is for the gate system. This costs around $160/month.

## Solution

The cost for a cellular based solution is around $15/month for a cheap connectivity provider.

E.g. https://tello.com/ 

The service provider needs to support the modem used.
This can be confirmed by checking the modem's IMEI on the "bring your own phone" page.


# parts

Raspberry Pi Pico (rp2040)

Cellular Modem SIM7600G-H https://www.waveshare.com/wiki/SIM7600E-H_4G_HAT

speaker (output)

microphone (input)

16x2 lcd (output)

keyboard + buttons (input)

MT8870 DTMF decoder (the cellular modem doesn't have it :-()

Relay control board https://www.waveshare.com/rpi-relay-board.htm



