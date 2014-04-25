# ir.py

import sentinel
from enum import IntEnum
from codes import MesgID, SubsysID, IRCommand




def init(sen):
    """ Initializes the IR subsystem for use. """
    sen.tx_mesg(MesgID.command, SubsysID.ir, IRCommand.init, None)
    sen.rx_mesg(MesgID.command, SubsysID.ir, IRCommand.init, False)





def readings(sen, n = 50, raw = True, rand = False, timestamps = False):
    """
    Returns a `tuple` of length `n` of IR readings from the `rover`.

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

    if raw = True and timestamps = False:
        pack_format = "<" + "H" * n
        readings = struct.unpack(pack_format, rx_d)
    else:
        raise NotImplementedError("raw = False and timestamps = False are not implemented.")
    

    return readings

