var device_buttons = [];

window.onload = function()
{
    device_buttons = {"temp":document.getElementById("thermometer-button"), "garage": document.getElementById("garage-button"), "relay": document.getElementById("sprinkler-button")};
    refreshDevices();
    window.setInterval(refreshDevices, 5000);
}
var devices = {};
function refreshDevices()
{
	$.ajax({
		url: "/devices",
		type:"GET",
		dataType:"json"
	})
	.done (function(json){
		var devs = JSON.parse(res);
		for (var deviceId in devs)
		{
			var device = devs[deviceId];
			devices[device.type] = deviceId;
			device_buttons[device.type].disabled = false;
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
    var deviceId = devices["garage"];
	$.ajax({
		url: "/" + deviceId + "/garage/door",
		type:"GET",
		dataType:"json"
	})
	.done (function(json){
		var doorButton = document.getElementById("doorButton");
    		if (res == "1")
	    	{
	    		doorButton.innerHTML = "Open";
	    	}
	    	else if (res == "0")
	    	{
	    		doorButton.innerHTML = "Closed";
	    	}
	})
	.fail(function( xhr, status, errorThrown ) {
		
	}).always(function() {
		$.ajax({
			url: "/" + deviceId + "/garage/light",
			type:"GET",
			dataType:"json"
		})
		.done (function(json){
			var doorButton = document.getElementById("lightButton");
	    		if (res == "1")
		    	{
		    		doorButton.innerHTML = "On";
		    	}
		    	else if (res == "0")
		    	{
		    		doorButton.innerHTML = "Off";
		    	}
		})
		.fail(function( xhr, status, errorThrown ) {
			
		});
	});
}

function tempClick()
{
    var list = document.getElementById("device-list");
    var temp = document.getElementById("thermometer");

    list.style.display = "none";
    temp.style.display = "block";

    refreshTempuratures();
	window.setInterval(refreshTempuratures, 2000);
}

function garageClick()
{
    var list = document.getElementById("device-list");
    var garage = document.getElementById("garage");

    list.style.display = "none";
    garage.style.display = "block";

    refreshGarage();
	window.setInterval(refreshGarage, 2000);
}

function sprinklerClick()
{
}

function toggleGarageClick()
{
	var deviceId = devices["garage"];
	$.ajax({
		url: "/" + deviceId + "/garage/door",
		type:"POST",
		dataType:"json"
	});
}

function toggleLightClick()
{
	var deviceId = devices["garage"];
	$.ajax({
		url: "/" + deviceId + "/garage/light",
		type:"POST",
		dataType:"json"
	});
}

