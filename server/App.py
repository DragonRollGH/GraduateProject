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

if __name__ == '__main__':
    # app.debug = True
    # app.run()
    app.run(debug=True)
