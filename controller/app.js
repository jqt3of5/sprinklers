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
var uuid = require('node-uuid');

DeviceTypeEnum ={
	Garage : "garage",
	Temp : "temp",
	Relay : "relay"
};

var endpoints = [{url:"/:deviceId/:pin",         method:"GET",  allowedDevices:[], createCommand:function(req) {return "R " + req.params.pin} },
		 {url:"/:deviceId/:pin/:value",  method:"POST", allowedDevices:[], createCommand:function(req) {return "W " + req.params.pin + " " + req.params.value} },
		 {url:"/:deviceId/schedule",     method:"GET",  allowedDevices:[DeviceTypeEnum.Relay],  createCommand:function(req) {return "S R"}},
		 {url:"/:deviceId/schedule",     method:"POST", allowedDevices:[DeviceTypeEnum.Relay],  createCommand:function(req) {return "S W" + req.body}, },
		 {url:"/:deviceId/temp/:probe",  method:"GET",  allowedDevices:[DeviceTypeEnum.Temp],   createCommand:function(req) {return "T " + req.params.probe}, },
		 {url:"/:deviceId/garage/door",  method:"GET",  allowedDevices:[DeviceTypeEnum.Garage], createCommand:function(req) {return "D S"}, },
		 {url:"/:deviceId/garage/door",  method:"POST", allowedDevices:[DeviceTypeEnum.Garage], createCommand:function(req) {return "D T"}, },
		 {url:"/:deviceId/garage/light", method:"POST", allowedDevices:[DeviceTypeEnum.Garage], createCommand:function(req) {return "L T"}, },
		 {url:"/:deviceId/garage/config", method:"POST",  allowedDevices:[DeviceTypeEnum.Garage], createCommand:function(req) {return "L O "+req.body}, },
];

app.use(express.static('public'));

app.get('/devices', function(req, res) {
	res.end(JSON.stringify(device_infos));
});

for (var i = 0; i < endpoints.length; ++i)
{
	if (endpoints[i].method == "GET")
	{
		app.get(endpoints[i].url,function (j) { return function(req, res) { handleHttpRequest(endpoints[j], req, res)}}(i) );
	}
	else if (endpoints[i].method == "POST")
	{
		app.post(endpoints[i].url, function (j) {return function(req, res) { handleHttpRequest(endpoints[j], req, res)}}(i) );
	}
}

function handleHttpRequest(endpoint, req, res)
{
	var deviceId = req.params.deviceId;
	if (device_infos[deviceId] == undefined)
	{
		res.status(404).end();
	}
	else if (endpoint.allowedDevices.length > 0 && endpoint.allowedDevices.indexOf(device_infos[deviceId].type) == -1)
	{
		res.status(405).end();
	}
	else
	{
		var deviceSocket = device_sockets[deviceId];
		//While I really really like this idea, I'm worried data will be lost
		deviceSocket.once('data', function (data) {
			res.end(data);
		});
		
		var cmd = endpoint.createCommand(req);
		deviceSocket.write(cmd);	
	}
}



var http_server = app.listen(8080, function(){
    console.log("Started listening");			
});

var device_infos = {};
var device_sockets = {};
var socket_server = net.createServer(function (socket)
{
	socket.once('data', function (data)
	{
		var device_info = JSON.parse(data);
		//When the device first connects, it should send an object describing itself. name/type
		socket.deviceId = device_info.deviceId;
		device_infos[device_info.deviceId] = {type:device_info.type, deviceId:device_info.deviceId};
		device_sockets[device_info.deviceId] = socket;
	});
	
	socket.on('close', function ()
	{
		device_infos[socket.deviceId] = undefined;
	});
});

socket_server.listen(8081, function(){ console.log("Server Started")});


