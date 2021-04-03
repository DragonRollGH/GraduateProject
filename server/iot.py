import paho.mqtt.client as MQTT


class IoT:
    def __init__(self):
        mqtt = MQTT.Client('IoT')
        mqtt.connect("192.168.1.110")
        self.device = {
            'light': Light(mqtt)
        }

    def api(self, form):
        device = self.device.get(form.get('d'))
        if device:
            method = device.method.get(form.get('m'))
            if method:
                args = form.get('a')
                if args != None:
                    method(args)


class Light:
    def __init__(self, mqtt):
        self.mqtt = mqtt
        self.method = {
            'offset': self.offset
        }
        self.power = 0
        self.value = 0

    def offset(self, value):
        print(value)
        print("**********************")
        self.mqtt.publish('Light', value)
