
onload = function () {
    // ctx = document.getElementById("Canvas").getContext("2d");
    // ctx.fillStyle = "rgb(255,200,0)";
    devices = [
        'light',
    ];
    datas = [];
    requests = [];
    for (let i in devices) {
        requests.push(new XMLHttpRequest());
        requests[i].open('GET', `/api/history?d=${devices[i]}`, true);
        requests[i].send()
        requests[i].onreadystatechange = ()=>{
            if (requests[i].readyState == 4) {
                datas.push(requests[i].responseText)
                console.log(requests[i].responseText);
            }
        }
    }
}