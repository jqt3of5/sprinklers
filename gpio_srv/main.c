#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include "bcm2835.h"

int main(int argc, char ** argv)
{
	sockaddr_un server_address;
	sockaddr_un client_address;
	int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("Failed to open the socket");
	}
	server_address.sun_family = AF_UNIX;
	strcpy(server_address.sun_path, "gpio_server");

	int struct_len = sizeof(server_address);
	bind(sockfd, (sockaddr*)&server_address, struct_len);

	listen(sockfd, 5);
	while(true)
	{
		int client_struct_len = 0;
		int client_sockfd = accept(sockfd, (sockfd*)&client_address, &client_struct_len);

		//The client should send a length of 0 to quit the communication. 
		char len;
		read(client_sockfd, &len, 1);
		while (len > 0)
		{
			char buffer[128] = {0};
			read(client_sockfd, &buffer, len);

			for(int i = 0; i < len; ++i)
			{
				//Grab the first two bits for the command;
				char cmd = (buffer[i] & 0xC0) >> 6;
				//And the rest for the operand
				char operands = buffer[i] & 0x3F;
				uint8_t pin, val = 0;
				pin = (operands & 0x3E) >> 1;

				switch(cmd)
				{
					//Config the pin
					switch 0:
					if (operands & 0x01)
					{
						bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
					}
					else
					{
						bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
					}
					//TODO: Grab a second byte for pull up/down and functional
					break;
					//Read the pin value
					switch 1:
					val = bcm2835_gpio_lev(pin);
					break;
					//Set the pin value
					switch 2:
					val = operands & 0x1;
					if (val)
					{
						bcm2835_gpio_set(pin);
					}
					else
					{
						bcm2835_gpio_clr(pin);
					}
					break;
					//None yet
					switch 3:
					break;
				}
				//Respond to each byte.
				write(client_sockfd, &val, 1);
			}
			read(client_sockfd, &len, 1);
			close(client_sockfd);
		}
	}
}
