#include "Device.h"

class ICommand
{
    public:
    virtual void Execute();
};

class ICommandFactory
{
    public:
    virtual ICommand * CreateCommand(TCPClient * client, char *data, int len);
    virtual bool IsCommandPrefix(char * prefix);
};

class Command : public ICommand
{
    public:
        Command() {}
        Command(TCPClient * client)
        {
          _client = client;
        }
        virtual ICommand * ParseCommand(char * data, int len)
        {
            _cmd = strtok(data, " ");
            _subcmd = strtok(nullptr, " ");

            _argc = 0;
            _args[0] = strtok(nullptr, " ");
            for(_argc = 0; _args[_argc] != nullptr; _argc++)
            {
                 _args[_argc+1] = strtok(nullptr, " ");
            }
            return this;
        }
    protected:
    TCPClient * _client;
    char * _cmd;
    char * _subcmd;
    char * _args[10];
    int _argc;
};

class DeviceIdentCommand : public Command
{
public:
    DeviceIdentCommand() {}
    DeviceIdentCommand(TCPClient * client) : Command(client) {}

    ICommand * CreateCommand(TCPClient * client, Device * device)
    {
        DeviceIdentCommand * command = new DeviceIdentCommand(client);
        command->_device = device;
        return command;
    }

    void Execute()
    {
        char * ident = _device->Serialize();
        _client->println(ident);
    }
    private :
    Device * _device;
};
/*class RelayCommand : public Command, public ICommandFactory
{
    public:
    char * GetCommandPrefix()
    {
        char * cmdPrefix = new char[2]{'R'};
        return cmdPrefix;
    }
    ICommand * CreateCommand(TCPClient * client, char *data, int len)
    {
        Command * command = new GarageDoorCommand();
        command->client = client;
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
}*/

class NullCommand : public Command
{
    void Execute()
    {

    }
};

class CommandFactory
{
    public:
    //static CommandFactory * Instance = new CommandFactory();
    //CommandFactory(){}
    ICommand * ParseCommand(TCPClient * client, char * data, int len)
    {
        if (data == nullptr || len == 0)
        {
            DeviceIdentCommand * cmd = new DeviceIdentCommand();
            return cmd->CreateCommand(client,CommandFactory::_device);
        }

        char * cmd = strtok(data, " ");

        int i = 0;
        ICommandFactory * factory = _factories[i];
        while (factory != nullptr)
        {
            if (factory->IsCommandPrefix(cmd))
            {
                return factory->CreateCommand(client, data, len);
            }
            i++;
            factory = _factories[i];
        }
        return new NullCommand();
    }
    private:
    static Device * _device;
    static ICommandFactory  *_factories[];

};
