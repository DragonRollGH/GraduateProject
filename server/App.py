from flask import Flask, redirect, url_for, request
from iot import IoT

app = Flask(__name__, static_url_path='')
iot = IoT()

@app.route("/")
def index():
    return redirect(url_for('static', filename='index.html'))


@app.route("/api")
def api():
    values = request.values
    iot.api(values)
    return values

@app.route("/api/history")
def history():
    values = request.values
    return iot.history(values)

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True)
