power = 1;


function toggle() {
    power = (power + 1) % 4;
    if (power == 1 || power == 3) {
        document.getElementById("power").innerHTML = "AUTO";
        document.getElementById("power").style = "color: #2fa4e7;";
    }
    else if (power == 0) {
        document.getElementById("power").innerHTML = "OFF";
        document.getElementById("power").style = "color: gray;";
    }
    else if (power == 2) {
        document.getElementById("power").innerHTML = "ON";
        document.getElementById("power").style = "color: rgb(255,200,0);";

    }
    Request.open('GET', `/api?d=window&m=toggle&a=${power}`, true);
    Request.send();
}

function offset(value) {
    Request.open('GET', `/api?d=window&m=offset&a=${value}`, true);
    Request.send();
}

onload = function () {
    Request = new XMLHttpRequest();
}