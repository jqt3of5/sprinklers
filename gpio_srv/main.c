#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "bcm2835.h"

int main(int argc, char ** argv)
{
	sockaddr_un server_address;
	sockaddr_un client_address;
	int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("Failed to open the socket");
		return 1;
	}

	memset(&server_address, 0, sizeof(server_address));
	server_address.sun_family = AF_UNIX;
	strcpy(server_address.sun_path, "/tmp/gpio.sock");

	int struct_len = sizeof(server_address);
	int rc = unlink(server_address.sun_path);
	if (rc < 0)
	{
		printf("Failed to unbind the socket: %d", errno);
		return 1;
	}
	rc = bind(sockfd, (sockaddr*)&server_address, struct_len);
	if (rc < 0)
	{
		printf("bind failed: %d", errno);
		return 1;
	}
	rc = listen(sockfd, 5);
	if (rc < 0)
	{
		printf("listen failed: %d", errno);
		return 1;
	}

	bcm2835_init();

	while(true)
	{
		socklen_t client_struct_len = sizeof(&client_address);
		int client_sockfd = accept(sockfd, (sockaddr*)&client_address, &client_struct_len);
		if (client_sockfd < 0)
		{
			printf("accept failed %d", errno);
			continue;
		}
		printf("accept succeeded: ");
		//The client should send a length of 0 to quit the communication. 
		char len;
		read(client_sockfd, &len, 1);
		printf("read length: %d ", len);
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
					case 0:
					if (operands & 0x01)
					{
						printf("Set to input, pin: %d\n", pin);
						bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
					}
					else
					{
						printf("Set to output, pin: %d\n", pin);
						bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
					}
					//TODO: Grab a second byte for pull up/down and functional
					break;
					//Read the pin value
					case 1:
					printf("Get input value, pin: %d\n", pin);

					val = bcm2835_gpio_lev(pin);
					break;
					//Set the pin value
					case 2:
					val = operands & 0x1;
					if (val)
					{
						printf("Set high, pin: %d\n", pin);
						bcm2835_gpio_set(pin);
					}
					else
					{
						printf("Set low, pin: %d\n", pin);
						bcm2835_gpio_clr(pin);
					}
					break;
					//None yet
					case 3:
					break;
				}
				//Respond to each byte.
				write(client_sockfd, &val, 1);
			}
			read(client_sockfd, &len, 1);
			printf("read length: %d ", len);
			if (len == 0)
			{
				printf("\n");
			}

		}
		close(client_sockfd);
	}
}
