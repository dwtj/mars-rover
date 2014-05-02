# sensors.py - A class that fascilitates conversion from raw sensor data to
# usable measurements using dynamic calibration data.

import sys
import csv
import time
import struct

import numpy as np
import matplotlib.pyplot as plt

import ir
import sonar
import servo
import oi
import codes


def scan(sen, pulse_widths):
    """ This communicates with the rover to efficiently generate raw
    distance readings from both IR and sonar. The `pulse_widths` argument
    is expected to be a list of integer values. For each pulse width in
    this list, the rover's servo will move to that position and collect 
    five raw readings from each of these sensors.

    (Note that we say *efficiently*, because we found scanning to be too
    slow if `control` had to send and recieve a message for each of the
    commands individually (ie. it was too slow to send and recieve a
    servo.pulse_width(), ir.readings(), and sonar.readings() for each angle).

    The return value is a 2-tuple, where both elements are an
    `np.ndarray` object. The first element of the pair is a column vector of
    raw IR data readings, and the second element is a column vector of the raw
    sonar readings.

    At each angle, the rover will record 5 IR readings and 5 sonar
    readings. So the method call will, in total, prompt

        2 * 5 * (abs(end - start) + 1)

    distance readings to be recorded and returned.
    """

    if len(pulse_widths) == 0 or len(pulse_widths) > 200:
        raise Exception('The number of pulse widths in the given list must be '
                                           'in the range [1..200], inclusive.')

    # Allocate the results arrays:
    num_rows = 5 * len(pulse_widths)
    ir_data = np.empty(shape = (num_rows,))
    sonar_data = np.empty(shape = (num_rows,))

    # Generate the request's framed data:
    tx_data = bytearray(2 * len(pulse_widths))
    for i, w in enumerate(pulse_widths):
        struct.pack_into('<H', tx_data, i * 2, w)

    # Send the message, and listen for the response:
    sen.tx_mesg(codes.MesgID.scan, data = tx_data)
    rx_data = sen.rx_mesg(codes.MesgID.scan, has_data = True)

    # expected_len = sensors * angles * readings_per_angle * bytes_per_reading
    expected_len = 2 * len(pulse_widths) * 5 * 2
    if len(rx_data) != expected_len:
        raise Exception('Expected ' + str(expected_len) + ' bytes of data, but '
        								       'received ' + str(len(rx_data)))

    # Unpack the recieved data into `ir_data` and `sonar_data`:
    for i in range(len(pulse_widths)):
        buf_offset = i*20  # For each pulse width, 20 bytes were delivered.
        arr_offset = i*5   # For each pulse width, 5 readings were performed by each sensor.
        ir_data[arr_offset: arr_offset+5] = struct.unpack_from('<HHHHH', rx_data, buf_offset)
        sonar_data[arr_offset: arr_offset+5] = struct.unpack_from('<HHHHH', rx_data, buf_offset + 10)
    
    return (ir_data, sonar_data)




def calibrate_cliff(sen, cliff_csv, n = 10):
    """ Interacts with the rover over the given `Sentinel` object to generate
    calibration data for each of the four cliff sensor LEDs of the `rover`.
    This data is then appended to the indicated `.csv` file.

    The method records `n` samples from each of the four sensors.

    The rows appended to the `.csv` file will contains four columns, one for
    each of the LEDs. In particular:

    - first column: left sensor
    - second colunn: front left sensor
    - third column: front right sensor
    - fourth column: right sensor
    """

    cliff_csv = csv.writer(open(cliff_csv, 'a'))

    for i in range(n):
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


    # For both IR and sonar at each distance and for each of the 50
    # readings: add the current distance along with the reading as a row
    # in the `.csv` file.

    dist = start
    while dist < end:

        # Report current distance to the user and wait for the enter key:
        input("dist: {:.2f}".format(dist))

        try:
            if 3.0 <= dist and dist <= 300.0:
                rows = [(dist, r) for r in sonar.readings(sen, n=50)]
                sonar_csv.writerows(rows)

            if 9.0 <= dist and dist <= 80.0:
                rows = [(dist, r) for r in ir.readings(sen, n=50)]
                ir_csv.writerows()

            dist += inc

        except Exception as ex:
            sys.stderr.write("Exception while generating calibration data:")
            sys.stderr.write(str(ex) + '\n')




def gen_servo_converter(csv_file):
    """ Uses the given `.csv` file to generate a callable object that converts
    from desired angles to pulse width proportions that will move the servo
    to approximately that angle.

    TODO: update comments

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
        return np.rint(conv(angles)).astype(int)

    return guarded_conv




def gen_sonar_converter(sonar_csv, plot_conv = False):
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

    If and only if `plot_conv` is `True` will a plot also be generated showing
    calibration data as a scatterplot along with the resulting converter as
    a curve.
    """

    (conv, raw, dist) = _gen_dist_converter(sonar_csv, 1)

    color = "blue"

    if plot_conv == True:
        _plot_dist_converter(conv, raw, dist, color = "blue")
        
    return conv




def gen_ir_converter(ir_csv, plot_conv = False):
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

    The returned converter will only convert values that are between the
    minimum and maximum `raw` measurements observed in the calibration data.
    In other cases, the converter will return `np.nan`.

    The `.csv` file is expected to have two columns. The first represents the
    raw IR data, that is, the 16-bit integer output generated by the rover's
    ADC connected to the IR sensor, and the second represents the corresponding
    distance measurements measured in centimeters.

    If and only if `plot_conv` is `True` will a plot also be generated showing
    calibration data as a scatterplot along with the resulting converter as
    a curve.
    """

    (conv, raw, dist) = _gen_dist_converter(ir_csv, 5)

    if plot_conv == True:
        _plot_dist_converter(conv, raw, dist, color = "green")
        
    return conv




def _plot_dist_converter(conv, raw, dist, color = "blue"):
    ax = plt.subplot()
    xs = np.arange(min(raw), max(raw) + 1)
    ax.plot(xs, conv(xs), c = color)
    ax.scatter(raw, dist, c = color)




def _gen_dist_converter(csv_file, order):
    """ Uses the given `csv_file` to generates the distance converter via a
    polynomial least squares regression of the given order. Returns a 3-tuple
    containing

    - The converter
    - An `np.ndarray` of the raw sensor readings
    - A parallel `np.ndarray` of the associated distance measurements
    """

    calib_data = np.genfromtxt(csv_file, delimiter=',')
    dist_calib = calib_data[:, 0]
    raw_calib = calib_data[:, 1]

    raw_min = min(raw_calib)
    raw_max = max(raw_calib)

    # Perform a polynomial least squares regression of the given order:
    converter = np.poly1d(np.polyfit(raw_calib, dist_calib, order))

    # Notice that `converter` is now defined for any floating point number,
    # though the regression was only performed for a certain range of values.

    # The following converter is `restricted` in the sense that it is defined
    # on a restricted domain. In particular, it is restricted to raw values
    # between the minimum and maximum values observed in the calibration data.
    def restricted_converter(xs):

        if type(xs) is np.ndarray:

            # If given array contains only integers, convert to floating points:
            if xs.dtype != np.float32 and xs.dtype != np.float64:
                xs = xs.astype(np.float64)

            # Set any input values which are outside of the domain to `NaN`:
            xs[xs < raw_min] = np.nan
            xs[xs > raw_max] = np.nan

        elif type(xs) in {int, float}:
            xs = np.nan if xs < raw_min or xs > raw_max else xs

        else:
            raise TypeError("Argument to the converter must be a numpy array,"
                                                         "an int, or a float.")
        return converter(xs)

    return (restricted_converter, raw_calib, dist_calib)
