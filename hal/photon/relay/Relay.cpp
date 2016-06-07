#include <Particle.h>
#include "Relay.h"
const int EEPROM_SCHEDULE = 5;
//relay
struct Schedule
{
  char data[64];
};


void Relay::ConfigPins()
{
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
}

char * Relay::ProcessData(char * data, int count)
{
  char cmd = data[0];
  char * temp;
  char pin = 0;
  char * result = 0;
  Schedule schedule;
  switch (cmd)
      {
        case 'S':
        //write schedule
        memcpy(&schedule, &data[1], count - 1);
        EEPROM.put(EEPROM_SCHEDULE, schedule);
        break;

        case 'R':
        strtok(data, " ");
        temp = strtok(nullptr, " ");
        pin = atoi(temp);

        result = new char[2]();
        result[0] = digitalRead(0) == HIGH ? '1' : '0';
        break;

        case 'W':
        strtok(data, " ");
        temp = strtok(nullptr, " ");
        pin = atoi(temp);
        temp = strtok(nullptr, " ");

        digitalWrite(pin, atoi(temp) ? HIGH : LOW);
        break;

      }
    return result;

}

char * Relay::GetDeviceType()
{
  return "relay";
}
