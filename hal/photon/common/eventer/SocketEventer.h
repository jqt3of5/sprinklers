#include "IEventer.h"

class SocketEventer : public IEventer
{
public:
    SocketEventer(char * url, int port, const char * deviceId);
    void eventNotification(char * event, char * data);

  protected:
    char * _url;
    int _port;
    char * _deviceId;
};
