import paho.mqtt.client as MQTT
import random


class IoT:
    def __init__(self):
        self.mqtt = MQTT.Client('IoT_'+str(random.randint(10,99)))
        self.mqtt.connect("192.168.1.110")
        self.mqtt.subscribe('IoT')
        self.mqtt.on_message = self.on_message
        self.mqtt.loop_start()

        self.device = {
            'light': Light(self)
        }

        self.sensor = {
            'lightsensor': Lightsensor(self)
        }

    def on_message(self, client, userdata, msg):
        print('[{}] {}'.format(msg.topic, msg.payload))
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


class Light:
    def __init__(self, iot):
        self.iot = iot
        self.method = {
            'offset': self.offset,
            'toggle': self.toggle
        }
        self.power = 1
        self.value = 12
        self.offsetx = 12

    def publish(self):
        if self.power is 0:
            self.iot.mqtt.publish('light', '0')
            print('[light] 0')
        else:
            value = self.value + self.offsetx - 12
            value = 0 if value < 0 else value
            value = 24 if value > 24 else value
            self.iot.mqtt.publish('light', str(value))
            print('[light] ' + str(value))

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
        self.iot.mqtt.subscribe('lightsensor')

    def update(self, value):
        self.value = int(value)
        self.iot.device['light'].sensor(self.value)
