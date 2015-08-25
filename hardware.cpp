/**
 * This file is part of Robotics-Car-API, RestAPI on RaspberryPi B+.
 *
 * Copyright (c) 2015 Robotics-Car-API contributors (http://mercandalli.com)
 *
 * LICENSE:
 *
 * Robotics-Car-API is free software: you can redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any
 * later version.
 *
 * Robotics-Car-API is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * @author E.Mercandalli J.Mercandalli
 * @license http://www.gnu.org/licenses/gpl.html
 * @copyright 2015 Robotics-Car-API contributors (http://mercandalli.com)
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <math.h>
#include <assert.h>
#include "utils.h"

#include "hardware.h"

// Hardware Raspberry
#ifdef __APPLE__
#else
	// Lib Control servo pwm AdaFruit9685
	#include "pca/pca9685.h"

	#include "wiringPi/wiringPi.h"
	#include "wiringPi/wiringPiI2C.h"
#endif

static int fd_pwm = -9999;


/************************************************************/
/**** Hardware functions                                 ****/
/************************************************************/

int hardware_init_pin(int pin_bcm, int input_output) {

	#ifdef __APPLE__
	#else
		int pin = convertBcmToWiring(pin_bcm);		
	  	//pinMode(pin, OUTPUT);
		pinMode(pin, input_output);
	#endif

	return 0;
}

int hardware_write_pin(int pin_bcm, int on_off) {

	#ifdef __APPLE__
	#else
		int pin = convertBcmToWiring(pin_bcm);
	  	//pinMode(pin, OUTPUT) ;
		digitalWrite(pin, on_off);
	#endif

	return 0;
}

double hardware_read_pin(int pin_bcm) {

	#ifdef __APPLE__
	#else
		int pin = convertBcmToWiring(pin_bcm);
	  	//pinMode(pin, OUTPUT) ;
		return digitalRead(pin);
	#endif

	return -1;
}

double hardware_read_distance_i2c(int ad) {
	// Reading distance and brightness: SRF08
	// (Use i2cdetect -y 1 command to find device address : here 0x71 and 0x72)

	int distance = 0;

	#ifdef __APPLE__
	#else
		int fd_dist = wiringPiI2CSetup(ad);		// ad = 0x71 and 0x72
		if(fd_dist == -1) {
			printf("Can't setup the I2C device (distance SRF08)\n");
		 	return -1;
	  	}
		// printf ("Setup I2C device (distance SRF08) OK - numero : %d \n", fd_dist);

		// Started the distance measure
		int j = wiringPiI2CWriteReg8(fd_dist, 0, 0x51);
		//printf ("Write j (0 attendu ; -1 si pb) : %d \n", j);
		
		// waiting for measure
		tempo(75); // default 680

		/*
		// reading the brightness
		int k = wiringPiI2CReadReg8(fd_dist, 1);
		//printf ("Brightness level: %d \n", k);

		if(k<0)
			return -1;
		*/

		// reading the distance
		int range1 = wiringPiI2CReadReg8(fd_dist, 2);
		int range2 = wiringPiI2CReadReg8(fd_dist, 3);
		distance = (range1 << 8) + range2;
		//printf ("Distance: %d \n", distance);
	#endif
	
	return distance < 0 ? -2 : distance;
}

double* hardware_read_distance_i2c_all() {
	double* result = new double[2];

	int distance_1 = 0;
	int distance_2 = 0;

	#ifdef __APPLE__
	#else
		int fd_dist_1 = wiringPiI2CSetup(0x71);		// ad = 0x71 and 0x72
		int fd_dist_2 = wiringPiI2CSetup(0x72);		// ad = 0x71 and 0x72
		if(fd_dist_1 == -1 || fd_dist_2 == -1) {
			printf("Can't setup the I2C device (distance SRF08)\n");
			result[0] = -1;
			result[1] = -1;
		 	return result;
	  	}
		// printf ("Setup I2C device (distance SRF08) OK - numero : %d \n", fd_dist);

		// Started the distance measure
		wiringPiI2CWriteReg8(fd_dist_1, 0, 0x51);
		wiringPiI2CWriteReg8(fd_dist_2, 0, 0x51);
		//printf ("Write j (0 attendu ; -1 si pb) : %d \n", j);
		
		// waiting for measure
		tempo(75);

		// reading the distance
		int range1 = wiringPiI2CReadReg8(fd_dist_1, 2);
		int range2 = wiringPiI2CReadReg8(fd_dist_1, 3);
		distance_1 = (range1 << 8) + range2;

		range1 = wiringPiI2CReadReg8(fd_dist_2, 2);
		range2 = wiringPiI2CReadReg8(fd_dist_2, 3);
		distance_2 = (range1 << 8) + range2;
	#endif
	
	result[0] = distance_1 < 0 ? -2 : distance_1;
	result[1] = distance_2 < 0 ? -2 : distance_2;

	return result;
}


int hardware_write_servo_i2c(int ad, int id_pwm, double value) {
	
	#ifdef __APPLE__
	#else
		// Init
		if(fd_pwm == -9999) {
			fd_pwm = pca9685Setup(300, ad, 60);	// int pinBase (>64 eg.300), const int i2cAddress (default : 0x40), float freq (default : 50)
			if(fd_pwm == -1)
				return -1;
		}

		// Secure hardware
		if(value < 0) 			value = 0;
		else if(value > 1) 	value = 1;		

		// --- servos activation : value range
		//			servoMin = 380  		# Min pulse length out of 4096	(servo right max)
		//			servoCenter = 410		#	
		//			servoMax = 480  		# Max pulse length out of 4096	(servo left max)
		value = 350 + 120*value;
		pca9685PWMWrite(fd_pwm, id_pwm, 0, value); // int fd, int pin, int on, int off
	#endif

	return 0;
}

double hardware_read_distance(int device) {
	switch(device) {
		case 1: return hardware_read_distance_i2c(0x71);
		case 2: return hardware_read_distance_i2c(0x72);
	}
	return 0;
}

int hardware_write_servo(int device, double value) {
	switch(device) {
		case 1: return hardware_write_servo_i2c(0x40, 0, value);
		case 2: return hardware_write_servo_i2c(0x40, 15, value);
	}
	return 0;
}




/************************************************************/
/**** Public Functions called by the server              ****/
/************************************************************/

int hardware_init() {
	int result = 0;

	#ifdef __APPLE__
	#else	

		wiringPiSetup();

		//
		// Init pin LED (GPIO Blackberry)
		//
		result |= hardware_init_pin(18, OUTPUT);

		// --- Init driver servo PWM (Adafruit 9685)
		// 	 (Use i2cdetect -y 1 command to find device address : here 0x40 default address)
		//
		fd_pwm = pca9685Setup(300, 0x40, 60);	// int pinBase (>64 eg.300), const int i2cAddress (default : 0x40), float freq (default : 50)
		if(fd_pwm == -1)
			return -1;
   	// printf ("Setup I2C PWM device (Adafruit : PCA 9685) OK - numero : %d \n", fd_pwm);

	#endif
	return result;
}

int hardware_write(int id, double value) {
	printf("hardware_write(id=%d, value=%lf)\n", id, value);

	switch(id) {

		case ID_LED_1 :
			return hardware_write_pin(18, value);

		case ID_DISTANCE_1:
			return -1;

		case ID_DISTANCE_2:
			return -1;

		case ID_SERVO_1:
			return hardware_write_servo(1, value);

		case ID_SERVO_2:
			return hardware_write_servo(2, value);
	}
	return -1;
}

double hardware_read(int id) {
	double result = 0;

	switch(id) {

		case ID_LED_1:		
			result = hardware_read_pin(18);
		break;

		case ID_DISTANCE_1:
			result = hardware_read_distance(1);
		break;

		case ID_DISTANCE_2:
			result = hardware_read_distance(2);
		break;

		case ID_SERVO_1:
			result = -1;
		break;

		case ID_SERVO_2:
			result = -1;
		break;
	}

	printf("hardware_read(id=%d)  =  %lf\n", id, result);

	return result;
}

double* hardware_read_distance() {
	return hardware_read_distance_i2c_all();
}



/************************************************************/
/**** Test functions called each time the program start  ****/
/************************************************************/

int hardware_test() {

	printf("\n");
	printLine();
	printf("TEST : HARDWARE\n\n");

	// clignotement LED
	int time =  100;
	for(int i = 0; i< 3; i++) {
		hardware_write(1, 1);
		assert(1 == hardware_read(1));
		tempo(time);
		printf("\n");

		hardware_write(1, 0);
		assert(0 == hardware_read(1));
		tempo(time);
		printf("\n");
	}

	// lecture capteurs distance
	hardware_read(ID_DISTANCE_1);
	hardware_read(ID_DISTANCE_2);

	// commande des servos
	// --- --- a droite
	hardware_write(ID_SERVO_1, 0);
	hardware_write(ID_SERVO_2, 0);
	tempo (2000);

	// --- --- a gauche
	hardware_write(ID_SERVO_1, 1);
	hardware_write(ID_SERVO_2, 1);
	tempo (2000);

	// --- --- au centre
	hardware_write(ID_SERVO_1, 0.5);
	hardware_write(ID_SERVO_2, 0.5);

	printf("\n");

	printLine();

	return 0;
}



