# codes.py - Codes of the communication protocol between `control` and `rover`.

from enum import IntEnum



class Signal(IntEnum):
    """ Different control signals to be sent/received as bytes in a message. """
    null = 0
    start = 1  # Start of message
    stop = 2   # End of message



class MesgID(IntEnum):
    """ Identifies different message types. """
    error = 0
    ping = 1
    echo = 2
    command = 3
    seed_rng = 4



class SubsysID(IntEnum):
    """ Identifies different subsystems in messages. """
    lcd = 0
    oi = 1
    sonar = 2
    servo = 3
    ir = 4



class LCDCommand(IntEnum):
    init = 0
    puts = 1
    clear = 2



class OICommand(IntEnum):
    init = 0
    move = 1
    rotate = 2
    play_song = 3
    dump = 4


class SonarCommand(IntEnum):
    init = 0
    calibrate = 1
    readings = 2



class ServoCommand(IntEnum):
    init = 0
    calibrate = 1
    state = 2
    angle = 3
    pulse = 4



class IRCommand(IntEnum):
    init = 0
    calibrate = 1
    readings = 2
