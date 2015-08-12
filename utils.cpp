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


/************************************************************/
/**** Utils                                              ****/
/************************************************************/

void printLine() {
	printf("------------------------------------------------\n");
}

int convertBcmToWiring(int pin_bcm) {
	switch(pin_bcm) {
		case 18: return 1;
		case 23: return 4;
		case 24: return 5;
		case 25: return 6;
		case 4:  return 7;
		case 17: return 0;
		case 22: return 3;
	}
	return 1;
}

void tempo(unsigned int howLong) {
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = (time_t)(howLong / 1000) ;
  sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

  nanosleep (&sleeper, &dummy) ;
}
