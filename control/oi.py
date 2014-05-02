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




def dump_param(sen, param, raw_data = None):
    """
    `param` is a string (e.g. "bumper_left") naming an element of the
    C-side OI struct. This function grabs that element either from a
    given `raw_data` tuple or a freshly obtained tuple from `dump()`.
    
    # TODO: double check this works for:
        iRobot Bump Sensor (left, right) 
        iRobot Cliff Sensors (left, front left, front right, right) 
        iRobot Cliff Sensor Signals (left, front left, front right, right) 
    As specified in project document.
    """ 
    
    if raw_data is None:
        raw_data = dump(sen)
    
    # Combine possible choices within unions so they are treated as the same
    if param == 'bumper_left' or param == 'wheeldrop_right' or param == 'wheeldrop_left' or param == 'wheeldrop_caster':
        param = 'bumper_right'
    elif param == 'overcurrent_ld0' or param == 'overcurrent_ld2' or param == 'overcurrent_driveright' or param == 'overcurrent_dirveleft':
        param = 'overcurrent_ld1'
    # TODO: more unions but don't do those yet
    # TODO: sensors are good but it gets goof by the distance/angle part? test

    oi_struct = ['bumper_right', 'wall', 'cliff_left', 'cliff_frontleft','cliff_frontright', 'cliff_right', 'virtual_wall', 'overcurrent_ld1', 'unused_bytes', 'infared_byte', 'button_{play, advance}', 'distance', 'angle', 'charging_state', 'voltage', 'current', 'temperature', 'charge', 'capacity', 'wall_signal', 'cliff_left_signal', 'cliff_frontleft_signal', 'cliff_frontright_signal', 'cliff_right_signal', 'cargo_bay_{io0, io1, io2, io3, baud}', 'cargo_bay_voltage', '{internal_charger_on, home_base_charger_on}', 'oi_mode', 'song_number', 'song_playing', 'number_packets', 'requested_velocity', 'requested_radius', 'requested_right_velocity', 'requested_left_velocity']

    oi_dict = dict(zip(oi_struct, raw_data))

    return oi_dict[param]



def dump(sen):
    """
    Dumps all OI data. This function unpacks all OI bytes and dumps them
    into a tuple.
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



def end_sequence(sen):
    """
    Sends a command to `rover` that it should begin the end sequence, which is
    to flash the power LED and play a song. This behavior is hardcoded on the
    C-side. 
    
    Data frame sent: none.
    
    Data frame received: none.
    """
    
    sen.tx_mesg(MesgID.command, SubsysID.oi, OICommand.end_sequence, None)
    sen.rx_mesg(MesgID.command, SubsysID.oi, OICommand.end_sequence, False)
