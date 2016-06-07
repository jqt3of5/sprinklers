///=============================================================================================================
/// Written By: John Todd
/// Date: 20-5-2016
/// Description:
///  A RESTful server meant to act as a reverse proxy for the UNIX/INET socket GPIO service.
/// TODO:
///    1. Test the rudementary features
///    3. Create a "checkout" system. Where a user can checkout certain gpio pins, so that no other user
///       can interfere with another.
///    4. Added Security
///=============================================================================================================

var express = require('express');
var app = express();
var fs = require('fs');
var net = require('net');
var gpio = require('./gpio');
var uuid = require('node-uuid');

DeviceTypeEnum ={
	Garage : "garage",
	Temp : "temp",
	Relay : "relay"
};


app.use(express.static('public'));

//Get the value of a pin. 
app.get('/:deviceId/:pin', function(req, res) {
	var deviceId = req.params.deviceId;
	var pin = parseInt(req.params.pin);
	var cmd = "R " + pin;
	
	if (device_infos[deviceId] == undefined)
	{
		res.status(404).end();
	}
	else
	{
		queryDevice(cmd, res, function (data) {
			res.end(data);
		});	
	}
});

//Set the value of a pin
app.post('/:deviceId/:pin', function(req, res) {
	var deviceId = req.params.deviceId;
    	var pin = parseInt(req.params.pin);
    	var cmd = "W " + pin + " " + req.body;
    	
	if (device_infos[deviceId] == undefined)
	{
		res.status(404).end();	
	}
	else
	{
		queryDevice(cmd, res, function (data) {
			res.end(data);
		});	
	}
});

app.get ('/:deviceId/schedule', function(req, res){
	var deviceId = req.params.deviceId;
    	var pin = parseInt(req.params.pin);
    	var cmd = "S R";
    	
    	if (device_infos[deviceId] == undefined)
    	{
    		res.status(404).end();
    	}
    	else if (device_infos[deviceId].type != DeviceTypeEnum.Relay)
    	{
    		res.status(405).end();	
    	}
    	else 
	{
		queryDevice(cmd, res, function (data) {
			res.end(data);
		});	
	}
});
app.post('/:deviceId/schedule', function(req, res){
	var deviceId = req.params.deviceId;
    	var pin = parseInt(req.params.pin);
    	var cmd = "S W" + req.body;
    	
    	if (device_infos[deviceId] == undefined)
    	{
    		res.status(404).end();
    	}
    	else if (device_infos[deviceId].type != DeviceTypeEnum.Relay)
    	{
    		res.status(405).end();	
    	}
    	else 
	{
		queryDevice(cmd, res, function (data) {
			res.end(data);
		});
	}
});

app.get('/:deviceId/temp/:probe', function(req, res) {
	var deviceId = req.params.deviceId;
	var probe = parseInt(req.params.probe);
	var cmd = "T " + probe;
	
	if (device_infos[deviceId] == undefined)
	{
		res.status(404).end();
	}
	else if (device_infos[deviceId].type != DeviceTypeEnum.Temp)
	{
		res.status(405).end();
	} 
	else 
	{
	queryDevice(cmd, res, function (data) {
			res.end(data);
		});		
	}
});

app.get('/:deviceId/garage/door', function(req, res) {
	var deviceId = req.params.deviceId;
	//door, state
	var cmd = "D S";
	
	if (device_infos[deviceId] == undefined)
	{
		res.status(404).end();
	}
	else if (device_infos[deviceId].type != DeviceTypeEnum.Garage)
	{
		res.status(405).end();
	} 
	else
	{
	queryDevice(cmd, res, function (data) {
			res.end(data);
		});		
	}
});

app.get('/:deviceId/garage/light', function(req, res) {
	var deviceId = req.params.deviceId;
	//Light, State
	var cmd = "L S";
	
	if (device_infos[deviceId] == undefined)
	{
		res.status(404).end();
	}
	else if (device_infos[deviceId].type != DeviceTypeEnum.Garage)
	{
		res.status(405).end();
	} 
	else
	{
		queryDevice(cmd, res, function (data) {
			res.end(data);
		});	
	}
});

app.post('/:deviceId/garage/light', function(req, res) {
	var deviceId = req.params.deviceId;
	//Light, Toggle
	var cmd = "L T";
	
	if (device_infos[deviceId] == undefined)
	{
		res.status(404).end();
	}
	else if (device_infos[deviceId].type != DeviceTypeEnum.Garage)
	{
		res.status(405).end();
	} 
	else
	{
		queryDevice(cmd, res, function (data) {
			res.end(data);
		});	
	}
});

app.post('/:deviceId/garage/door', function(req, res) {
	var deviceId = req.params.deviceId;
	//Door, Toggle
	var cmd = "D T";
	
	if (device_infos[deviceId] == undefined)
	{
		res.status(404).end();
	}
	else if (device_infos[deviceId].type != DeviceTypeEnum.Garage)
	{
		res.status(405).end();
	} 
	else
	{
		queryDevice(cmd, res, function (data) {
			res.end(data);
		});
	}
});

app.get('/devices', function(req, res) {
	res.end(JSON.stringify(device_infos));
});

function queryDevice(cmd, deviceId, onComplete)
{
	var deviceSocket = device_infos[deviceId].socket;
	//While I really really like this idea, I'm worried data will be lost
	deviceSocket.once('data', function (data) {
		onComplete(data);
	});
	deviceSocket.write(cmd);	
}

app.configure(function ()
{
	app.use(express.bodyParser());	
});

var http_server = app.listen(8080, function(){
    console.log("Started listening");			
});

var device_infos = {};
var socket_server = net.createServer(function (socket)
{
	socket.once('data', function (data)
	{
		var device_info = JSON.parse(data);
		//When the device first connects, it should send an object describing itself. name/type
		socked.deviceId = device_info.deviceId;
		device_infos[device_info.deviceId] = {socket:socket, type:device_info.type};	
		
		socket.removeListener('data', configDevice);
	});
	
	socket.on('close', function ()
	{
		device_infos[socket.deviceId] = undefined;
	});
});

socket_server.listen(8081, function(){ console.log("Server Started")});


