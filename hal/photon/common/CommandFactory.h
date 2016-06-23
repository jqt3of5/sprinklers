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
            int offset = 0;
            ParseCommandPart(data, len);
            offset += strlen(_cmd) + 1;
            offset += strlen(_subcmd) + 1;
            
            _argc = 0;
            _args[0] = ParseNextArg(0, data + _offset, len-_offset);
            offset += strlen(_args[0]) + 1;
            for(_argc = 0; _args[_argc] != nullptr; _argc++)
            {
                 _args[_argc+1] = ParseNextArg(_argc+1, data + _offset, len-_offset);
                 offset += strlen(_args[_argc+1]) + 1;
            }
            return this;
        }
    protected:
    virtual void ParseCommandPart(char * data, int len)
    {
        _cmd = strtok(data, " ");
        _subcmd = strtok(nullptr, " ");
    }
    virtual char * ParseNextArg(int index, char * data, int len)
    {
        return strtok(nullptr, " ");
    }
    
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
        free(ident);
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
    static ICommand * ParseCommand(TCPClient * client, char * data, int len)
    {
        if (data == nullptr || len == 0)
        {
            DeviceIdentCommand commandFactory;
            ICommand * command = commandFactory.CreateCommand(client,CommandFactory::_device);
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
