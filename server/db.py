import sqlite3
import time
from threading import Timer as setTimeout

conn = sqlite3.connect('server/database.db')
cur = conn.cursor()

# cur.execute("""
# drop table iot
# """)
# cur.execute("""CREATE TABLE IoT
# (
#     timestamp number not null primary key,
#     light number,
#     curtain number,
#     window number,
#     fan number,
#     humidifier number,
#     rotator number,
#     lightness number,
#     rain number,
#     temperature number,
#     humidity number,
#     body number
# );
# """)

# conn.commit()


sql = "select * from IoT"
cur.execute(sql)
a = cur.fetchall()
pass
