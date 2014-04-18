# oi.py

import comm
from enum import IntEnum


class OICommand(IntEnum):
    init = 0
    move = 1
    rotate = 2
    stop = 3
    play_song = 4



def init():
    """
    Initializes the rover's open interface subsystem, that is, the open-
    interface over USART communication system between the ATmega128 and the
    iRobot create.
    """

    tx_mesg(Message.command, Subsys.oi, OICommand.init, None)
    rx_mesg(Message.command, Subsys.oi, OICommand.init, False)

def move(speed = 500, distance = None):
    """
    Moves the rover. `speed` is an integer in the range (0, 500]. `distance`, 
    if provided, is in millimeters and is an integer in range [-3000, 3000]. 
    Negative `distance` values command the robot to move backwards. If
    distance is not provided, the rover will move until stop() is called.
    """
    
    if not 0 < speed and speed <= 500:
        raise ValueError("Argument `speed` must be in the interval (0, 500].")
    
    if not (0 < distance and distance <= 300) and distance != None:
        raise ValueError("Argument `distance` must be in the closed interval [-3000, 3000].")

    if distance != None:
        # TODO: Pack just speed. Don't block?
        pass
    else:
        # TODO: Pack together speed and distance. Indicate somehow that there 
        # is a specific distance to move. Block until "finished" signal?
        b = pack("<Ii", speed, distance)
        tx_mesg(Message.command, Subsys.oi, OICommand.move, b)
        rx_mesg(Message.command, Subsys.oi, OICommand.move, False)
    
    # TODO: Something with collision detection


def rotate(angle):
    """
    Expects `angle` in range (-360, 360). Positive values represent clockwise 
    motion and negative values represent counterclockwise motion. 
    """
    
    raise NotImplementedError()


def stop():
    """
    If the rover is currently moving, this function stops it.
    """
    
    raise NotImplementedError()

def play_song():
    """
    Plays a song on the rover.
    """
	
	raise NotImplementedError()

