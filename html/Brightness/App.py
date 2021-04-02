from flask import Flask, redirect, url_for, request
from Light import Light

app = Flask(__name__, static_url_path='')
light = Light()


@app.route("/")
def index():
    return redirect(url_for('static', filename='index.html'))


@app.route("/api")
def api():
    value = request.values
    light.adjBrightness(int(value))
    return str(light.brightness)



if __name__ == '__main__':
    app.debug = True
    app.run()
    app.run(debug=True)
