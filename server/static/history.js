function drawCharts(values) {

}

onload = function () {
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
    var request = new XMLHttpRequest();
    request.open("GET", '/api/history');
    request.onreadystatechange = ()=>{
        if (request.readyState == 4) {
            values = JSON.parse(request.responseText);
            drawCharts(values);
        }
    }
    ctx = document.getElementById("light").getContext("2d");
    // var my = new Chart(ctx, {
    //     type: "line",
    //     data: [0,10,20,10,30,0],
    //     options: {}
    // });
    new Chart(ctx, {
        type: 'line',
        data: {
            labels: 'asdasdasf',
            datasets: [{
                data: [10, 20, 30, 10, 50, 60],
                borderColor: 'red',
            }],
        },
    });
}