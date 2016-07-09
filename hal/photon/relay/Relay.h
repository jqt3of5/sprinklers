#ifndef RELAY_H
#define RELAY_H
#include "common/Device.h"

//relay
struct Zone
{
  uint8_t zone;

  uint16_t timeStart;
  uint16_t timeStop;
  uint8_t dayOfTheWeek;
  Zone()
  {
    zone = 0xFF;
    timeStart = 0;
    timeStop = 0;
    dayOfTheWeek = 0xFF;
  }
};
struct Schedule
{
  int numberOfZones;
  Zone * zones;
};

class Relay : public Device
{
public:
  static Relay * Instance;
  Relay();
  void ConfigPins();
  const char * GetDeviceType();

  void SaveSchedule(uint8_t * data, int len);
  uint8_t * ReadSchedule();

  void ToggleRelay(int relay);
  int RelayState(int relay);

  void SetDelay(int hours);
  void EnableSchedule(bool enabled);
private:
  void SetSchedule(uint8_t * data, uint8_t count);
  bool _isScheduleEnabled;
  int _delay;
  void RunSchedule();
  Schedule _schedule;
  Timer * _runSchedule;
};


#endif
