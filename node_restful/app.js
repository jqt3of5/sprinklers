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

app.use(express.static('public'));

//Allocate a new pin
app.put('/gpio/:pin', function (req, res) {
    var pin = parseInt(req.params.pin);
    var cmd = [0x00 << 6 | pin << 1 | 0];

    gpio.send(cmd, function(data){
        res.end();
    });
});

//unallocate a pin
app.delete('/gpio/:pin', function(req, res) {
    res.end();
});

//Get the value of a pin. 
app.get('/gpio/:pin', function(req, res) {
    var pin = parseInt(req.params.pin);
    var cmd = [0x01 << 6 | pin << 1];

    gpio.send(cmd, function (data) {
	res.end(data);
    });
});

//Set the value of a pin to low
app.post('/gpio/:pin/clr', function(req, res) {
    var pin = parseInt(req.params.pin);
    var cmd = [0x02 << 6 | pin << 1 | 0];

    gpio.send(cmd, function (data) {
	res.end(data);
    });
});

//set the value of a pin to high
app.post('/gpio/:pin/set', function(req, res) {
    var pin = parseInt(req.params.pin);
    var cmd = [0x02 << 6 | pin << 1 | 1];

    gpio.send(cmd, function (data) {
	res.end(data);
    });
});

app.post('/gpio/:pin/pulse', function(req, res) {
    var pin = parseInt(req.params.pin);
    var cmd_on = [0x02 << 6 | pin << 1 | 0];
    var cmd_off = [0x02 << 6 | pin << 1 | 1];

    gpio.send(cmd_on, function(data) {
	setTimeout(function() {
		gpio.send(cmd_off, function(data) {
		    res.end();
	        });

	}, 500);
    });
});

var server = app.listen(8080, function(){

    console.log("Started listening");
			    
});
