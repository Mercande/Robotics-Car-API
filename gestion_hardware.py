#!/usr/bin/python

import sys, os, wiringpi2 as wiringpi, json

# exporter la broche de le LED
#os.system( 'gpio export 18 out' )

## configurer la broche de la LED en sortie
#io = wiringpi.GPIO(wiringpi.GPIO.WPI_MODE_SYS)
#io.pinMode(18,io.OUTPUT)

#recuperation des arguments
argument1 = int(sys.argv[1])
if argument1 == 0:
  OnOff=0
else:
  OnOff=1

#json.loads(argument1)

# GPIO port numbers  
wiringpi.wiringPiSetupGpio()
#wiringpi.pinMode(25, 0) # sets GPIO 25 to input
#wiringpi.pinMode(24, 1) # sets GPIO 24 to output
#wiringpi.pinMode(18, 2) # sets GPIO 18 to PWM mode


#code_de_retour = subprocess.call(["kdesudo", "./init_GPIO.py"])
#if code_de_retour==0 print 'init GPIO OK'


wiringpi.pinMode(18,1) # Set pin 18 to 1 ( OUTPUT )
wiringpi.digitalWrite(18,OnOff) # Write 0 ou 1 ( LOW ou HIGH ) to pin 18
etat_LED = wiringpi.digitalRead(18) # Read pin 18

etat = str(etat_LED)

print "Etat de la LED : " + etat
