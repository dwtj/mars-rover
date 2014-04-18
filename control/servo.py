# servo.py

import comm
import struct


class ServoCommand(IntEnum):
    init = 0
    calibrate = 1
    state = 2
    angle = 3
    pulse = 4


def init():
    """
    Initializes the servo subsystem for use. Also activates the servo, i.e.,
    the servo state is made to be "on".
    """

    tx_mesg(Message.command, Subsys.servo, ServoCommand.init, None)
    rx_mesg(Message.command, Subsys.servo, ServoCommand.init, False)




def rover_calibrate():
    """
    Initiates the `rover`-operated calibration routine of the servo subsystem.
    """
    tx_mesg(Message.command, Subsys.servo, ServoCommand.calibrate, None)
    rx_mesg(Message.command, Subsys.servo, ServoCommand.calibrate, False)



def calibrate():
    """
    Initiates the `control`-operated calibration routine of the servo subsystem.
    """

    raise NotImplementedError()



def state(s = None):
    """
    Gets and sets the servo's state. If the function is invoked with no
    arguments, or if it is passed `None`, then it returns the servos current
    state as a string, either "on" or "off". If the function is passed "on",
    then the servo will be turned on, and if it is passed "off", then the
    servo will be turned off.
    """
    
    if s == None:
        tx_mesg(Message.command, Subsys.servo, ServoCommand.state, False)
        return rx_mesg(Message.command, Subsys.servo, ServoCommand.state, True)
    elif s == "on":
        init()
    elif s == "off":
        # TODO: How would I tell the servo to turn off?? Send a 0? Make 
        # consistent on rover side
        b = struct.pack("<I", 0)
        tx_mesg(Message.command, Subsys.servo, ServoCommand.state, b)
        rx_mesg(Message.command, Subsys.servo, ServoCommand.state, False)
    else:
        raise ValueError("Argument `s` must be `None`, \"on\", or \"off\".");






def angle(angle, wait = True):
    """
    Expects `angle` to be an integer in the range [0..180]. The servo will be
    moved to this angle. If `wait` is `True`, then the following will happen:

    (1) The `rover` will stream status signals indicating "not-finished" until
    it believes that the servo has actually reached the indicated angle, then
    it sends "finished" signal.

    (2) At the same time, the function will continue to block until it receives
    this "finished" signal.
    """
    
    if not (0 <= p and p  <= 180):
        raise ValueError("Argument `p` must be in the closed interval [0, 180].")
    
    b = struct.pack("<I", p)
    tx_mesg(Message.command, Subsys.servo, ServoCommand.angle, b)
    
    # TODO: wait for "finished" signal from rover. 
    # Is the finished signal in the data sent back? Make consistent with rover
    #while rx_mesg(Message.command, Subsys.servo, ServoCommand.angle, True) != ServoSignal.finished:
        #pass
    rx_mesg(Message.command, Subsys.servo, ServoCommand.angle, False)



def pulse(p):
    """
    Sets `p` as the proportion of the servo's PWM cycle which is logical-high.

    Expects a floating point number in the interval (0, 1), (though the values
    near the boundaries of this interval are probably not advisable choices).

    Sends a single 16-bit floating point numer in the data segment of the
    sending message. Expects nothing the the response message's data segment
    """

    if not (0.0 < p and p < 1.0):
        raise ValueError("Argument `p` must be in the open interval (0, 1).")

    b = struct.pack("<f", p)
    tx_mesg(Message.command, Subsys.servo, ServoCommand.pulse, b)
    rx_mesg(Message.command, Subsys.servo, ServoCommand.pulse, False)
