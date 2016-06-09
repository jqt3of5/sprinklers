#include <Particle.h>
#include "Garage.h"

Garage::Garage()
{
  _lightTimeoutSeconds = 60;
  _timer = new Timer(1000 * _lightTimeoutSeconds, LightTimedOut, true);
}

void Garage::LightTimedOut()
{
  digitalWrite(D7, LOW);
}

void Garage::MotionSensed()
{
  digitalWrite(D7, digitalRead(D2));
  _timer.start();
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
  //motion
  pinMode(D2, INPUT_PULLDOWN);
  attachInterrupt(D2, MotionSensed, RISING);
}


char * Garage::ProcessData(char * data, int count)
{
  char *cmd = strtok(data, " ");;
  char *subCmd = strtok(nullptr, " ");

  char * response = new char[2]();
  switch(cmd[0])
  {
    case 'L':
      if (subCmd[0] == 'T') //toggle
      {
        digitalWrite(D7, 1^digitalRead(D7));
      }
      else if (subCmd[0] == 'S') //state
      {
          response[0] = digitalRead(D7) == HIGH ? '1' : '0';
      }
      else if (subCmd[0] == 'O') //Update Light time out
      {
        unsigned int timeout = atoi(strtok(nullptr, " "));
        _timer.changePeriod(1000 * timeout);
      }
    break;
    case 'D':
      if (subCmd[0] == 'T') //I know it's a T, but pulse the pin to toggle the garage door 
      {
        digitalWrite(D6, HIGH);
        System.delay(500);
        digitalWrite(D6, LOW);
      }
      else if (subCmd[0] == 'S')
      {
        response[0] = digitalRead(D0) == HIGH ? '1' : (digitalRead(D1) == HIGH ? '0' : '-1');
      }
    break;
  }
  return response;
}

char * Garage::GetDeviceType()
{
  return "garage";
}
