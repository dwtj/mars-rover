# sensors.py - A class that fascilitates conversion from raw sensor data to
# usable measurements using dynamic calibration data.

import numpy as np
import csv
import time

import ir
import sonar
import oi



def calibrate_cliff(sen, cliff_csv):
    """ Interacts with the rover over the given `Sentinel` object to generate
    calibration data for each of the four cliff sensor LEDs of the `rover`.
    This data is then appended to the indicated `.csv` file.

    The rows appended to the `.csv` file will contains four columns, one for
    each of the LEDs. In particular:

    - first column: left sensor
    - second colunn: front left sensor
    - third column: front right sensor
    - fourth column: right sensor

    There will be 100 samples are measured over at least 5 seconds.
    """

    cliff_csv = csv.writer(open(cliff_csv, 'a'))

    for i in range(10):
        dump = oi.dump(sen)
        cliff_csv.writerow([dump[x] for x in range(20, 24)])
        time.sleep(0.05)




def calibrate_servo(sen, servo_csv):
    raise NotImplementedError



def calibrate_dist(sen, ir_csv, sonar_csv, inc = 1.0, start = 5.0, end = 100.0):
    """ Interacts with the rover over the given `Sentinel` object, to generate
    both sonar and IR calibration data to be appended to the `.csv` files
    indicated by the given `String` objects, `ir_csv` and `sonar_csv`.

    This function generates `.csv` files in the format expected by
    `gen_ir_converter()` and `gen_sonar_converter()`. See those functions for
    file format specifications.

    The routine prompts the user to place the rover different distances from an
    object. The first and smallest distance is `start`. The distance is
    incrementally increased by the given `inc`. The largest possible distance
    to be prompted is `end`.

    Any distance measurements which are outside of the supported range of the
    IR sensor (i.e. 9cm to 80cm) will be ignored. This means that no readings 
    from IR will not be requested from the rover for distances outside of the
    supported range. Behavior is similar for the sonar, whose valid range is
    2cm to 3m.

    If a distance is within IR's supported range, then 50 IR readings are
    recorded. The behavior is similar for sonar.
    """

    # TODO: Guard against bad arguments.

    # Open the `csv` writers in append mode:
    sonar_csv = csv.writer(open(sonar_csv, 'a'))
    ir_csv = csv.writer(open(ir_csv, 'a'))

    dist = start

    while dist < end:

        # Report current distance to the user and wait for the enter key:
        input("dist: {:.2f}".format(dist))

        # For both IR and sonar at each distance and for each of the 50
        # readings: add the current distance along with the reading as a row
        # in the `.csv` file.

        if 3.0 <= dist and dist <= 300.0:
            sonar_csv.writerows([(dist, r) for r in sonar.readings(sen, n=50)])

        if 9.0 <= dist and dist <= 80.0:
            ir_csv.writerows([(dist, r) for r in ir.readings(sen, n=50)])

        dist += inc




def gen_servo_converter(csv_file):
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

    Note that the converter object will raise an exception if its angle is not
    within 0 and 180.
    """

    data = np.genfromtxt(csv_file, delimiter=',')
    angles_calib = data[:, 0]
    pulse_widths_calib = data[:, 1]

    # Linear least squares regression:
    conv = np.poly1d(np.polyfit(angles_calib, pulse_widths_calib, 1))

    # Pass this angles to be converted to pulse widths:
    def guarded_conv(angles):
        if type(angles) is np.ndarray:
            if angles[angles < 0.0].any() or angles[angles > 180.0].any():
                raise ValueError("Can't convert angles outside of [0, 180].")
        else:
            # `angles` is actually just a single int or floating point value:
            if angles < 0.0 or angles > 180.0:
                raise ValueError("Can't convert angles outside of [0, 180].")

        # Otherwise, use the polynomial fit to convert to pulse widths:
        return conv(angles)

    return guarded_conv




def gen_sonar_converter(sonar_csv):
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

    return _gen_dist_converter(sonar_csv, 1)




def gen_ir_converter(ir_csv):
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
    raw IR data, that is, the 16-bit integer output generated by the rover's
    ADC connected to the IR sensor, and the second represents the corresponding
    distance measurements measured in centimeters.
    """

    return _gen_dist_converter(ir_csv, 3)




def _gen_dist_converter(csv_file, order):

    data = np.genfromtxt(csv_file, delimiter=',')
    dist_calib = data[:, 0]
    raw_calib = data[:, 1]

    max_dist = max(dist_calib)
    min_dist = min(dist_calib)

    # Perform a polynomial least squares regression of the given order:
    converter = np.poly1d(np.polyfit(raw_calib, dist_calib, order))

    # This is bounded in the sense that it returns whatever `converter` came up
    # with, unless a computed distance is greater than the maximum distance or
    # less than the minimum distance found in the calibration data:
    def bounded_converter(raw):

        dists = converter(raw)

        if type(dists) is np.ndarray:
            dists[dists < min_dist] = 0
            dists[dists > max_dist] = np.inf
        else:
            # `dists` is actually just a single int or floating point value.
            dists = 0 if dists < min_dists else dists
            dists = np.inf if dists > max_dists else dists
        
        return dists

    return bounded_converter
