import paho.mqtt.client as MQTT
import random
import sqlite3
from threading import Timer as setTimeout


def limit(value, MAX, MIN = 0):
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
        self.db.update()

        self.devices = {
            'light': Device(self, 'light', 24, 50, 400),
            'curtain': Device(self, 'curtain', 24, 50, 400),
            # 'window': Window(self),
            # 'fan': Fan(self),
            # 'humidifier': Humidifier(self),
            # 'rotator': Rotator(self)
        }

        self.sensors = {
            'lightness': Sensor(self, 'lightness', ['light', 'curtain']),
            # 'sensorRain': SensorRain(self),
            'temperature': Sensor(self, 'Temperature', []),
            'humidity': Sensor(self, 'humidity', []),
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
        self.period = 60

        self.conn = sqlite3.connect('server/database.db')
        self.cur = conn.cursor()

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

    def record(self, name, value):
        device = self.values.get(name)
        if device != None:
            device.append(value)

    def update(self):
        value = []
        for k in self.values.keys():
            value.append(
                sum(self.values[k])/len(self.values[k]) if self.values[k] else 'null')
            self.values[k] = []
        sql = 'insert into iot values ({})'.format(','.join(value))
        self.cur.execute(sql)
        self.conn.commit()
        setTimeout(self.period, self.update).start()


class Device:
    def __init__(self, iot, name, maxValue, sensorMin, sensorMax):
        self.iot = iot
        self.name = name
        self.power = 1
        self.maxValue = maxValue
        self.sensorMin = sensorMin
        self.sensorMax = sensorMax
        self.value = self.maxValue / 2
        self.offsetValue = 0
        self.outputValue = self.value
        self.method = {
            'offset': self.offset,
            'toggle': self.toggle
        }

    def update(self):
        if self.power is 0:
            self.outputValue = 0
        elif self.power is 2:
            self.outputValue = self.maxValue
        else:
            self.outputValue = limit(self.value + self.offsetValue, self.maxValue)

    def publish(self):
        self.update()
        self.iot.mqtt.publish(self.name, self.outputValue)
        self.iot.db.record(self.name, self.outputValue)
        print('[{}] {}'.format(self.name, self.outputValue))

    def toggle(self, value):
        self.power = int(value)
        self.publish()

    def offset(self, value):
        self.offsetValue = int(value) - self.maxValue
        self.publish()

    def sensor(self, name, value):
        value = limit(value, self.sensorMax, self.sensorMin)
        self.value = round((value - self.sensorMin) * self.MAX / (self.sensorMax - self.sensorMin))
        self.publish()


class Sensor:
    def __init__(self, iot, name, devices):
        self.iot = iot
        self.name = name
        self.value = 0
        self.iot.mqtt.subscribe(self.name)
        self.devices = devices

    def update(self, value):
        self.value = int(value)
        for d in self.devices:
            self.iot.devices[d].sensor(self.name, self.value)
        self.iot.db.record(self.name, self.value)
        print('[{}] {}'.format(self.name, self.value))