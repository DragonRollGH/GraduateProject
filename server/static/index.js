isLight = false;


function adjBrightness(value) {
    document.getElementById("optBrightness").innerHTML = `${value}%&ensp;`;
    Request.open('GET', `/adjBrightness?v=${value}`, true);
    Request.send();
    Request.onreadystatechange = function () {
        if (Request.readyState == 4 && Request.status == 200) {
            var response = Request.responseText;
            drawBrightness(isLight ? Number(response) : 0)
        }
    };
}

function adjLight(value) {
    document.getElementById("optLight").innerHTML = `${value}%&ensp;`;
    Request.open('GET', `/adjLight?v=${value}`, true);
    Request.send();
    Request.onreadystatechange = function () {
        if (Request.readyState == 4 && Request.status == 200) {
            var response = Request.responseText;
            drawBrightness(isLight ? Number(response) : 0)
        }
    };
}

function adjHour(value) {
    document.getElementById("optHour").innerHTML = `${value}:00&ensp;`;
    Request.open('GET', `/adjHour?v=${value}`, true);
    Request.send();
}

function toggleLight() {
    isLight = !isLight;
    if (isLight) {
        document.getElementById("Light").style = "color: rgb(255,200,0);"
        Request.open('GET', '/onLight', true);
        Request.send();
        Request.onreadystatechange = function () {
            if (Request.readyState == 4 && Request.status == 200) {
                var response = Request.responseText;
                drawBrightness(Number(response))
            }
        };
    } else {
        document.getElementById("Light").style = "color: gray;"
        ctx.clearRect(0, 0, 300, 300);
    }
}

function drawBrightness(brightness) {
    ctx.clearRect(0, 0, 300, 300);
    brightness = 195 + brightness / 100 * (30 - 195);
    ctx.fillRect(0, brightness, 300, 300);
}

onload = function () {
    ctx = document.getElementById("Canvas").getContext("2d");
    ctx.fillStyle = "rgb(255,200,0)";
    Request = new XMLHttpRequest();
    adjBrightness(90);
    adjLight(70);
    adjHour(12);
}
