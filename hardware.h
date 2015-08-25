#ifndef HARDWARE_H
#define HARDWARE_H

static const int ID_LED_1		= 1;
static const int ID_DISTANCE_1	= 2;
static const int ID_DISTANCE_2	= 3;
static const int ID_SERVO_1		= 4;
static const int ID_SERVO_2		= 5;

static const int AD_DISTANCE_1	= 0x71;
static const int AD_DISTANCE_2	= 0x72;

int hardware_init();
int hardware_write(int id, double value);
double hardware_read(int id);
double* hardware_read_distance();

int hardware_test();

#endif
