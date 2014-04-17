# ir.py

import comm
from enum import IntEnum


class IRCommand(IntEnum):
    init = 0
    calibrate = 1
    readings = 2
    


def init():
    """ Initializes the IR subsystem for use. """

    # TODO: check that response was appropriate
    send_message(MsgType.command, Subsystem.ir, IRCommand.init, None)
    recieve_message(MsgType.command, Subsystem.ir, IRCommand.init, False)





def calibrate():
    """
    Initiates the `control`-operated calibration routine of the IR subsystem.

    TODO: UPDATE THIS:
    Communicates with the rover to generate calibration data for the infared
    (IR) sensor. The results are returned as a two-column ndarray where the
    first column is the (human-measured) distance from an object and the second
    column is the integer output from the ATmega's ADC connected to the IR
    sensor.
    """

    raise NotImplementedError




def readings(n, raw = True, rand = False, timestamps = False):
    """
    Gets an `ndarray` of IR readings from the `rover`.

    Expects `n` to be an unsiged integer. Expects both `rand` and `timestamps`
    to be booleans.

    If `n` is a positive integer, then `rover` should stream `n` independent
    IR readings back to `control`. If `n` is zero, then independent IR readings
    should be streamed back to `control` until `control` sends a command to
    stop.

    If `raw` is `true`, then the raw ADC output is streamed back to `control`
    rather than the approximated distance as calculated by `rover`.

    If `rand` is `true`, then readings will not be taken as often as possible.
    Rather, one each reading should delayed by a random amount of time before
    being taken.

    If `timestamps` is `true`, then timestamps indicating when a reading was
    taken are streamed back to `control` along with the readings themselves.
    """

    # TODO
    d = command(MessageType.command, Subsystem.ir, IRCommand.readings, None)

    raise NotImplementedError
