var device_buttons = [];

window.onload = function()
{
    device_buttons = [document.getElementById("thermometer-button"),document.getElementById("garage-button"), document.getElementById("sprinkler-button")];
    refreshDevices();
    window.setInterval(refreshDevices, 1000);
}


var devices = [];

function refreshDevices()
{
    //Hit the endpoint
    //Update the global variable
    //Update the UI
    for(var i = 0; i < device_buttons.length; ++i)
    {
	device_buttons[i].disabled = true;
    }
}

function refreshTempuratures()
{
    //It's possible this device has disconnected, kick back to devices screen

    //Search through devices for temp device
}

function refreshGarage()
{
    //It's possible this device has disconnected, kick back to devices screen
    
    //Search through devices for garage device
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
}

function toggleLightClick()
{
}

function sprinklerClick()
{
}
