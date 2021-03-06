/*#include "Relay.h"

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

    virtual int ParseNextArg(int index, char * data, int len, Arg & part)
    {
      if (!strcmp(_subcmd, "S"))
      {
        part.arg = data;
        part.len = len;
        return len;
      }
      return Command::ParseNextArg(index, data, len, part);
    }

    void Execute()
    {
        Relay * relay = Relay::Instance;
        char * response = nullptr;
        if (!strcmp(_subcmd,"W")) //toggle
          {
            relay->ToggleRelay(atoi(args[0].arg));
            _client->write("OK");
          }
        else if (!strcmp(_subcmd,"R")) //state
          {
            int state = relay->RelayState(atoi(args[0].arg));
            char stateStr[30] = {0};
            sprintf(stateStr, "{\"state\":%d, \"mem\":%d}", state, System.freeMemory());
            _client->write(stateStr);
          }
          else if (!strcmp(_subcmd, "S"))
          {
            if (_argc == 0)
            {
              relay->ReadSchedule();
            }
            else
            {
              relay->SaveSchedule(_args[0].arg, _args[0].len));
            }
          }
          else
          {
            _client->stop();
          }
    }
};
*/
