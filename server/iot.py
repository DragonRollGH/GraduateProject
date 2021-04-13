import paho.mqtt.client as MQTT
import random
import sqlite3
from threading import Timer as setTimeout

def limit(value, MIN, MAX):
    value = MIN if value < MIN else value
    value = MAX if value > MAX else value
    return value


class IoT:
    def __init__(self):
        try:
            self.mqtt = MQTT.Client('IoT_'+str(random.randint(10, 99)))
            self.mqtt.connect("192.168.1.110")
            self.mqtt.subscribe('IoT')
            self.mqtt.on_message = self.mqttMsg
            self.mqtt.loop_start()
        except Exception as e:
            print(e)

        self.db = DB(self)

        self.devices = {
            'light': Light(self),
            'curtain': Curtain(self),
            # 'window': Window(self),
            # 'fan': Fan(self),
            # 'humidifier': Humidifier(self),
            # 'rotator': Rotator(self)
        }

        self.sensors = {
            'sensorLight': SensorLight(self),
            # 'sensorRain': SensorRain(self),
            'sensorTemperature': SensorTemperature(self),
            'sensorHumidity': SensorHumidity(self),
            # 'sensorBody': SensorBody(self)
        }

    def mqttMsg(self, client, userdata, msg):
        print('[{}] {}'.format(msg.topic, msg.payload))
        sensor = self.sensors.get(msg.topic)
        if sensor:
            sensor.update(msg.payload)

    def api(self, form):
        device = self.devices.get(form.get('d'))
        if device:
            method = device.method.get(form.get('m'))
            if method:
                args = form.get('a')
                if args != None:
                    method(args)

class DB:
    def __init__(self, iot):
        self.timeout = 60
        self.values = {
            'light': [],
            'curtain': [],
            'window': [],
            'fan': [],
            'humidifier': [],
            'rotator': [],
            'sensorLight': [],
            'sensorRain': [],
            'sensorTemperature': [],
            'sensorHumidity': [],
            'sensorBody': []
        }

    def record (self, name, value):
        device = self.values.get(name)
        if device != None:
            device.append(value)

    def update(self):
        setTimeout(self.timeout, self.update).start()


class Light:
    def __init__(self, iot, name):
        self.iot = iot
        self.name = name
        self.method = {
            'offset': self.offset,
            'toggle': self.toggle
        }
        self.MAX = 24
        self.power = 1
        self.value = self.MAX / 2
        self.offsetx = self.MAX

    def publish(self):
        if self.power is 0:
            self.iot.mqtt.publish('light', '0')
            print('[light] 0')
        else:
            value = self.value + self.offsetx - self.MAX
            value = limit(value, 0, self.MAX)
            value = int(value)
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
        MAX = 400
        value = limit(value, MIN, MAX)
        self.value = round((value - MIN) * self.MAX / (MAX - MIN))
        self.publish()


class Curtain:
    def __init__(self, iot):
        self.iot = iot
        self.method = {
            'offset': self.offset,
            'toggle': self.toggle
        }
        self.MAX = 180
        self.power = 1
        self.value = self.MAX / 2
        self.offsetx = self.MAX

    def publish(self):
        if self.power is 0:
            self.iot.mqtt.publish('curtain', '0')
            print('[curtain] 0')
        elif self.power is 2:
            self.iot.mqtt.publish('curtain', str(self.MAX))
            print('[curtain] '+ str(self.MAX))
        else:
            value = self.value + self.offsetx - self.MAX
            value = limit(value, 0, self.MAX)
            value = int(value)
            self.iot.mqtt.publish('curtain', str(value))
            print('[curtain] ' + str(value))

    def toggle(self, value):
        self.power = int(value)
        self.publish()

    def offset(self, value):
        self.offsetx = int(value)
        self.publish()

    def sensor(self, value):
        MIN = 50
        MAX = 400
        value = limit(value, MIN, MAX)
        self.value = round((value - MIN) * self.MAX / (MAX - MIN))
        self.publish()


class Window:
    def __init__(self, iot):
        self.iot = iot
        self.method = {
            'offset': self.offset,
            'toggle': self.toggle
        }
        self.MAX = 180
        self.power = 1
        self.value = self.MAX / 2
        self.offsetx = self.MAX

    def publish(self):
        if self.power is 0:
            self.iot.mqtt.publish('curtain', '0')
            print('[curtain] 0')
        elif self.power is 2:
            self.iot.mqtt.publish('curtain', str(self.MAX))
            print('[curtain] '+ str(self.MAX))
        else:
            value = self.value + self.offsetx - self.MAX
            value = limit(value, 0, self.MAX)
            value = int(value)
            self.iot.mqtt.publish('curtain', str(value))
            print('[curtain] ' + str(value))

    def toggle(self, value):
        self.power = int(value)
        self.publish()

    def offset(self, value):
        self.offsetx = int(value)
        self.publish()

    def sensor(self, value):
        MIN = 50
        MAX = 400
        value = limit(value, MIN, MAX)
        self.value = round((value - MIN) * self.MAX / (MAX - MIN))
        self.publish()


class Fan:
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
            self.iot.mqtt.publish('window', '0')
            print('[window] 0')
        elif self.power is 2:
            self.iot.mqtt.publish('window', '2')
            print('[window] 2')
        else:
            value = self.value + self.offsetx - 12
            value = 0 if value < 0 else value
            value = 24 if value > 24 else value
            self.iot.mqtt.publish('window', str(value))
            print('[window] ' + str(value))

    def toggle(self, value):
        self.power = int(value)
        self.publish()

    def offset(self, value):
        self.offsetx = int(value)
        self.publish()

    def sensor(self, value):
        MIN = 50
        MAX = 400
        value = MIN if value < MIN else value
        value = MAX if value > MAX else value
        self.value = round((value - MIN) * 24 / (MAX - MIN))
        self.publish()


class Humidifier:
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
            self.iot.mqtt.publish('window', '0')
            print('[window] 0')
        elif self.power is 2:
            self.iot.mqtt.publish('window', '2')
            print('[window] 2')
        else:
            value = self.value + self.offsetx - 12
            value = 0 if value < 0 else value
            value = 24 if value > 24 else value
            self.iot.mqtt.publish('window', str(value))
            print('[window] ' + str(value))

    def toggle(self, value):
        self.power = int(value)
        self.publish()

    def offset(self, value):
        self.offsetx = int(value)
        self.publish()

    def sensor(self, value):
        MIN = 50
        MAX = 400
        value = MIN if value < MIN else value
        value = MAX if value > MAX else value
        self.value = round((value - MIN) * 24 / (MAX - MIN))
        self.publish()


class Rotator:
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
            self.iot.mqtt.publish('window', '0')
            print('[window] 0')
        elif self.power is 2:
            self.iot.mqtt.publish('window', '2')
            print('[window] 2')
        else:
            value = self.value + self.offsetx - 12
            value = 0 if value < 0 else value
            value = 24 if value > 24 else value
            self.iot.mqtt.publish('window', str(value))
            print('[window] ' + str(value))

    def toggle(self, value):
        self.power = int(value)
        self.publish()

    def offset(self, value):
        self.offsetx = int(value)
        self.publish()

    def sensor(self, value):
        MIN = 50
        MAX = 400
        value = MIN if value < MIN else value
        value = MAX if value > MAX else value
        self.value = round((value - MIN) * 24 / (MAX - MIN))
        self.publish()


class SensorLight:
    def __init__(self, iot):
        self.iot = iot
        self.value = 0
        self.iot.mqtt.subscribe('sensorlight')

    def update(self, value):
        self.value = int(value)
        self.iot.devices['light'].sensor(self.value)
        self.iot.devices['curtain'].sensor(self.value)


class SensorRain:
    def __init__(self, iot):
        self.iot = iot
        self.value = 0
        self.iot.mqtt.subscribe('sensorLight')

    def update(self, value):
        self.value = int(value)
        self.iot.devices['light'].sensor(self.value)
        self.iot.devices['curtain'].sensor(self.value)


class SensorTemperature:
    def __init__(self, iot):
        self.iot = iot
        self.value = 0
        self.iot.mqtt.subscribe('sensorTemperature')

    def update(self, value):
        self.value = int(value)
        self.iot.devices['window'].sensor(self.value)


class SensorHumidity:
    def __init__(self, iot):
        self.iot = iot
        self.value = 0
        self.iot.mqtt.subscribe('sensorHumidity')

    def update(self, value):
        self.value = int(value)


class SensorBody:
    def __init__(self, iot):
        self.iot = iot
        self.value = 0
        self.iot.mqtt.subscribe('sensorlight')

    def update(self, value):
        self.value = int(value)
        self.iot.devices['light'].sensor(self.value)
        self.iot.devices['curtain'].sensor(self.value)
