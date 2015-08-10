#include <stdio.h>
#include <stdint.h>
#include "hardware.h"

#ifdef __APPLE__

#else

	// Hardware Raspberry
	#include "wiringPi/wiringPi.h"
	#include "wiringPi/wiringPiI2C.h"

	//Run python sudo apt-get install python-dev
	#include "/usr/include/python2.7/Python.h"

#endif

/************************************************************/
/**** Hardware functions                                 ****/
/************************************************************/

int gpio_write_pin(int pin_bcm, int on_off) {

	#ifdef __APPLE__

	#else
		int pin = convertBcmToWiring(pin_bcm);
		wiringPiSetup () ;
	  	pinMode (pin, OUTPUT) ;
		digitalWrite (pin, on_off);
	#endif

	return 0;
}

double gpio_read_pin(int pin) {
	// TODO

	return 0;
}

double gpio_read_distance(int device) {
	// TODO

	return 0;
}