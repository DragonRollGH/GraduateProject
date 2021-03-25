class Light:
    def __init__(self):
        self.brightness = 0
        self.snsHour = 12
        self.snsLight = 70
        self.brnSleep = 10
        self.brnTotal = 90

    def adjBrightness(self, value):
        self.brnTotal = value
        self.updateBrightness()

    def adjLight(self, value):
        self.snsLight = value
        if not self.isManSleep():
            self.updateBrightness()

    def adjHour(self, value):
        self.snsHour = value

    def onLight(self):
        self.updateSensors()
        if self.isManSleep():
            self.brightness = self.brnSleep
        else:
            self.updateBrightness()

    def updateSensors(self):
        pass

    def updateBrightness(self):
        self.brightness = self.brnTotal - self.snsLight
        if self.brightness < 0:
            self.brightness = 0

    def isManSleep(self):
        if 0 <= self.snsHour <= 8:
            return True
        else:
            return False