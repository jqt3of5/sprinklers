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
        Garage * garage = new Garage();
        char * response = nullptr;
        if (!strcmp(_subcmd,"T")) //toggle
          {
            garage->ToggleDoor();
          }
        else if (!strcmp(_subcmd,"S")) //state
          {
            int state = garage->DoorState();
          }
          free(garage);
    }
};

class GarageLightCommand : public Command, public ICommandFactory
{
    public:
      GarageLightCommand() {}
      GarageLightCommand(TCPClient * client) : Command(client) {}

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
          Garage * garage = new Garage();
          char * response = nullptr;
          if (!strcmp(_subcmd,"T")) //toggle
            {
              garage->ToggleLight();
            }
          else if (!strcmp(_subcmd,"S")) //state
            {
              int state = garage->LightState();
            }
          else if (!strcmp(_subcmd,"O")) //Update Light time out
            {
              garage->UpdateTimeout(atoi(_args[0]));
            }
            free(garage);
      }
};
