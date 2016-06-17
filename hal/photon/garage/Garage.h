#include "Device.h"

class Garage : public Device
{
public:

  void ConfigPins();
  char * GetDeviceType();

  void ToggleLight();
  void ToggleGarage();
  
  int LightState();
  int DoorState();
  
  void UpdateTimeout(int seconds);

private:
  
  bool _lightOverride;
  int _lightTimeoutSeconds;
  Timer * _motionTimer;
  Timer * _overrideTimer;

  void MotionSensed();
  void LightTimedOut();
  void LightOverrideTimedOut();
};
