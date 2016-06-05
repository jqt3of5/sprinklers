///=============================================================================================================
/// Written By: John Todd
/// Date: 20-5-2016
/// Description:
///  A RESTful server meant to act as a reverse proxy for the UNIX/INET socket GPIO service.
/// TODO:
///    1. Test the rudementary features(success)
///    2. Create a way for it to act as a multicast proxy - so it can connect to multiple GPIO services
////      on different RaspberryPis. Have to keep a record of available pis.
///    3. Create a "checkout" system. Where a user can checkout certain gpio pins, so that no other user
///       can interfere with another.
///    4. 
///=============================================================================================================

var express = require('express');
var app = express();
var fs = require('fs');
var net = require('net');
var gpio = require('./gpio');
var uuid = require('node-uuid');

app.use(express.static('public'));

//Allocate a new pin
/*app.put('/:host/gpio/:pin', function (req, res) {
    var host = req.params.host;
    var pin = parseInt(req.params.pin);
    var cmd = [0x00 << 6 | pin << 1 | 0];

    gpio.send(host, cmd, function(data, error){
        res.end();
    });
});

//unallocate a pin
app.delete('/:host/gpio/:pin', function(req, res) {
    res.end();
});
*/
//Get the value of a pin. 
app.get('/:deviceId/gpio/:pin', function(req, res) {
	var deviceId = req.params.deviceId;
	var pin = parseInt(req.params.pin);
	var cmd = "R " + pin;
	
	var deviceIp = device_infos[deviceId].ip;
	var deviceSocket = device_connections[deviceIp].socket;
	
	deviceSocket.on('data', function (data) {
		//var state = JSON.parse(data);
		res.end(data);
	});
	deviceSocket.write(cmd);
});

//Set the value of a pin to low
app.post('/:deviceId/gpio/:pin/clr', function(req, res) {
	var deviceId = req.params.deviceId;
    var pin = parseInt(req.params.pin);
    var cmd = "W " + pin + " 0";
	
	var deviceIp = device_infos[deviceId].ip;
	var deviceSocket = device_connections[deviceIp].socket;
	
	deviceSocket.write(cmd);
	res.end();
});

//set the value of a pin to high
app.post('/:deviceId/gpio/:pin/set', function(req, res) {
    var deviceId = req.params.deviceId;
    var pin = parseInt(req.params.pin);
    var cmd = "W " + pin + " 1";
	
	var deviceIp = device_infos[deviceId].ip;
	var deviceSocket = device_connections[deviceIp].socket;
	
	deviceSocket.write(cmd);
	res.end();
});

app.get('/devices', function(req, res) {
	res.end(JSON.stringify(device_infos));
});

var http_server = app.listen(8080, function(){
    console.log("Started listening");			
});

var device_connections = {};
var device_infos = {};

var socket_server = net.createServer(function (socket)
{
	socket.on('data', function (data)
	{
		//When the device first connects, it should send an object describing itself. name/type
		if (device_connections[socket.remoteAddress] == undefined)
		{
			var device_info = JSON.parse(data);
			device_connections[socket.remoteAddress] = {socket:socket, deviceId:device_info.deviceId};
			device_infos[device_info.deviceId] = {ip:socket.remoteAddress, type:device_info.type};	
		}
	});
	
	socket.on('end', function ()
	{
		//I don't know if this will work, because remoteAddress is supposed to be undefined when the client disconnects
		//var device = connected_devices[socket.remoteAddress];
		//connected_devices[socket.remoteAddress] = undefined;
		//device_infos[device.deviceId] = undefined;
		
	})
});

socket_server.listen(8081, function(){ console.log("Server Started")});


