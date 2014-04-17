# comm.py - The module by which the control program communicates with the rover
# program.


import serial
import numpy as np
from enum import IntEnum

MAX_DATA_FRAME_LENGTH = 100



ser = None  # The serial connection to the rover.



class Signal(IntEnum):
    null = 0
    start = 1  # Start of message
    stop = 2   # End of message



class MsgType(IntEnum):
    error = 3
    ping = 4
    echo = 5
    command = 6



class Subsystem(IntEnum):
    lcd = 0
    oi = 1
    sonar = 2
    servo = 3
    ir = 4
    rng = 5



def connect():
    ser = serial.Serial(port = 0, baudrate = 57600)

    

def disconnect():
    ser.close()
    ser = None
    



def send_message(t, subsys = None, command = None, data = None):
    """ Issues the command specified by the arguments to the rover.

    - `t` must be a `MsgType`.
    - `subsys` must be `None` or a `Subsystem`.
    - Command is `None` if and only if `subsys` is `None`. Otherwise, `command`
      must be an int that is a valid command code of the given Subsystem.
    - `data` must be either `None` or a `bytes` object.

    A message of type `t` is sent to the rover, and a response message of type
    `t` is expected in return. This function returns any data included with the
    response message as a `bytes` object.

    If there isn't a timely response message, or if the response has
    unexpected features, an error is raised.

    You should expect that no checks of correctness are performed on the args.
    If you pass garbage to this function, garbage may well be sent to the rover.
    """

    # Some validation for function arguments:
    if type(t) != MsgType:
        raise Exception()
    if subsys != None and type(subsys) != Subsystem:
        raise Exception()

    # `subsys` is `None` iff `command` is `None`:
    if subsys == None and command != None:
        raise Exception()
    if subsys != None and command == None:
        raise Exception()

    ser.write(Signal.start)
    ser.write(t)

    if subsys is not None:
        ser.write(subsys)
        ser.write(command)
        if data is not None:
            ser.write(frame_data(data))

    ser.write(Signal.stop)



def recieve_message():
    #TODO



def frame_data(d):
    """ A helper function that takes the bytes object `d` and returns a `bytes`
    object with added data framing bytes. """

    #num_frames = len

    rv = bytes()
    


def deframe_data(d):
    # TODO
    



def heartbeat():
    """
    Sends ping and recieves ping signals to the rover indefinitely.
    """

    while True:
        ping()
        if b == Signal.ping:
            sys.stdout.write('.')
        sleep(1)



def ping():
    send(MsgType.ping)
    response = ser.read(3)
    



def echo():
    



def tty(stream)
    """
    Indefinitely writes to `stream` (a subclass of `io.IOBase`) whatever data
    is coming from the rover.
    """

    while True:
        stream.write(ser.read())
