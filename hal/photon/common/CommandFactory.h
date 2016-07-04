#include "Device.h"

struct Arg
{
  int len;
  char * arg;
};

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
        ~Command()
        {
          free(_data);
          free(_args);
        }
        virtual ICommand * ParseCommand(char * data, int len)
        {
            _data = new char[len+1]();
            memcpy(_data, data, len);

            int offset = ParseCommandPart(_data, len);

            Arg args[10];
            for(_argc = 0; offset < len && _argc < 10; _argc++)
            {
                 offset += ParseNextArg(_argc, _data + offset, len-offset, args[_argc]);
            }

            if (_argc > 0)
            {
              _args = new Arg[_argc];
              memcpy(_args, args, sizeof(Arg) * _argc);
            }

            return this;
        }
    protected:
    virtual int ParseCommandPart(char * data, int len)
    {
        _cmd = strtok(data, " ");
        _subcmd = strtok(nullptr, " ");
        return strlen(_cmd) + strlen(_subcmd) + 2;
    }
    virtual int ParseNextArg(int index, char * data, int len, Arg & part)
    {
      part.arg = strtok(nullptr, " ");
      part.len = strlen(part.arg) + 1;
      return part.len;
    }

    char * _data;
    TCPClient * _client;
    char * _cmd;
    char * _subcmd;
    Arg * _args;
    int _argc;
};

class DeviceIdentCommand : public Command
{
public:
    DeviceIdentCommand() {}
    DeviceIdentCommand(TCPClient * client) : Command(client) {}

    ICommand * CreateCommand(TCPClient * client)
    {
        return new DeviceIdentCommand(client);
    }

    void Execute()
    {
        char * ident = _device->Serialize();
        _client->write(ident);
        free(ident);
    }
    private :
    static Device * _device;
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
            ICommand * command = commandFactory.CreateCommand(client);
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
    static ICommandFactory  *_factories[];

};
