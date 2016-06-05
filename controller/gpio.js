var net = require('net');
var fs = require('fs');

/*var hosts = JSON.parse(fs.readFileSync("hosts.json", "utf8"));
function extractHostConfig(name)
{
    for each (var host in hosts)
    {
	if (host.name === name)
	{
	    if (host.type =="unix")
  	    {
		return unix(host);
 	    }
	    else if (host.type =="tcp")
  	    {
		return tcp(host);
 	    }
	    else if (host.type =="udp")
  	    {
		return udp(host);
	    }
	}
    }
}

function unix(host)
{
	return {path:host.path};
}

function tcp(host)
{
	return {address:host.address, port:host.port}
}

function udp(host)
{
}
*/
function send(name, commands, completion)
{		
    var config = extractHostConfig(name);

    var client = net.connect(config, function () {
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
            completion(bytesRecieved, null);
            //End the connection by writing a 0x00
            client.write(new Buffer([0]));
            client.end();
        }
    });

    client.on('error', function(error){
        completion(null, error.toString());
    });

    client.on('end', function () {
    });
}

exports.send = send;
	
