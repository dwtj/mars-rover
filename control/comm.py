# comm.py - The module by which the control program communicates with the rover
# program.


import serial
from enum import IntEnum

ser = None

class Query(IntEnum):
    null = 0
    ping = 1


def connect():
    ser = serial.Serial()
    ser.port(0)
    ser.baud(57600)
    ser.open()
    
    

def disconnect():
    ser.close()
    
    

def send(t):
    """
    Sends the signal of the given type `t` to the rover.
    """
    assert type(t) == Query


def heartbeat():
    while True:
        send(Query.ping)
        sleep(1)
