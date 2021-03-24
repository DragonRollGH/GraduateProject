from flask import Flask


class Light:
    def __init__(self):
        self.brightness = 0
        self.snsHour = 0
        self.snsLight = 0
        self.brnSleep = 0.2
        self.brnTotal = 0.8

    def getBrightness(self):
        self.updateSensors()
        if self.isManSleep():
            self.brightness = self.brnSleep
            return self.brightness
        else:
            self.updateBrightness()
            return self.brightness

    def updateSensors(self):
        pass

    def updateBrightness(self):
        self.brightness += self.brnTotal - self.snsLight

    def isManSleep(self):
        if 0 < self.hour < 8:
            return True
        else:
            return False
