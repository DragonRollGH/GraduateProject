import sqlite3

conn = sqlite3.connect('database.db')
cur = conn.cursor()
# cur.execute("""CREATE TABLE IoT
# (
#     timestamp number,
#     light number,
#     curtain number,
#     window number,
#     fan number,
#     humidifier number,
#     rotator number,
#     sensorLight number,
#     sensorRain number,
#     sensorTemperature number,
#     sensorHumidity number,
#     sensorBody number
# );
# """)
cur.execute("""
insert into iot values(1852,1,2,3,5,6,7,8,9,1,2,3)
""")
conn.commit()