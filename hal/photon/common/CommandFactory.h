class ICommand
{
    public:
    virtual void Execute();
}

class ICommandFactory
{
    public:
    virtual ICommand * CreateCommand(TCPClient * client, char *data, int len);
    virtual char * GetCommandPrefix();
}

class Command : public ICommand
{
    public:
        virtual ICommand * ParseCommand(char * data, int len)
        {
            _cmd = strtok(data, " ");
            _subCmd = strtok(nullptr, " ");
  
            _argc = 0;
            _args[0] = strtok(nullptr, " ");
            for(_argc = 0; _args[_argc] != null; _argc++)
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
}

class GarageDoorCommand : public Command, public ICommandFactory
{
    public:
    char * GetCommandPrefix()
    {
        char * cmdPrefix = new char[2]{'D'};
        return cmdPrefix;
    }
    ICommand * CreateCommand(char *data, int len)
    {
        Command * command = new GarageDoorCommand();
        command->_client = client;
        command->ParseCommand(data, len);
        return command;
    }
   
    void Execute()
    {
        Garage * garage = new Garage();
        char * response = nullptr;
        if (!strcmp(_subCmd,"T")) //toggle
          {
            garage->DoorToggle();
          }
        else if (!strcmp(_subCmd,"S")) //state
          {
            int state = garage->DoorState();
          }
          free(garage);
    }
}

class GarageLightCommand : public Command, public ICommandFactory
{
    public:
    char * GetCommandPrefix()
    {
        char * cmdPrefix = new char[2]{'L'};
        return cmdPrefix;
    }
     ICommand * CreateCommand(TCPClient * client, char *data, int len)
    {
        Command * command = new GarageDoorCommand();
        command->_client = client;
        command->ParseCommand(data, len);
        return command;
    }
    
    void Execute()
    {
        Garage * garage = new Garage();
        char * response = nullptr;
        if (!strcmp(_subCmd,"T")) //toggle
          {
            garage->LightToggle();
          }
        else if (!strcmp(_subCmd,"S")) //state
          {
            int state = garage->LightState();
          }
        else if (!strcmp(_subCmd,"O")) //Update Light time out
          {
            garage->UpdateTimeout(atoi(args[0]));
          }
          free(garage);
    }
}

class DeviceIdentCommand : public Command
{
    ICommand * CreateCommand(TCPClient * client, Device * device)
    {
        DeviceIdentCommand * command = new DeviceIdentCommand();
        command->_client = client;
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
}
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
}

class CommandFactory
{
    public:
    static CommandFactory * Instance = new CommandFactory();
    
    ICommand * ParseCommand(TCPClient * client, char * data, int len)
    {
        if (data == nullptr || len = 0)
        {
            return new DeviceIdentCommand()->CreateCommand(client,new Garage());
        }
        
        char * cmd = strtok(data, " ");
        
        int i = 0;
        ICommandFactory * factory = _factories[i];
        while (factory != nullptr)
        {
            if (!strcmp(factory->GetCommandPrefix(), cmd))
            {
                return factory->CreateCommand(client, data, len);
            }
            i++;
            factory = _factories[i];
        }
        return new NullCommand();
    }
    private:
    static ICommandFactory * _factories = new ICommandFactory[]{new GarageDoorCommand(), new GarageLightCommand(), nullptr};
    CommandFactory();
}