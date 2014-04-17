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
    """ Sends a message to the rover as described by the arguments.

    - `t` must be a `MsgType`. This is the message type of the message to be
      sent.
    - `subsys` must be `None` or a `Subsystem`. This is the value to be sent
      in the subsystem ID byte. If this is `None`, then no such byte will be
      sent in this message.
    - `command` is either `None` or an int that is a valid command code (of the
      given `subsys`. This is the value to be sent in the command ID byte. If
      this is `None`, then no such byte will be sent in this message. `command`
      is `None` if and only if `subsys` is `None`. 
    - `data` must be either `None` or a `bytes` object. This is the data to be
      sent in the sequence of data frames. In the case that this is `None`, no
      data frames will be sent.

    The message being sent will illicit a subsequent response message (of the
    same type). Reading this response should be handled by `receive_message()`.

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
    """ Recieves a message from the rover, and expects it to have the format
    specified by the given arguments.

    - `t` must be a `MsgType`. This is the expected message type of the
      message being received.
    - `subsys` must be `None` or a `Subsystem`. This is the expected Subsystem
      ID of the message being received. If this is `None`, then no Subsystem ID
      byte is expected.
    - `command` must be `None` or an int that is a valid command code of the
      given `subsys`. This is the expected value of the Command ID byte of the
      message being received. Command is `None` if and only if `subsys` is
      `None`. 
    - `data` must be a boolean indicating whether or not a sequence of data
      frames is expected in the message being recieved.

    This function returns any data that was included with the response message
    as a `bytes` object.

    If there isn't a timely response message, or if the response has
    unexpected features, an error is raised.
    """

    # TODO: raise errors for timeouts



def frame_data(d):
    """ A helper function that takes the bytes object `d` and returns a `bytes`
    object with added data framing bytes. """

    #num_frames = len

    rv = bytes()
    


def read_data(d):



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
