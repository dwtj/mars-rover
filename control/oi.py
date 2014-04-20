# oi.py

from codes import MesgID, SubsysID, OICommand



def init(sen):
    """
    Initializes the rover's open interface subsystem, that is, the open-
    interface over USART communication system between the ATmega128 and the
    iRobot create.
    """

    sen.stop_watch()
    sen.tx_mesg(MesgID.command, SubsysID.oi, OICommand.init, None)
    sen.rx_mesg(MesgID.command, SubsysID.oi, OICommand.init, False)
    sen.start_watch()



def move(sen, speed = 500, distance = 3000, stream = False):
    """
    Moves the rover. `speed` is an integer in the range (0, 500]. `distance`, 
    if provided, is in millimeters and is an integer in range [-3000, 3000]. 
    Negative `distance` values command the robot to move backwards.
   
    TODO: what bytes in data frame mean what (in both directions)
    """
    
    sen.stop_watch()

    if not 0 < speed and speed <= 500:
        raise ValueError("Argument `speed` must be in the interval (0, 500].")
    
    if not (0 < distance and distance <= 300) and distance != None:
        raise ValueError("Argument `distance` must be in the closed interval [-3000, 3000].")

    b = pack("<Hh?", speed, distance, stream)
    sen.tx_mesg(MesgID.command, SubsysID.oi, OICommand.move, b)
    data = sen.rx_mesg(MesgID.command, SubsysID.oi, OICommand.move, True)
    
    # ^ TODO: return this data right after sen.start_watch()??

    # TODO: in byte describing why rover stopped: left, right bumper? other?

    sen.start_watch()



def rotate(sen, angle):
    """
    Expects `angle` in range (-360, 360). Positive values represent clockwise 
    motion and negative values represent counterclockwise motion. 
    """

    raise NotImplementedError()
    sen.stop_watch()
    # TODO
    sen.start_watch()



def play_song(sen):
    """
    Plays a song on the rover.
    """
    
    raise NotImplementedError()

    sen.stop_watch()
    # TODO
    sen.start_watch()

