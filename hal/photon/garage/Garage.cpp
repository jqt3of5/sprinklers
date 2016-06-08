#include <Particle.h>
#include "Garage.h"

void turn_off()
{
  digitalWrite(D7, LOW);
}

Timer timer(1000 * 60 * 5, turn_off, true);

void ISR1()
{
  digitalWrite(D7, digitalRead(D2));
  timer.start();
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
  attachInterrupt(D2, ISR1, RISING);
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
      else if (subCmd[0] == 'S')
      {
          response[0] = digitalRead(D7) == HIGH ? '1' : '0';
      }
    break;
    case 'D':
      if (subCmd[0] == 'T') //toggle
      {
        digitalWrite(D6, 1^digitalRead(D6));
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
