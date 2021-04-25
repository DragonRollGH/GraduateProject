import json
import random
import sqlite3
import time
from threading import Timer

import paho.mqtt.client as MQTT


def limit(value, MAX, MIN=0):
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
            'light': Device(self, 'light', 24, 50, 400),
            'curtain': Device(self, 'curtain', 180, 50, 400),
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

    def history(self, form):
        return self.db.get()
        # device = form.get('d')
        # if device:
        #     return self.db.get(device)
        # else:
        #     return 0

class DB:
    def __init__(self, iot):
        self.period = 6
        self.name = 'server/database.db'
        self.keys = tuple((
            'time',
            'light',
            'curtain',
            'window',
            'fan',
            'humidifier',
            'rotator',
            'lightness',
            'rain',
            'temperature',
            'humidity',
            'body'
        ))
        self.values = { k: [] for k in self.keys if k is not 'time'}
        self.setTimeout()

    # def get(self, device):
    #     sql = 'select {} from iot'.format(device)
    #     print(sql)
    #     conn = sqlite3.connect(self.name)
    #     cur = conn.cursor()
    #     cur.execute(sql)
    #     data = cur.fetchall()
    #     conn.close()
    #     data = [str(i[0]) for i in data]
    #     return json.dumps(data)
    def get(self):
        sql = 'select * from iot'
        conn = sqlite3.connect(self.name)
        cur = conn.cursor()
        cur.execute(sql)
        data = cur.fetchall()
        conn.close()
        value = {}
        for i,k in enumerate(self.keys):
            value[k] = [d[i] for d in data]
        return json.dumps(value)

    def record(self, name, value):
        device = self.values.get(name)
        if device != None:
            device.append(value)

    def setTimeout(self):
        Timer(self.period, self.update).start()

    def update(self):
        value = []
        allNull = True
        for k in self.values.keys():
            if self.values[k]:
                avg = int(sum(self.values[k])/len(self.values[k])) #TypeError: unsupported operand type(s) for +: 'int' and 'NoneType'
                value.append(str(avg))
                allNull = False
            else:
                value.append('null')
            self.values[k] = []

        if not allNull:
            sql = 'insert into iot values ({},{})'.format(int(time.time()), ','.join(value))
            print(sql)
            conn = sqlite3.connect(self.name)
            cur = conn.cursor()
            cur.execute(sql)
            conn.commit()
            conn.close()
        self.setTimeout()


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
            'toggle': self.toggle,
        }

    def update(self):
        if self.power is 0:
            self.outputValue = 0
        elif self.power is 2:
            self.outputValue = self.maxValue
        else:
            offset = 0
            if self.offsetValue > 0:
                offset = (self.maxValue - self.value) * self.offsetValue
            elif self.offsetValue < 0:
                offset = self.value * self.offsetValue
            self.outputValue = int(limit(self.value + offset, self.maxValue))

    def publish(self):
        self.update()
        self.iot.mqtt.publish(self.name, self.outputValue)
        self.iot.db.record(self.name, self.outputValue)
        print('[{}] {}'.format(self.name, self.outputValue))

    def toggle(self, value):
        self.power = int(value)
        self.publish()

    def offset(self, value):
        self.offsetValue = int(value) / 100
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
