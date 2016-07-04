#include <Particle.h>
#include "Relay.h"
const int EEPROM_SCHEDULE = 5;

//TODO: These singletons step on each other. Figure out a way to get them to get along. 
Relay * Relay::Instance = nullptr;//new Relay();

Relay::Relay()
{
  ConfigPins();
  ReadSchedule();
  _runSchedule = new Timer(60000, [this]()-> void {this->RunSchedule();});
}

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

void Relay::RunSchedule()
{
  int currentTime = Time.hour() * 60 + Time.minute();
  for (int i = 0; i < 10; ++i)
  {
    Zone & zone = _schedule.zones[i];
    //Make sure this schedule object is real
    if (zone.zone != 0xFF)
    {
      //Make sure that we are on the correct day of the week.
      //Unless it isn't set
      if (zone.dayOfTheWeek == -1
        || zone.dayOfTheWeek == Time.weekday())
      {
          if (zone.timeStart < currentTime && zone.timeStop > currentTime)
          {
              digitalWrite(zone.zone, HIGH);
              continue;
          }
      }
      digitalWrite(zone.zone, LOW);
    }
  }
}

const char * Relay::GetDeviceType()
{
  char * str = new char[7];
  strcpy(str, "relay");
  return str;
}

uint8_t * Relay::ReadSchedule()
{
  uint8_t count = EEPROM.read(EEPROM_SCHEDULE);
  int byteCount = count * sizeof(Zone);
  uint8_t * data = new uint8_t[byteCount+1];
  data[0] = count;
  if (count != 0xFF)
  {
    for (int i = 0; i < byteCount; ++i)
    {
      data[i+1] = EEPROM.read(EEPROM_SCHEDULE + i + 1);
    }
  }

  SetSchedule(data, count);
  return data;
}

void Relay::SaveSchedule(uint8_t * data, int len)
{
  uint8_t count = len / sizeof(Zone);

  EEPROM.write(EEPROM_SCHEDULE, count);
  for (int i = 0; i < len; ++i)
  {
    EEPROM.write(EEPROM_SCHEDULE + i + 1, data[i]);
  }

  SetSchedule(data, count);
}

void Relay::SetSchedule(uint8_t * data, uint8_t count)
{
    free(_schedule.zones);

    _schedule.numberOfZones = count;

    _schedule.zones = new Zone[count];
    memcpy(_schedule.zones, data + 1, count * sizeof(Zone));
}

void Relay::ToggleRelay(int pin)
{
  digitalWrite(pin, 1^digitalRead(pin));
}

int Relay::RelayState(int pin)
{
  return digitalRead(pin);
}

void Relay::SetDelay(int hours)
{
  _delay = hours * 60;
}

void Relay::EnableSchedule(bool enabled)
{
  _isScheduleEnabled = enabled;
}
