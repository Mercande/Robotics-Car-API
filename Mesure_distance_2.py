#!/usr/bin/env python
import smbus
import time
bus = smbus.SMBus(1)
address_capteur1 = 0x71
address_capteur2 = 0x72

#SRF08 REQUIRES 5V

def write(value):
        bus.write_byte_data(address, 0, value)
        return -1

def lightlevel():
        light = bus.read_byte_data(address, 1)
        return light

def range():
        range1 = bus.read_byte_data(address, 2)
        range2 = bus.read_byte_data(address, 3)
        range3 = (range1 << 8) + range2
        return range3

while True:
        address = address_capteur1
        write(0x51)
        time.sleep(0.7)
        lightlvl = lightlevel()
        rng = range()
        print "Capteur 1 : light = " + str(lightlvl) + "\tdistance = " + str(rng)
        
        address = address_capteur2
        write(0x51)
        time.sleep(0.7)
        lightlvl = lightlevel()
        rng = range()
        print "Capteur 2 : light = " + str(lightlvl) + "\tdistance = " + str(rng)

