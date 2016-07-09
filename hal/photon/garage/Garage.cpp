#include <Particle.h>
#include "Garage.h"

#define GARAGE_BUTTON D3 // The button to push to toggle the garage door
#define LIGHT_BUTTON D2 // the button to push to toggle the light
#define GARAGE_SWITCH D6 //The switch that opens/closes the garage door
#define LIGHT_SWITCH D7 //The siwtch that turns the light on/off
#define MOTION_INPUT D4 //The input from the motion detector
#define REED_OPEN D0 //The reed switch that indicates the garage door is open
#define REED_CLOSE D1 //The reed switch that indicates the garage door is closed

Garage::Garage(IEventer * eventer) : Garage()
{
  _eventer = eventer;
}

Garage::Garage()
{
  ConfigPins();

  _lightOverride = false;
  _lightTimeoutSeconds = 60;
  _motionTimer = new Timer(1000 * _lightTimeoutSeconds, [this]() -> void {this->LightTimedOut();},  true);
  _overrideTimer = new Timer(1000 * 10 * _lightTimeoutSeconds, [this]() -> void {this->LightOverrideTimedOut();},  true);
  _garageDoorPulseTimer = new Timer(500, []() -> void {digitalWrite(GARAGE_SWITCH, LOW);});
}

void Garage::ConfigPins()
{
  //Light
  pinMode(LIGHT_SWITCH, OUTPUT);
  //Door
  pinMode(GARAGE_SWITCH, OUTPUT);
  //Reed Switches
  pinMode(REED_OPEN, INPUT); // Open
  pinMode(REED_CLOSE, INPUT); // Close
  //Light Toggle Button
  pinMode(LIGHT_BUTTON, INPUT_PULLDOWN);
  attachInterrupt(LIGHT_BUTTON, [this]() -> void {this->ToggleLight();}, RISING);
  //Garage Toggle Button
  pinMode(GARAGE_BUTTON, INPUT_PULLDOWN);
  attachInterrupt(GARAGE_BUTTON, [this]() -> void {this->ToggleDoor();}, RISING);
  //motion
  pinMode(MOTION_INPUT, INPUT_PULLDOWN);
  attachInterrupt(MOTION_INPUT,[this]() -> void {this->MotionSensed();}, RISING);
}

void Garage::LightTimedOut()
{
  //If the Motion detection is high, then we do not want the light to turn off. So just restart the timer
  if (digitalRead(MOTION_INPUT))
  {
    _motionTimer->startFromISR();
    return;
  }
  if (!_lightOverride)
  {
    digitalWrite(LIGHT_SWITCH, LOW);
  }
}

void Garage::LightOverrideTimedOut()
{
  _lightOverride = false;
  LightTimedOut();

}
void Garage::MotionSensed()
{
  digitalWrite(LIGHT_SWITCH, HIGH);
  _motionTimer->startFromISR();
}

void Garage::ToggleLight()
{
  _lightOverride = true;
  digitalWrite(LIGHT_SWITCH, 1^digitalRead(LIGHT_SWITCH));
  _overrideTimer->startFromISR();
}

void Garage::ToggleDoor()
{
  digitalWrite(GARAGE_SWITCH, HIGH);
 _garageDoorPulseTimer->startFromISR();
}

int Garage::LightState()
{
  return digitalRead(LIGHT_SWITCH);
}

int Garage::DoorState()
{
  return digitalRead(REED_OPEN) ? 1 : (digitalRead(REED_CLOSE) ? 0 : -1);
}

void Garage::UpdateTimeout(int seconds)
{
  _lightTimeoutSeconds = seconds;
  _motionTimer->changePeriod(1000 * _lightTimeoutSeconds);
  _overrideTimer->changePeriod(1000 * 10 * _lightTimeoutSeconds);
}

const char * Garage::GetDeviceType()
{
  char * str = new char[7];
  strcpy(str, "garage");
  return str;
}
