# oi.py

import struct

from codes import MesgID, SubsysID, OICommand, OIStopID


def init(sen):
    """
    Initializes the rover's open interface subsystem, that is, the open-
    interface over USART communication system between the ATmega128 and the
    iRobot create.
    """

    sen.tx_mesg(MesgID.command, SubsysID.oi, OICommand.init, None)
    sen.rx_mesg(MesgID.command, SubsysID.oi, OICommand.init, False)




def move(sen, dist = 300, speed = 90, stream = False):
    """
    Moves the rover. `speed` is an integer in the range (0, 500]. `dist`, 
    if provided, is in millimeters and is an integer in range [-3000, 3000]. 
    Negative `dist` values command the robot to move backwards.
   
    Returns a 2-tuple, where [0] is the reported distance traveled and [1] is
    a code for why the rover stopped. See `codes.OIStopID` for the encodings.

    Data frame sent: 2 bytes for `speed`, 2 bytes for `distance`, and 1 byte
    for `stream`.

    Data frame received: 2 bytes containing the actual distance traveled 
    and 1 byte containing why `rover` stopped (as encoded in `OIStopID`).
    """

    if sen == "DEBUG":
        return (dist, OIStopID.full_distance)

    if not 0 < speed and speed <= 500:
        raise ValueError("Argument `speed` must be in the interval (0, 500].")
    
    if not (-3000 <= dist and dist <= 3000) and distance != None:
        raise ValueError("Argument `dist` must be in the closed interval"
                                                         "[-3000, 3000].")

    if stream != False:
        raise NotImplementedError()

    b = struct.pack("<Hh?", speed, dist, stream)
    sen.tx_mesg(MesgID.command, SubsysID.oi, OICommand.move, b)
    data = sen.rx_mesg(MesgID.command, SubsysID.oi, OICommand.move, True)
    unpacked_data = struct.unpack("<hB", data)

    return unpacked_data



def rotate(sen, angle):
    """
    Expects `angle` in range (-360, 360). Positive values represent clockwise 
    motion and negative values represent counterclockwise motion. 
    
    Returns nothing.

    Data frame sent: 2 bytes for `angle`.

    Data frame received: 2 bytes for actual angle moved.
    """

    if sen == "DEBUG":
        return

    if not -360 < angle and angle <= 360:
        raise ValueError("Argument `angle` must be in the interval (-360, 360)")

    b = struct.pack("<h", angle)
    sen.tx_mesg(MesgID.command, SubsysID.oi, OICommand.rotate, b)
    data = sen.rx_mesg(MesgID.command, SubsysID.oi, OICommand.rotate, True)
    unpacked_data = struct.unpack("<h", data)

    return unpacked_data




def play_song(sen):
    """
    Plays a song on the rover.
    """
    
    raise NotImplementedError()

    # TODO




def dump(sen):
    """
    TODO: description.

    Dumps all OI data. This function should unpack these bytes and place
    them into a python version of the OI struct.
    """

    # This mirrors the `oi_t` struct defined in `open_interface.h`.
    # Use this format string to unpack the copy of `oi_t` from the rover.

    format = "<"   # little endian

    format += "B"  # bumper_right, bumper_left,
                   # wheeldrop_right, wheeldrop_left, wheeldrop_caster

    format += "B"  # wall
    format += "B"  # cliff_left
    format += "B"  # cliff_frontleft
    format += "B"  # cliff_frontright
    format += "B"  # cliff_right
    format += "B"  # virtual_wall

    format += "B"  # overcurrent_{ld1, ld0, ld2, driveright, driveleft}

    format += "H"  # unused_bytes

    format += "B"  # infared_byte
    format += "B"  # button_{play, advance}

    format += "H"  # distance
    format += "H"  # angle

    format += "B"  # charging_state
    format += "H"  # voltage
    format += "h"  # current
    format += "b"  # temperature
    format += "H"  # charge
    format += "H"  # capacity
    
    format += "H"  # wall_signal
    format += "H"  # cliff_left_signal
    format += "H"  # cliff_frontleft_signal
    format += "H"  # cliff_frontright_signal
    format += "H"  # cliff_right_signal

    format += "B"  # cargo_bay_{io0, io1, io2, io3, baud}
    format += "H"  # cargo_bay_voltage

    format += "B"  # {internal_charger_on, home_base_charger_on}

    format += "B"  # oi_mode

    format += "B"  # song_number
    format += "B"  # song_playing

    format += "B"  # number_packets
    format += "h"  # requested_velocity
    format += "h"  # requested_radius
    format += "h"  # requested_right_velocity
    format += "h"  # requested_left_velocity


    sen.tx_mesg(MesgID.command, SubsysID.oi, OICommand.dump, None)
    data = sen.rx_mesg(MesgID.command, SubsysID.oi, OICommand.dump, True)

    return struct.unpack(format, data)
