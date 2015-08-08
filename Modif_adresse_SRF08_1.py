#!/usr/bin/env python
import smbus
import time
bus = smbus.SMBus(1)

# Adresse ancienne (du SRF08) => 0x70 pour un SRF neuf, correspondant a 0xE0 car decalage a droite d un bit sur 4 => 1110 0000 devient 0111 0000, cela a cause d un bit pour read-write
address_old = 0x70

# Adresse nouvelle (a programmer dans le SRF08 : ex 0xE2 va donner une adresse 71 lu par i2cdetect)
address_new = 0xE4

#SRF08 REQUIRES 5V

#Code pour changer l adresse (a ecrire dans le seul registre du SRF08)
bus.write_byte_data(address_old, 0, 0xA0)
time.sleep(0.7)

bus.write_byte_data(address_old, 0, 0xAA)
time.sleep(0.7)

bus.write_byte_data(address_old, 0, 0xA5)
time.sleep(0.7)

#Ecriture de la nouvelle adresse
bus.write_byte_data(address_old, 0, address_new)
time.sleep(0.065)
