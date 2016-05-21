#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "bcm2835.h"

int main(int argc, char ** argv)
{
	bcm2835_init();
	int pin =  RPI_V2_GPIO_P1_03;
	bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_set(pin);
//	bcm2835_gpio_clr(pin);

	pin =  RPI_V2_GPIO_P1_05;
	bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_set(pin);
//	bcm2835_gpio_clr(pin);

	pin =  RPI_V2_GPIO_P1_07;
	bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_set(pin);
//	bcm2835_gpio_clr(pin);

}
