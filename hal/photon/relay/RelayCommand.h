#include "Relay.h"

class RelayCommand : public Command, public ICommandFactory
{
    public:
    RelayCommand() {}
    RelayCommand(TCPClient * client) : Command(client){}

    bool IsCommandPrefix(char * prefix)
    {
      return !strcmp(prefix, "R");
    }

    ICommand * CreateCommand(TCPClient * client, char *data, int len)
    {
        Command * command = new RelayCommand(client);
        command->ParseCommand(data, len);
        return command;
    }

    void Execute()
    {
        Relay * relay = Relay::Instance;
        char * response = nullptr;
        if (!strcmp(_subcmd,"W")) //toggle
          {
            relay->ToggleRelay(atoi(args[0]));
            _client->write("OK");
          }
        else if (!strcmp(_subcmd,"R")) //state
          {
            int state = relay->RelayState(atoi(args[0]));
            char stateStr[30] = {0};
            sprintf(stateStr, "{\"state\":%d}", state);
            _client->write(stateStr);
          }
          else if (!strcmp(_subcmd, "S"))
          {
            //The schedule parsing doesn't quite work yet. 
          }
          else
          {
            _client->stop();
          }
    }
};
