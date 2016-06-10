var device_buttons = [];

window.onload = function()
{
    device_buttons = {"temp":document.getElementById("thermometer-button"), "garage": document.getElementById("garage-button"), "relay": document.getElementById("sprinkler-button")};
    refreshDevices();
    window.setInterval(refreshDevices, 1000);
}
var devices = {};
function refreshDevices()
{
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (xhttp.readyState == 4 && xhttp.status == 200) 
		{
			var devs = JSON.parse(xhttp.responseText);
			for (var deviceId in devs)
			{
				var device = devs[deviceId];
				devices[device.type] = deviceId;
				device_buttons[device.type].disabled = false;
			}
		}
	};
	xhttp.open("GET", "/devices", true);
	xhttp.send();
}

function refreshTempuratures()
{
    //It's possible this device has disconnected, kick back to devices screen

    //Search through devices for temp device
}

function refreshGarage()
{
    //It's possible this device has disconnected, kick back to devices screen
    var deviceId = devices["garage"];
	var doorRequest = new XMLHttpRequest();
	doorRequest.onreadystatechange = function() {
		if (doorRequest.readyState == 4 && doorRequest.status == 200) 
		{
		}
	};
	doorRequest.open("GET", "/" + deviceId + "/garage/door", true);
	doorRequest.send();
	
	var lightRequest = new XMLHttpRequest();
	lightRequest.onreadystatechange = function() {
		if (lightRequest.readyState == 4 && lightRequest.status == 200) 
		{
		}
	};
	lightRequest.open("GET", "/" + deviceId + "/garage/light", true);
	lightRequest.send();
}

function tempClick()
{
    var list = document.getElementById("device-list");
    var temp = document.getElementById("thermometer");

    list.style.display = "none";
    temp.style.display = "block";

    window.setInterval(refreshTempuratures, 2000);
    refreshTempuratures();
}

function garageClick()
{
    var list = document.getElementById("device-list");
    var garage = document.getElementById("garage");

    list.style.display = "none";
    garage.style.display = "block";

    window.setInterval(refreshGarage, 500);
    refreshGarage();
}

function toggleGarageClick()
{
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (xhttp.readyState == 4 && xhttp.status == 200) 
		{
		}
	};
	xhttp.open("POST", "/:deviceId/garage/door", true);
	xhttp.send();
}

function toggleLightClick()
{
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (xhttp.readyState == 4 && xhttp.status == 200) 
		{
		}
	};
	xhttp.open("POST", "/:deviceId/garage/light", true);
	xhttp.send();
}

function sprinklerClick()
{
}
