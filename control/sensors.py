# sensors.py - A class that fascilitates conversion from raw sensor data to
# usable measurements using dynamic calibration data.


import numpy as np

def get_servo_converter(csv_file):
    """ Uses the given `.csv` file to generate a callable object that converts
    from desired angles to pulse width proportions that will move the servo
    to approximately that angle.

    In particular, the converter expects a single argument. This argument can
    be:

    - A floating point value, representing a single angle to be converted. In
      this case, a single floating point value will be returned.
    
    - A python list of floating point values, representing a list angles to be
      converted. In this case, a 1-dimensional `numpy.ndarray` containing the
      converted values will be returned.

    - A 1-dimensional `numpy.ndarray` of floating point values to be converted.
      As in the last case, a 1-dimensional `numpy.ndarray` containing the
      converted values will be returned.
    """

    data = np.genfromtxt(csv_file, delimiter=',')
    raw = data[:, 1]
    dist = data[:, 2]

    # Linear least squares regression:
    return np.poly1d(polyfit(raw, dist, 1))




def gen_sonar_converter(csv_file):
    """ Uses the given `.csv` file to generate a callable object that converts
    raw sonar readings to an approximate distance measurement.

    In particular, the converter expects a single argument. This argument can
    be:

    - An integer value, representing a single sonar reading to be converted. In
      this case, a single floating point value will be returned.
    
    - A python list of integer values, representing a list of sonar readings to
      be converted. In this case, a 1-dimensional `numpy.ndarray` containing
      the converted values will be returned.

    - A 1-dimensional `numpy.ndarray` of the integer values to be converted.
      As in the last case, a 1-dimensional `numpy.ndarray` containing the
      converted values will be returned.

    The `.csv` file is expected to have two columns. The first represents the
    raw sonar data, and the second represents converted distance measurements.
    """

    data = np.genfromtxt(csv_file, delimiter=',')
    raw = data[:, 1]
    dist = data[:, 2]

    # Third order least-squares polynomial regression:
    return np.poly1d(polyfit(raw, dist, 3))




def get_ir_converter(csv_file):
    """ Uses the given `.csv` file to generate a callable object that converts
    raw IR readings to an approximate distance measurement.

    In particular, the converter expects a single argument. This argument can
    be:

    - An integer value, representing a single IR reading to be converted. In
      this case, a single floating point value will be returned.
    
    - A python list of integer values, representing a list of IR readings to
      be converted. In this case, a 1-dimensional `numpy.ndarray` containing
      the converted values will be returned.

    - A 1-dimensional `numpy.ndarray` of the integer values to be converted.
      As in the last case, a 1-dimensional `numpy.ndarray` containing the
      converted values will be returned.

    The `.csv` file is expected to have two columns. The first represents the
    raw IR data, and the second represents converted distance measurements.
    """

    data = np.genfromtxt(csv_file, delimiter=',')
    raw = data[:, 1]
    dist = data[:, 2]

    return np.poly1d(polyfit(raw, dist, 3))
