import paho.mqtt.client as MQTT


class IoT:
    def __init__(self):
        self.mqtt = MQTT.Client('IoT')
        self.mqtt.connect("192.168.1.110")
        self.mqtt.on_message = self.on_message
        self.device = {
            'light': Light(self)
        }
        self.sensor = {
            'lightsensor': Lightsensor(self)
        }

    def on_message(self, client, userdata, msg):
        sensor = self.sensor.get(msg.topic)
        if sensor:
            sensor.update(msg.payload)

    def api(self, form):
        device = self.device.get(form.get('d'))
        if device:
            method = device.method.get(form.get('m'))
            if method:
                args = form.get('a')
                if args != None:
                    method(args)

    def GET(self):
        return {
            'light': {
                'power': 1,

            }
        }


class Light:
    def __init__(self, iot):
        self.iot = iot
        self.method = {
            'offset': self.offset
        }
        self.power = 0
        self.value = 0
        self.offsetx = 12

    def publish(self):
        if self.power is 0:
            self.iot.mqtt.publish('light', '0')
        else:
            value = self.value + self.offsetx - 12
            value = 0 if value < 0 else value
            value = 24 if value > 24 else value
        self.iot.mqtt.publish('light', str(value))

    def toggle(self, value):
        self.power = int(value)
        self.publish()

    def offset(self, value):
        self.offsetx = int(value)
        self.publish()

    def sensor(self, value):
        MIN = 50
        MAX = 500
        value = MIN if value < MIN else value
        value = MAX if value > MAX else value
        self.value = round((value - MIN) * 24 / (MAX - MIN))
        self.publish()


class Lightsensor:
    def __init__(self, iot):
        self.iot = iot
        self.value = 0
        self.iot.mqtt.subcribe('lightsensor')

    def update(self, value):
        self.value = int(value)
        self.iot.device['light'].sensor(value)
