# oi.py

from codes import MesgID, SubsysID, OICommand



def init(sen):
    """
    Initializes the rover's open interface subsystem, that is, the open-
    interface over USART communication system between the ATmega128 and the
    iRobot create.
    """

    sen.tx_mesg(MesgID.command, SubsysID.oi, OICommand.init, None)
    sen.rx_mesg(MesgID.command, SubsysID.oi, OICommand.init, False)



def move(sen, speed = 500, distance = 3000, stream = False):
    """
    Moves the rover. `speed` is an integer in the range (0, 500]. `distance`, 
    if provided, is in millimeters and is an integer in range [-3000, 3000]. 
    Negative `distance` values command the robot to move backwards.
   
    Returns a list where [0] is the encoding for the reason for stopping
    and [1] is the actual distance traveled.

    Data frame sent: 2 bytes for `speed`, 2 bytes for `distance`, and 1 byte
    for `stream`.

    Data frame received: 1 byte containing why `rover` stopped (as encoded 
    in `OIStopCommand`) and 2 bytes containing the actual distance traveled.

    """

    if not 0 < speed and speed <= 500:
        raise ValueError("Argument `speed` must be in the interval (0, 500].")
    
    if not (0 < distance and distance <= 300) and distance != None:
        raise ValueError("Argument `distance` must be in the closed interval [-3000, 3000].")

    if stream != False:
        raise NotImplementedError()

    b = pack("<Hh?", speed, distance, stream)
    sen.tx_mesg(MesgID.command, SubsysID.oi, OICommand.move, b)
    data = sen.rx_mesg(MesgID.command, SubsysID.oi, OICommand.move, True)
    unpacked_data = struct.unpack("<bh", data)

    return unpacked_data


def rotate(sen, angle):
    """
    Expects `angle` in range (-360, 360). Positive values represent clockwise 
    motion and negative values represent counterclockwise motion. 
    """

    raise NotImplementedError()
    # TODO



def play_song(sen):
    """
    Plays a song on the rover.
    """
    
    raise NotImplementedError()

    # TODO



def dump(sen):
    """
    Dumps all OI data. This function should unpack these bytes and place
    them into a python version of the OI struct.
    """
    
    raise NotImplementedError()
    
    # TODO
