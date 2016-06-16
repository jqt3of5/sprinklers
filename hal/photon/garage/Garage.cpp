#include <Particle.h>
#include "Garage.h"

#define GARAGE_BUTTON D3 // The button to push to toggle the garage door
#define LIGHT_BUTTON D2 // the button to push to toggle the light
#define GARAGE_SWITCH D6 //The switch that opens/closes the garage door
#define LIGHT_SWITCH D7 //The siwtch that turns the light on/off
#define MOTION_INPUT D4 //The input from the motion detector
#define REED_OPEN D0 //The reed switch that indicates the garage door is open
#define REED_CLOSE D1 //The reed switch that indicates the garage door is closed

Garage::Garage()
{
  _lightOverride = false;
  _lightTimeoutSeconds = 60;
  _motionTimer = new Timer(1000 * _lightTimeoutSeconds, [this]() -> void {this->LightTimedOut();},  true);
  _overrideTimer = new Timer(1000 * 10 * _lightTimeoutSeconds, [this]() -> void {this->LightOverrideTimedOut();},  true);
  _garageDoorPulseTimer = new Timer(500, []() -> {digitalWrite(GARAGE_SWITCH, LOW);});
}

void Garage::LightTimedOut()
{
  if (!_lightOverride)
  {
    digitalWrite(LIGHT_SWITCH, LOW);
  }
}

void Garage::LightOverrideTimedOut()
{
  if (!digitalRead(LIGHT_BUTTON))
  {
    LightTimedOut();
  }
  _lightOverride = false;
}
void Garage::MotionSensed()
{
  digitalWrite(LIGHT_SWITCH, digitalRead(LIGHT_BUTTON));
  _motionTimer->startFromISR();
}

void Garage::ToggleLight()
{
  _lightOverride = true;
  digitalWrite(LIGHT_SWITCH, 1^digitalRead(LIGHT_SWITCH));
  _overrideTimer->startFromISR();
}

void Garage::ToggleGarage()
{
  digitalWrite(GARAGE_SWITCH, HIGH);
 _garageDoorPulseTimer->startFromISR();
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
  attachInterrupt(LIGHT_BUTTON, [this]() -> void {this->ToggleGarage();}, RISING);
  //motion
  pinMode(MOTION_INPUT, INPUT_PULLDOWN);
  attachInterrupt(MOTION_INPUT,[this]() -> void {this->MotionSensed();}, RISING);
}


char * Garage::ProcessData(char * data, int count)
{
  char *cmd = strtok(data, " ");
  char *subCmd = strtok(nullptr, " ");
  
  char * args[10] = {strtok(nullptr, " ")};
  for(int i = 0; arg[i] != null; i++)
  {
    args[i+1] = strtok(nullptr, " ");
  }
  
  char * response;
  if (!strcmp(cmd, "L"))
  {
    response = HandleLightCommand(subcmd, args);
  }
  else if (!strcmp(cmd, "D"))
  {
    response = HandleDoorCommand(subcmd, args);
  }
  
  return response;
}

char * Garage::HandleLightCommand(char *subCmd, char* args[])
{
    char * response = nullptr;
    if (!strcmp(subCmd,"T")) //toggle
      {
        digitalWrite(LIGHT_SWITCH, 1^digitalRead(LIGHT_SWITCH));
      }
    else if (!strcmp(subCmd,"S")) //state
      {
          response = new char[15]();
          sprintf(response, "{\"state\":%d}", digitalRead(LIGHT_SWITCH));
      }
    else if (!strcmp(subCmd,"O")) //Update Light time out
      {
        _lightTimeoutSeconds = atoi(args[0]);
        _motionTimer->changePeriod(1000 * _lightTimeoutSeconds);
        _overrideTimer->changePeriod(1000 * 10 * _lightTimeoutSeconds);
      }
      
      return response;
}

char * Garage::HandleDoorCommand(char *subCmd, char* args[])
{
    char * response = nullptr;
    if (!strcmp(subCmd, "T")) //I know it's a T, but pulse the pin to toggle the garage door
      {
        ToggleGarage();
      }
    else if (!strcmp(subCmd,"S"))
      {
        response = new char[2]();
        int state = digitalRead(REED_OPEN) == HIGH ? '1' : (digitalRead(REED_CLOSE) == HIGH ? '0' : '-1');
        sprintf(response, "{\"state\":%d}",state);
      }
      
      return response;
}

char * Garage::GetDeviceType()
{
  return "garage";
}
