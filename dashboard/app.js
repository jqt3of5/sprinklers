var device_buttons = [];
$(function() {
	configDevicePage();
	configGaragePage();
	configSprinklerPage();
});
function configDevicePage()
{
	device_buttons = {
		"temp":$("#thermometer-button"), 
		"garage": $("#garage-button"), 
		"relay": $("#sprinkler-button")
	};
	
	device_buttons["temp"].click(tempClick);
	device_buttons["temp"].disabled = true;
	
	device_buttons["garage"].click(garageClick);
	device_buttons["garage"].disabled = true;
	
	device_buttons["relay"].click(sprinklerClick);
	device_buttons["relay"].disabled = true;
	
	refreshDevices();
	window.setInterval(refreshDevices, 5000);
}
function tempClick()
{
    var list = $("#device-list");
    var temp = $("#thermometer");

    list.css("display", "none");
    temp.css("display", "block");
}
function garageClick()
{
    var list = $("#device-list");
    var garage = $("#garage");

    list.css("display", "none");
    garage.css("display", "block");
}
function sprinklerClick()
{
}

function configGaragePage()
{

	
	var lightButton = $("#light-button")
	    .click(function() {
		var deviceId = devices["garage"];
		$.ajax({
		
			url: "/" + deviceId + "/garage/light",
			type:"POST",
			dataType:"json"
		}).done(function(){
			
		});
	});
	
	var lightButton = $("#door-button")
	    .click(function() {
		var deviceId = devices["garage"];
		$.ajax({
			url: "/" + deviceId + "/garage/door",
			type:"POST",
			dataType:"json"
		}).done(function(){
			
		});
	});
}

function configSprinklerPage()
{
	
}

var devices = {};
function refreshDevices()
{
	$.ajax({
		url: "/devices",
		type:"GET",
		dataType:"json"
	})
	.done (function(devs){
		//var devs = JSON.parse(json);
		for (var deviceId in devs)
		{
			var device = devs[deviceId];
			devices[device.type] = deviceId;
			device_buttons[device.type].disabled = false;
		}
		if (devices["temp"] != undefined)
		{
			refreshTempuratures();
		}
		if (devices["garage"] != undefined)
		{
			refreshGarage();
		}
		if (devices["relay"] != undefined)
		{
			
		}
	})
	.fail(function( xhr, status, errorThrown ) {
		
	});
}

function refreshTempuratures()
{
    //It's possible this device has disconnected, kick back to devices screen
    var deviceId = devices["temp"];
    	$.ajax({
		url: "/" + deviceId + "/temp",
		type:"GET",
		dataType:"json"
	})
	.done (function(json){
		
	})
	.fail(function( xhr, status, errorThrown ) {
		
	});
}

function refreshGarage()
{
    //It's possible this device has disconnected, kick back to devices screen
  	refreshGarageDoor().done(refreshGarageLight).fail(function( xhr, status, errorThrown ) {
			
		});
}

function refreshGarageLight()
{
	var deviceId = devices["garage"];
	return $.ajax({
			url: "/" + deviceId + "/garage/door",
			type:"GET",
			dataType:"json"
		})
		.done (function(json){
			var doorButton = $("#door-button");
	    	    if (json.state)
		    	{
		    	    doorButton.text("Open");
		    	}
		    else 
		    	{
		    	    doorButton.text( "Closed");
		    	}
		});
}

function refreshGarageDoor()
{
    	var deviceId = devices["garage"];
	return $.ajax({
			url: "/" + deviceId + "/garage/light",
			type:"GET",
			dataType:"json"
		})
		.done (function(json){
			var doorButton = $("#light-button");
	    	    if (json.state)
		    	{
		    	    doorButton.text("On");;
		    	}
		    else
		    	{
		    	    doorButton.text("Off");
		    	}
		});
}




