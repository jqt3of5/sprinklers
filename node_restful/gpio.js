var net = require('net');

function gpio_send(commands, completion)
{
    var client = net.connect({path:"/tmp/gpio.sock"}, function () {
        //write number of bytes
        client.write(new Buffer([commands.length]));
        //Then write that number of bytes
        client.write(new Buffer(commands));
    });

    var bytesRecieved = new Buffer(0);
    client.on('data', function (data) {
        bytesRecieved = Buffer.concat([bytesRecieved,data]);
        console.log(data);
        //Then read that number of bytes
        if (bytesRecieved.length >= commands.length)
        {
            completion(bytesRecieved);
            //End the connection
            client.write(new Buffer([0]));
            client.end();
        }
    });

    client.on('error', function(error){
        completion(error.toString() + '\n');
    });

    client.on('end', function () {
    });
}

exports.send = gpio_send;
