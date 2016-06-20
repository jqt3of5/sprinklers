#include "Garage.h"

class GarageDoorCommand : public Command, public ICommandFactory
{
    public:
    GarageDoorCommand() {}
    GarageDoorCommand(TCPClient * client) : Command(client){}

    bool IsCommandPrefix(char * prefix)
    {
      return !strcmp(prefix, "D");
    }

    ICommand * CreateCommand(TCPClient * client, char *data, int len)
    {
        Command * command = new GarageDoorCommand(client);
        command->ParseCommand(data, len);
        return command;
    }

    void Execute()
    {
        Garage * garage = Garage::Instance;
        char * response = nullptr;
        if (!strcmp(_subcmd,"T")) //toggle
          {
            garage->ToggleDoor();
            _client->write("OK");
          }
        else if (!strcmp(_subcmd,"S")) //state
          {
            int state = garage->DoorState();
            char stateStr[30] = {0};
            sprintf(stateStr, "{\"state\":%d, \"mem\":%d}", state, System.freeMemory());
            _client->write(stateStr);

          }
          else
          {
            _client->stop();
          }
    }
};

class GarageLightCommand : public Command, public ICommandFactory
{
    public:
      GarageLightCommand() {}
      GarageLightCommand(TCPClient * client) : Command(client) {}

      bool IsCommandPrefix(char * prefix)
      {
        return !strcmp(prefix, "L");
      }

     ICommand * CreateCommand(TCPClient * client, char *data, int len)
      {
          Command * command = new GarageLightCommand(client);
          command->ParseCommand(data, len);
          return command;
      }

      void Execute()
      {
          Garage * garage = Garage::Instance;
          char * response = nullptr;
          if (!strcmp(_subcmd,"T")) //toggle
            {
              garage->ToggleLight();
              _client->write("OK");
            }
          else if (!strcmp(_subcmd,"S")) //state
            {
              int state = garage->LightState();
              char stateStr[30] = {0};
              sprintf(stateStr, "{\"state\":%d, \"mem\":%d}", state, System.freeMemory());
              _client->write(stateStr);
            }
          else if (!strcmp(_subcmd,"O")) //Update Light time out
            {
              garage->UpdateTimeout(atoi(_args[0]));
              _client->write("OK");
            }
          else
          {
              _client->stop();
          }
      }
};
