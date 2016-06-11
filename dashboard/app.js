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
	hitEndPoint("GET", "/devices",function(res) {
			var devs = JSON.parse(res);
			for (var deviceId in devs)
			{
				var device = devs[deviceId];
				devices[device.type] = deviceId;
				device_buttons[device.type].disabled = false;
			}
	});
}

function refreshTempuratures()
{
    //It's possible this device has disconnected, kick back to devices screen
    var deviceId = devices["temp"];
    hitEndPoint("GET", "/"+deviceId+"/temp", function (res) {
    	//Should return connected probe numbers 
    	//loop through them and get their tempuratures
    });
}

function refreshGarage()
{
    //It's possible this device has disconnected, kick back to devices screen
    var deviceId = devices["garage"];
    hitEndPoint("GET", "/"+deviceId+"/garage/door", function (res) {
    	var doorButton = document.getElementById("doorButton");
    	if (res == "1")
    	{
    		doorButton.innerHTML = "On";
    	}
    	else if (res == "0")
    	{
    		doorButton.innerHTML = "Off";
    	}
    	
    });
    hitEndPoint("GET", "/"+deviceId+"/garage/light", function (res) {
    	var lightButton = document.getElementById("garageButton");
    	if (res == "1")
    	{
    		lightButton.innerHTML = "On";
    	}
    	else if (res == "0")
    	{
    		lightButton.innerHTML = "Off";
    	}
    });
}

function tempClick()
{
    var list = document.getElementById("device-list");
    var temp = document.getElementById("thermometer");

    list.style.display = "none";
    temp.style.display = "block";

    //window.setInterval(refreshTempuratures, 2000);
    refreshTempuratures();
}

function garageClick()
{
    var list = document.getElementById("device-list");
    var garage = document.getElementById("garage");

    list.style.display = "none";
    garage.style.display = "block";

   // window.setInterval(refreshGarage, 2000);
    refreshGarage();
}

function sprinklerClick()
{
}

function toggleGarageClick()
{
	var deviceId = devices["garage"];
	hitEndPoint("POST", "/"+deviceId+"/garage/door", undefined);
}

function toggleLightClick()
{
	var deviceId = devices["garage"];
	hitEndPoint("POST", "/"+deviceId+"/garage/light", undefined);
}

function hitEndPoint(method, url, onComplete)
{
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (xhttp.readyState == 4 && xhttp.status == 200) 
		{
			//Does not handle errors!!!
			if (onComplete != undefined)
			{
				onComplete(xhttp.responseText);	
			}
			
		}
	};
	xhttp.open(method, url, true);
	xhttp.send();
}

