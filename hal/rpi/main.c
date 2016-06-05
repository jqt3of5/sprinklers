//=========================================================================
// Author: John Todd
// Date: 22-5-2016
// Description: This hardware service provides an abstraction and interface
//              to the GPIO pins. It uses the bcm2835 GPIO library to interact 
//              with the hardware. And it provides either a UNIX or INET
//              socket interface.
//
// Command Format:
//    Config      - 00pppppd
//    Input Value - 01pppppx
//    Set Output  - 10pppppd
//=========================================================================
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "bcm2835.h"

int create_unix_socket_server(char * path);
int create_inet_socket_server(int port);


int main(int argc, char ** argv)
{
	if (argc < 3)
	{
		printf("Usage:\n \tgpio unix <path>\n \tgpio inet <port>");
		return 1;
	}
	int sockfd;
	if (!strcmp(argv[1], "unix"))
	{
		sockfd = create_unix_socket_server(argv[2]);
	}
	else if (!strcmp(argv[1], "inet"))
	{
		sockfd = create_inet_socket_server(atoi(argv[2]));
	}

	if (sockfd < 0)
	{
		return 1;
	}

	bcm2835_init();

	while(true)
	{
		sockaddr_un client_address;
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
					printf("Get input value, pin: %d ", pin);

					val = bcm2835_gpio_lev(pin);
					printf("Pin value: %d\n", val);
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

bool create_socket(int sockfd, sockaddr * socket_address)
{
	int struct_len = sizeof(*socket_address);
	int rc = bind(sockfd, socket_address, struct_len);
	if (rc < 0)
	{
		printf("bind failed: %d", errno);
		return false;
	}
	rc = listen(sockfd, 5);
	if (rc < 0)
	{
		printf("listen failed: %d", errno);
		return false;
	}
	return true;
}

int create_inet_socket_server(int port)
{
	sockaddr_in server_address;

	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("Failed to open the socket");
		return -1;
	}

	if (create_socket(sockfd, (sockaddr*)&server_address))
	{
		return sockfd;
	}

	return -1;
}

int create_unix_socket_server(char * path)
{
	sockaddr_un socket_address;

	memset(&socket_address, 0, sizeof(socket_address));
	socket_address.sun_family = AF_UNIX;
	strcpy(socket_address.sun_path, path);

	int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("Failed to open the socket");
		return -1;
	}

	unlink(socket_address.sun_path);

	if(create_socket(sockfd, (sockaddr*)&socket_address))
	{
		return sockfd;
	}

	return -1;
}
