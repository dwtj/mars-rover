# comm.py - The module by which the control program communicates with the rover
# program.


import serial
import numpy as np
from enum import IntEnum

ser = None  # The serial connection to the rover.


class Signal(IntEnum):
    null = 0
    ping = 1


def connect():
    ser = serial.Serial(port = 0, baudrate = 57600)

    

def disconnect():
    ser.close()
    ser = None
    
    

def send(t, data = None):
    """
    Sends the `Signal` of the given type `t` to the rover, along with `data`.
    """

    # TODO: implement proper guards

    ser.write(bytes(t))
    if data is not None:
        ser.write()



def receive(t):
    """
    Assumes that a correctly formatted signal of type `t` was just sent to the
    rover. Reads and interprets the rover's response to this signal, returning
    any relevant data.
    """

    raise NotImplementedError



def heartbeat():
    """
    Sends ping and recieves ping signals to the rover indefinitely.
    """

    while True:
        send(Signal.ping)
        b = receive()
        if b == Signal.ping:
            sys.stdout.write('.')
        sleep(1)



def echo(stream):
    """
    Writes to `stream`, a subclass of `io.IOBase`, whatever data is coming from
    the rover indefinitely.
    """

    while True:
        stream.write(ser.read())
