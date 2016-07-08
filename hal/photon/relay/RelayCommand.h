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
    ICommand * ParseCommand(char *data, int len)
    {
        _cmd = strtok(data, " ");
        _subcmd = strtok(nullptr, " ");

        if (!strcmp(subcmd, "S"))
        {
            int cmdPrefixLen = strlen(_cmd) + strlen(_subcmd) + 2;
            if (cmdPrefixLen < len)
            {
                _schedule = new char[len-cmdPrefixLen]();
                memcpy(_schedule, data + cmdPrefixLen, len-cmdPrefixLen);
            }
        }
        else
        {
            return Command::ParseCommand(data, len);
        }
    }
    void Execute()
    {

    }

    private:
    char * _schedule;
}