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
var fs = require('fs');
var net = require('net');
var uuid = require('node-uuid');

DeviceTypeEnum ={
	Garage : "garage",
	Temp : "temp",
	Relay : "relay"
};

var endpoints = [{url:"/:deviceId/relay/schedule",     method:"GET",  allowedDevices:[DeviceTypeEnum.Relay],  createCommand:function(params, body) {return "S R"}},
		 {url:"/:deviceId/relay/schedule",     method:"POST", allowedDevices:[DeviceTypeEnum.Relay],  createCommand:function(params, body) {return "S W" + body}, },
		 {url:"/:deviceId/garage/door",  method:"GET",  allowedDevices:[DeviceTypeEnum.Garage], createCommand:function(params, body) {return "D S"}, },
		 {url:"/:deviceId/garage/door",  method:"POST", allowedDevices:[DeviceTypeEnum.Garage], createCommand:function(params, body) {return "D T"}, },
		 {url:"/:deviceId/garage/light", method:"POST", allowedDevices:[DeviceTypeEnum.Garage], createCommand:function(params, body) {return "L T"}, },
		 {url:"/:deviceId/garage/light", method:"GET", allowedDevices:[DeviceTypeEnum.Garage], createCommand:function(params, body) {return "L S"}, },
		 {url:"/:deviceId/garage/config", method:"POST",  allowedDevices:[DeviceTypeEnum.Garage], createCommand:function(params, body) {return "L O "+body}, },
		 {url:"/:deviceId/temp/:probe",  method:"GET",  allowedDevices:[DeviceTypeEnum.Temp],   createCommand:function(params, body) {return "T " + params.probe}, },
		 {url:"/:deviceId/gpio/:pin",         method:"GET",  allowedDevices:[], createCommand:function(params, body) {return "R " + params.pin} },
		 {url:"/:deviceId/gpio/:pin/:value",  method:"POST", allowedDevices:[], createCommand:function(params, body) {return "W " + params.pin + " " + params.value} },
];

var app = express();
//Immediately Invoked Function Expression. Sigh :(
var iife = function (endpoint) { return function(req, res) { handleHttpRequest(endpoint, req, res)}};
for (var i = 0; i < endpoints.length; ++i)
{
	if (endpoints[i].method == "GET")
	{
		app.get(endpoints[i].url, iife(endpoints[i]));
	}
	else if (endpoints[i].method == "POST")
	{
		app.post(endpoints[i].url, iife(endpoints[i]));
	}
	console.log("Creating endpoing for: " + endpoints[i].url);
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
			console.log("Received response from photon")
			res.end(data);
		});
		
		var cmd = endpoint.createCommand(req.params, req.body);
		console.log("Executing command: " + cmd);
		console.log("\nOn device with ID: " + deviceId)
		deviceSocket.write(cmd);	
	}
}

app.use(express.static('public'));

app.post('/:deviceId/name/:name', function(req, res) {
	var deviceId = req.params.deviceId;
	var name = req.params.deviceId;
	
	if (device_info[name] != undefined)
	{
		res.status(405).end();
		return;
	}
	//When we assign a name to a device, add the same object under a different key
	device_infos[deviceId].name = name;
	device_infos[name] = device_infos[deviceId];
	res.end();
});
app.get('/devices', function(req, res) {
	res.end(JSON.stringify(device_infos));
});

var http_server = app.listen(8080, function(){
    console.log("Http Server Started");			
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
		device_infos[device_info.deviceId] = {type:device_info.type, deviceId:device_info.deviceId, name:undefined};
		device_sockets[device_info.deviceId] = socket;
		console.log("Device connected: " + device_info.deviceId);
	});
	
	socket.on('close', function ()
	{
		var deviceInfo = device_infos[socket.deviceId];
		device_infos[socket.deviceId] = undefined;
		device_infos[deviceInfo.name] = undefined;
	});
});

socket_server.listen(8081, function(){ console.log("Socket Server Started")});


