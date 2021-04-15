import sqlite3
import time
from threading import Timer as setTimeout

conn = sqlite3.connect('server/database.db')
cur = conn.cursor()

cur.execute("""
drop table iot
""")
cur.execute("""CREATE TABLE IoT
(
    timestamp number not null primary key,
    light number,
    curtain number,
    window number,
    fan number,
    humidifier number,
    rotator number,
    lightness number,
    rain number,
    temperature number,
    humidity number,
    body number
);
""")

conn.commit()

# value = ['0' for i in range(11)]

# cur.execute(sql)


# def update():
#     conn = sqlite3.connect('server/database.db')
#     cur = conn.cursor()
#     sql = 'insert into iot values ({},{})'.format(time.time(), ','.join(value))
#     cur.execute(sql)
#     conn.commit()
#     conn.close()
#     print(sql)
#     setTimeout(5, update).start()

# update()
# input()