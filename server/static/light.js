isLight = false;


function toggle() {
    isLight = !isLight;
    if (isLight) {
        document.getElementById("Light").style = "color: rgb(255,200,0);"
        ctx.fillRect(0, 0, 300, 300);
    } else {
        document.getElementById("Light").style = "color: gray;"
        ctx.clearRect(0, 0, 300, 300);
    }
}

function offset(value) {
    if (value > 0) {
        document.getElementById("opt").innerHTML = `+${value}`;
    } else {
        document.getElementById("opt").innerHTML = `${value}`;
    }
}

onload = function () {
    ctx = document.getElementById("Canvas").getContext("2d");
    ctx.fillStyle = "rgb(255,200,0)";
}
