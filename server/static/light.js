power = false;


function toggle() {
    power = !power;
    if (power) {
        document.getElementById("Light").style = "color: rgb(255,200,0);"
        ctx.fillRect(0, 0, 300, 300);
        Request.open('GET', `/api?d=light&m=toggle&a=1`, true);
        Request.send();
    } else {
        document.getElementById("Light").style = "color: gray;"
        ctx.clearRect(0, 0, 300, 300);
        Request.open('GET', `/api?d=light&m=toggle&a=0`, true);
        Request.send();
    }
}

function offset(value) {
    Request.open('GET', `/api?d=light&m=offset&a=${value}`, true);
    Request.send();
}

onload = function () {
    ctx = document.getElementById("Canvas").getContext("2d");
    ctx.fillStyle = "rgb(255,200,0)";
    Request = new XMLHttpRequest();
}