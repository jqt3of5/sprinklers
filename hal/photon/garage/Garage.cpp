#include <Particle.h>
#include "Garage.h"

Garage::Garage()
{
  _lightOverride = false;
  _lightTimeoutSeconds = 60;
  _motionTimer = new Timer(1000 * _lightTimeoutSeconds, [this]() -> void {this->LightTimedOut();},  true);
  _overrideTimer = new Timer(1000 * 10 * _lightTimeoutSeconds, [this]() -> void {this->LightOverrideTimedOut();},  true);
  _garageDoorPulseTimer = new Timer(500, []() -> {digitalWrite(D6, LOW);});
}

void Garage::LightTimedOut()
{
  if (!_lightOverride)
  {
    digitalWrite(D7, LOW);
  }
}

void Garage::LightOverrideTimedOut()
{
  if (!digitalRead(D2))
  {
    LightTimedOut();
  }
  _lightOverride = false;
}
void Garage::MotionSensed()
{
  digitalWrite(D7, digitalRead(D2));
  _motionTimer->startFromISR();
}

void Garage::ToggleLight()
{
  _lightOverride = true;
  digitalWrite(D7, 1^digitalRead(D7));
  _overrideTimer->startFromISR();
}

void Garage::ToggleGarage()
{
  digitalWrite(D6, HIGH);
 _garageDoorPulseTimer->startFromISR();
}


void Garage::ConfigPins()
{
  //Light
  pinMode(D7, OUTPUT);
  //Door
  pinMode(D6, OUTPUT);
  //Reed Switches
  pinMode(D0, INPUT); // Open
  pinMode(D1, INPUT); // Close
  //Light Toggle Button
  pinMode(D2, INPUT_PULLDOWN);
  attachInterrupt(D2, [this]() -> void {this->ToggleLight();}, RISING);
  //Garage Toggle Button
  pinMode(D3, INPUT_PULLDOWN);
  attachInterrupt(D2, [this]() -> void {this->ToggleGarage();}, RISING);
  //motion
  pinMode(D4, INPUT_PULLDOWN);
  attachInterrupt(D4,[this]() -> void {this->MotionSensed();}, RISING);
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
        digitalWrite(D7, 1^digitalRead(D7));
      }
    else if (!strcmp(subCmd,"S")) //state
      {
          response = new char[2]();
          response[0] = digitalRead(D7) ? '1' : '0';
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
        response[0] = digitalRead(D0) == HIGH ? '1' : (digitalRead(D1) == HIGH ? '0' : '-1');
      }
      
      return response;
}

char * Garage::GetDeviceType()
{
  return "garage";
}
