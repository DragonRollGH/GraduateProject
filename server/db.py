import sqlite3

conn = sqlite3.connect('database.db')
cur = conn.cursor()
cur.execute("""CREATE TABLE IoT
(
    timestamp number not null primary key,
    light number,
    curtain number,
    window number,
    fan number,
    humidifier number,
    rotator number,
    sensorLight number,
    sensorRain number,
    sensorTemperature number,
    sensorHumidity number,
    sensorBody number
);
""")

cur.execute("""
insert into iot (light, fan) values(4,3)
""")

# cur.execute("""
# drop table iot
# """)
conn.commit()