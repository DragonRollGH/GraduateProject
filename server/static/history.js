function drawCharts(values) {
    let charts = {
        light: ['light','curtain'],
    };
    let colors = [
        'red',
        'blue',
    ]
    let labels = values.time;
    for (let chart in charts) {
        let ctx = document.getElementById(chart).getContext("2d");
        let datasets = [];
        for (let i in charts[chart]) {
            datasets.push({
                data: values[charts[chart][i]],
                label: charts[chart][i],
                borderColor: colors[i],
                spanGaps: true,
                lineTension: 0.5,
                stepped: true,
            });
        }
        new Chart(ctx, {
            type: 'line',
            data: {
                labels: labels,
                datasets: datasets,
            },
        });
    }
}

onload = function () {
    let request = new XMLHttpRequest();
    request.open("GET", '/api/history');
    request.send();
    request.onreadystatechange = ()=>{
        if (request.readyState == 4) {
            let values = JSON.parse(request.responseText);
            drawCharts(values);
        }
    }
}