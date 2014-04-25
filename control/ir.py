# ir.py

import sentinel
from enum import IntEnum
from codes import MesgID, SubsysID, IRCommand




def init(sen):
    """ Initializes the IR subsystem for use. """
    sen.tx_mesg(MesgID.command, SubsysID.ir, IRCommand.init, None)
    sen.rx_mesg(MesgID.command, SubsysID.ir, IRCommand.init, False)





def calibrate(sen):
    """
    Initiates the `control`-operated calibration routine of the IR subsystem.
    """

    # TODO
    raise NotImplementedError()




def _use_calibration_data(sen, calib_data):
    """
    TODO:
    Another function communicates with the rover to generate calibration data
    for the infrared (IR) sensor. The results passed to this function as a
    two-column ndarray where the first column is the (human-measured) distance
    from an object and the second column is the integer output from the
    ATmega's ADC connected to the IR sensor. This data is used to modify the
    method by which raw ADC readings are interpreted.
    """

    raise NotImplementedError()



def readings(sen, n, raw = True, rand = False, timestamps = False):
    """
    Gets an `ndarray` of IR readings from the `rover`.

    Expects `n` to be an unsigned integer. Expects both `rand` and `timestamps`
    to be booleans.

    `rover` should stream `n` independent IR readings back to `control`. 

    If `raw` is `true`, then the raw ADC output is streamed back to `control`
    rather than the approximated distance as calculated by `rover`.

    If `rand` is `true`, then readings will not be taken as often as possible.
    Rather, one each reading should delayed by a random amount of time before
    being taken.

    If `timestamps` is `true`, then timestamps indicating when a reading was
    taken are streamed back to `control` along with the readings themselves.
    
    Data frame sent: 2 bytes for `n` then 1 byte for each boolean parameter 
    (`raw`, `rand`, and `timestamps`).
    
    Data frame received: list each reading (with the corresponding timestamp
    before each, if requested). 
    """

    tx_d = struct.pack("<h???", n, raw, rand, timestamps)
    sen.tx_mesg(MesgID.command, SubsysID.ir, IRCommand.readings, tx_d)
    rx_d = sen.rx_mesg(MessageID.command, SubsysID.ir, IRCommand.readings, True)

    return rx_d
