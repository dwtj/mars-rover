# servo.py

import comm


def init():
    """
    Initializes the servo subsystem for use. Also activates the servo, i.e.,
    the servo state is made to be "on".
    """

    raise NotImplementedError





def calibrate():
    """
    Initiates the `control`-operated calibration routine of the servo subsystem.
    """

    raise NotImplementedError




def state(s = None)`:
    """
    Gets and sets the servo's state. If the function is invoked with no
    arguments, or if it is passed `None`, then it returns the servos current
    state as a string, either "on" or "off". If the function is passed "on",
    then the servo will be turned on, and if it is passed "off", then the
    servo will be turned off.
    """

    raise NotImplementedError





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

    raise NotImplementedError



def pulse_width(pw):
    """
    Sets `pw` as the proportion of the servo's PWM cycle which is logical-high.

    Expects a floating point number in the interval (0, 1), (though the values
    near the boundaries of this interval are probably not advisable choices).
    """

    raise NotImplementedError
