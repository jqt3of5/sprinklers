#ifndef GARAGE_H
#define GARAGE_H
#include "Device.h"

class Garage : public Device
{
public:
  static Garage * Instance;
  Garage();
  void ConfigPins();
  const char * GetDeviceType();

  void ToggleLight();
  void ToggleDoor();

  int LightState();
  int DoorState();

  void UpdateTimeout(int seconds);

private:

  bool _lightOverride;
  int _lightTimeoutSeconds;
  Timer * _motionTimer;
  Timer * _overrideTimer;
  Timer * _garageDoorPulseTimer;

  void MotionSensed();
  void LightTimedOut();
  void LightOverrideTimedOut();
};
#endif
