# ir.py

import comm
from enum import IntEnum
from codes import MesgID, SubsysID, IRCommand




def init(sen):
    """ Initializes the IR subsystem for use. """
    sen.stop_watch()
    sen.tx_mesg(MesgID.command, SubsysID.ir, IRCommand.init, None)
    sen.rx_mesg(MesgID.command, SubsysID.ir, IRCommand.init, False)
    sen.start_watch()





def calibrate(sen):
    """
    Initiates the `control`-operated calibration routine of the IR subsystem.
    """

    sen.start_watch()
    # TODO
    sen.stop_watch()
    raise NotImplementedError()




def rover_calibrate(sen):
    """
    Initiates the `rover`-operated calibration routine of the IR subsystem.
    """

    sen.start_watch()
    sen.tx_mesg(Message.command, Subsys.ir, IRCommand.init, None)
    rx_d = sen.rx_mesg(Message.command, Subsys.ir, IRCommand.init, True)
    sen.stop_watch()

    raise NotImplementedError()



def _use_calibration_data(sen, calib_data):
    """
    TODO:
    Another funciton communicates with the rover to generate calibration data
    for the infared (IR) sensor. The results passed to this function as a
    two-column ndarray where the first column is the (human-measured) distance
    from an object and the second column is the integer output from the
    ATmega's ADC connected to the IR sensor. This data is used to modify the
    method by which raw ADC readings are interpreted.
    """

    raise NotImplementedError()



def readings(sen, n, raw = True, rand = False, timestamps = False):
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

    sen.stop_watch()

    # TODO: Specify transmit and response data protocol.
    tx_d = b''
    sen.tx_mesg(MesgID.command, SubsysID.ir, IRCommand.readings, tx_d)
    rx_d = sen.rx_mesg(MessageID.command, SubsysID.ir, IRCommand.readings, True)

    sen.start_watch()

    raise NotImplementedError()
