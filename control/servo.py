# servo.py

import struct

from codes import MesgID, SubsysID, ServoCommand


def init(sen):
    """
    Initializes the servo subsystem for use. Also activates the servo, i.e.,
    the servo state is made to be "on".
    """

    sen.tx_mesg(MesgID.command, Subsys.servo, ServoCommand.init, None)
    sen.rx_mesg(MesgID.command, Subsys.servo, ServoCommand.init, False)




def rover_calibrate(sen):
    """
    Initiates the `rover`-operated calibration routine of the servo subsystem.
    """
    sen.stop_watch()
    sen.tx_mesg(MesgID.command, Subsys.servo, ServoCommand.calibrate, None)
    sen.rx_mesg(MesgID.command, Subsys.servo, ServoCommand.calibrate, False)
    sen.stop_watch()



def calibrate(sen):
    """
    Initiates the `control`-operated calibration routine of the servo subsystem.
    """

    raise NotImplementedError()
    sen.stop_watch()
    # TODO
    sen.start_watch()



def state(sen, s = None):
    """
    Gets and sets the servo's state. If the function is invoked with no
    arguments, or if it is passed `None`, then it returns the servos current
    state as a string, either "on" or "off". If the function is passed "on",
    then the servo will be turned on, and if it is passed "off", then the
    servo will be turned off.
    """
    
    sen.stop_watch()

    if s == None:
        sen.tx_mesg(MesgID.command, Subsys.servo, ServoCommand.state, False)
        rv = sen.rx_mesg(MesgID.command, Subsys.servo, ServoCommand.state, True)
        rv = "on" if rv == b'\x01' else "off"
    elif s == "on":
        init(sen)
        rv = "on"
    elif s == "off":
        # TODO: How would I tell the servo to turn off?? Send a 0? Make 
        # consistent on rover side
        b = struct.pack("<I", 0)
        sen.tx_mesg(MesgID.command, Subsys.servo, ServoCommand.state, b)
        sen.rx_mesg(MesgID.command, Subsys.servo, ServoCommand.state, False)
        rv = "off"
    else:
        raise ValueError('Argument `s` must be `None`, "on", or "off".');

    sen.start_watch()

    return rv





def angle(sen, angle, wait = True):
    """
    Expects `angle` to be an integer in the range [0..180]. The servo will be
    moved to this angle. If `wait` is `True`, then the following will happen:

    (1) The `rover` will stream status signals indicating "not-finished" until
    it believes that the servo has actually reached the indicated angle, then
    it sends "finished" signal.

    (2) At the same time, the function will continue to block until it receives
    this "finished" signal.
    """
    
    if not (0 <= angle and angle  <= 180):
        raise ValueError("Argument `angle` must be an integer in [0..180].")
    
    start_watch()

    b = struct.pack("<I", angle)
    tx_mesg(MesgID.command, Subsys.servo, ServoCommand.angle, b)
    
    # TODO: wait for "finished" signal from rover. 
    # Is the finished signal in the data sent back? Make consistent with rover
    #while rx_mesg(MesgID.command, Subsys.servo, ServoCommand.angle, True) != ServoSignal.finished:
        #pass
    rx_mesg(MesgID.command, Subsys.servo, ServoCommand.angle, False)

    sen.start_watch()



def pulse(sen, p):
    """
    Sets `p` as the proportion of the servo's PWM cycle which is logical-high.

    Expects a floating point number in the interval (0, 1), (though the values
    near the boundaries of this interval are probably not advisable choices).

    Sends a single 16-bit floating point numer in the data segment of the
    sending message. Expects nothing the the response message's data segment
    """

    if not (0.0 < p and p < 1.0):
        raise ValueError("Argument `p` must be a float in the interval (0, 1).")

    sen.stop_watch()

    b = struct.pack("<f", p)
    sen.tx_mesg(MesgID.command, Subsys.servo, ServoCommand.pulse, b)
    sen.rx_mesg(MesgID.command, Subsys.servo, ServoCommand.pulse, False)

    sen.start_watch()
