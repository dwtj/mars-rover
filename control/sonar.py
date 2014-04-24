# sonar.py

from codes import MesgID, SubsysID, SonarCommand


def init(sen):
    """
    Initializes the sonar subsystem for use. Also activates the sonar, i.e.,
    the sonar state is made to be "on".
    """

    sen.tx_mesg(MesgID.command, SubsysID.sonar, SonarCommand.init, None)
    sen.rx_mesg(MesgID.command, SubsysID.sonar, SonarCommand.init, False)



def calibrate(sen):
    """
    Initiates the `control`-operated calibration routine of the sonar subsystem.
    """

    raise NotImplementedError()
    # TODO



def readings(sen, n, raw = True, rand = False, timestamps = False):
    """
    Gets an `ndarray` of sonar readings from the `rover`.

    Expects `n` to be an unsigned integer. Expects both `rand` and `timestamps`
    to be booleans.

    `rover` should stream `n` independent sonar readings back to `control`. 

    If `raw` is `true`, then the raw ADC output is streamed back to `control`
    rather than the approximated distance as calculated by `rover`.

    If `rand` is `true`, then readings will not be taken as often as possible.
    Rather, one each reading should delayed by a random amount of time before
    being taken.

    If `timestamps` is `true`, then timestamps indicating when a reading was
    taken are streamed back to `control` along with the readings themselves.
    
    Data frame sent: 2 bytes for `n` then 1 byte for each boolean parameter (`raw`, `rand`, and `timestamps`).
    
    Data frame received: list each reading (with the corresponding timestamp before each, if requested). 
    """

    tx_d = struct.pack("<h???", n, raw, rand, timestamps)
    sen.tx_mesg(MesgID.command, SubsysID.sonar, SonarCommand.readings, tx_d)
    rx_d = sen.rx_mesg(MessageID.command, SubsysID.sonar, SonarCommand.readings, True)

    return rx_d

