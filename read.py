
#!/usr/bin/python
# -*- coding: utf-8 -*-
from mfrc522 import SimpleMFRC522
import binascii
import mysql.connector
import time
import datetime
import subprocess
import signal
import sys
import RPi.GPIO as GPIO

mydb = mysql.connector.connect(
        host="localhost",
        user="dg",
        passwd="password",
        database="gunseldb",
)

mycursor = mydb.cursor()
GPIO.setwarnings(False)

reader = SimpleMFRC522()

try:
    id, text = reader.read()
    print(text)
    print(id)
    sql = "UPDATE user SET id = '%s' WHERE name = 'user'" % id
    mycursor.execute(sql)
    mydb.commit()
    print("okay world")
    del id_array[:]
    run = False
                 
finally:
    GPIO.cleanup()
    sys.exit()
                        




