#ifndef IEVENTER_H
#define IEVENTER_H
class IEventer
{
public:
  virtual void eventNotification(char * event, char * data);
};

#endif
