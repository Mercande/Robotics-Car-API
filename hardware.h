#ifndef HARDWARE_H
#define HARDWARE_H

static const int ID_LED_1		= 1;
static const int ID_DISTANCE_1	= 2;
static const int ID_DISTANCE_2	= 3;
static const int ID_SERVO_1		= 4;
static const int ID_SERVO_2		= 5;

int hardware_init();
int hardware_write(int id, double value);
double hardware_read(int id);

int hardware_test();

#endif
