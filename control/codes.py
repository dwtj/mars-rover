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
    scan = 5



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
    play_song = 3 # TODO: refactor. This will not be called directly from control.
    dump = 4
    end_sequence = 5


class OIStopID(IntEnum):
    """ Identifies reason the robot stopped in `oi.move()`. """
    full_distance = 0
    
    left_bumper = 1
    right_bumper = 2
    left_and_right_bumper = 3
    
    front_left_cliff = 4
    front_right_cliff = 5
    left_cliff = 6
    right_cliff = 7

    white_tape_front_left = 8
    white_tape_front_right = 9
    white_tape_left = 10
    white_tape_right = 11

    left_wheel = 12
    right_wheel = 13
    middle_wheel = 14



class SonarCommand(IntEnum):
    init = 0
    readings = 1



class ServoCommand(IntEnum):
    init = 0
    state = 1
    angle = 2
    pulse_width = 3



class IRCommand(IntEnum):
    init = 0
    readings = 1
