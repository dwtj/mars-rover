# codes.py - Codes of the communication protocol between `control` and `rover`.

class Signal(IntEnum):
    """ Different control signals to be sent/received as bytes in a message. """
    null = 0
    start = 1  # Start of message
    stop = 2   # End of message



class Message(IntEnum):
    """ Identifies different message types. """
    error = 3
    ping = 4
    echo = 5
    command = 6



class Subsys(IntEnum):
    """ Identifies different subsystems in messages. """
    lcd = 0
    oi = 1
    sonar = 2
    servo = 3
    ir = 4
    rng = 5
