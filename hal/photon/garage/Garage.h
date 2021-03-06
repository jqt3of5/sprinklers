#ifndef GARAGE_H
#define GARAGE_H
#include "Device.h"
#include "IEventer.h"

class Garage : public Device
{
public:
  static Garage * Instance;
  //Garage(IEventer * eventer);
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
  //IEventer * _eventer;

  void MotionSensed();
  void LightTimedOut();
  void LightOverrideTimedOut();
};
#endif
