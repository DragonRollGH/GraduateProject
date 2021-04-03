from flask import Flask, render_template, redirect, url_for, request
from Light import Light

app = Flask(__name__, static_url_path='')
light = Light()


@app.route("/")
def index():
    return redirect(url_for('static', filename='index.html'))


@app.route("/adjBrightness")
def adjBrightness():
    value = request.args.get('v')
    light.adjBrightness(int(value)) 
    return str(light.brightness)


@app.route("/adjLight")
def adjLight():
    value = request.args.get('v')
    light.adjLight(int(value))
    return str(light.brightness)


@app.route("/adjHour")
def adjHour():
    value = request.args.get('v')
    light.adjHour(int(value))
    return str(light.brightness)


@app.route("/onLight")
def onLight():
    light.onLight()
    return str(light.brightness)


if __name__ == '__main__':
    app.debug = True
    app.run()
    app.run(debug=True)
