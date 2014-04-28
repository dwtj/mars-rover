import sentinel
import sensors

sen = sentinel.Sentinel("/dev/tty.ElementSerial-ElementSe")

# Request that the sonar make a single raw reading:
mesg = b'\x01\x03\x02\x01\x05\x01\x00\x01\x00\x00\x05\x00\x02'
sen.write(mesg)

# This is meant to be invoked from the `mars-rover/control` directory:
sensors.calibrate_dist(sen, "calibrate/data/11/ir.csv", "calibrate/data/11/sonar.csv", inc = 1.0, start = 9.0, end = 10.0)
