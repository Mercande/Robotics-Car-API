#ifndef HARDWARE_H
#define HARDWARE_H

int gpio_write_pin(int pin_bcm, int on_off);
double gpio_read_pin(int pin);

double gpio_read_distance(int device);

#endif