# Sprinklers
Born of the desire to have a feature rich sprinkler controller, I decided to design my own. In the process, several other home automation devices were envisioned. 
The heart of the system is the Particle Photon. This little device, based around a broadcom processor, has a built in wifi module. Along with all of the features 
built into the firmware, makes this the perfect device for devoloping an IoT. 

## Description
The photons are designed to operate in standalone mode, or cloud mode. In standalone mode, the photon activates its softAP that a user can connect to and control
the features of the device, and configure it to operate in cloud mode. 

In cloud mode, the device will connect to the specified access point, the attempt to connect to the cloud service. 

The cloud service exposes a RESTful API that can be used by a client to configure/control each of the devices that are connected. 

## How the code is organized
The code is broken into two major directories:
### controller
This is the cloud service. It accepts TCP socket connections from devices. And uses NodeJS to serve up a RESTful express server that exposes an API that can then issue commands to the connected devices. 
The dashboard UI for interacting with the API is also found here. 
### hal
The hardware abstraction layer(hal) is where all of the photon code resides. It provides the foundation for desining more IoT. As well as concrete implementations for devies that already exist. 
