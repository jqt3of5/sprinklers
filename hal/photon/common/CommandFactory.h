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
        _client->write(ident);
    }
    private :
    Device * _device;
};

class NullCommand : public Command
{
public:
    NullCommand(TCPClient * client) : Command(client) {}
    void Execute()
    {
        _client->stop();
    }
};

class CommandFactory
{
    public:
    ICommand * ParseCommand(TCPClient * client, char * data, int len)
    {
        if (data == nullptr || len == 0)
        {
            DeviceIdentCommand * commandFactory = new DeviceIdentCommand();
            ICommand * command = commandFactory->CreateCommand(client,CommandFactory::_device);
            free(commandFactory);
            return command;
        }

        char cmd[15] = {0};
        sscanf(data, "%s", cmd);

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
        return new NullCommand(client);
    }
    private:
    static Device * _device;
    static ICommandFactory  *_factories[];

};
