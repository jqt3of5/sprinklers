#include <Particle.h>
#include "Relay.h"
const int EEPROM_SCHEDULE = 5;

Relay * Relay::Instance = new Relay();

Relay::Relay()
{
  ConfigPins();
  ReadSchedule();
  _runSchedule = new Timer(60000, [this]()->{this->RunSchedule()});
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
    Zone & zone = _schedule.zoneSchedules[i];
    //Make sure this schedule object is real
    if (zone.zone != -1 && _schedule )
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

char * Relay::GetDeviceType()
{
  return "relay";
}

unint8_t * Relay::ReadSchedule()
{
  uint8_t count = EEPROM.read(EEPROM_SCHEDULE);
  int byteCount = count * sizeof(Zone);
  uint8_t * data = new unint8_t[byteCount+1];
  data[0] = count;
  if (count != 0xFF)
  {
    for (int i = 0; i < byteCount; ++i)
    {
      data[i+1] = EEPROM.read(EEPROM_SCHEDULE + i + 1);
    }
  }

  SetSchedule(data);
  return data;
}

void Relay::SaveSchedule(unint8_t * data)
{
  uint8_t count = data[0] * sizeof(Zone);

  EEPROM.write(EEPROM_SCHEDULE, data[0]);
  for (int i = 0; i < count; ++i)
  {
    EEPROM.write(EEPROM_SCHEDULE + i + 1, data[i])
  }

  SetSchedule(data);
}

void Relay::SetSchedule(unint8_t * data)
{
    unint8_t count = data[0];

    for (int i = 0; i < _schedule.numberOfZones; ++i)
    {
      free(_schedule.zones[i]);
    }
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

void SetDelay(int hours)
{
  _delay = hours * 60;
}

void EnableSchedule(bool enabled)
{
  _isScheduleEnabled = enabled;
}
