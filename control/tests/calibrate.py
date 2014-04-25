import sentinel
import sensors

sen = sentinel.Sentinel("/dev/tty.ElementSerial-ElementSe-3")

# Request that the sonar make a single raw reading:
mesg = b'\x01\x03\x02\x01\x05\x01\x00\x01\x00\x00\x05\x00\x02'
sen.write(mesg)

#sensors.calibrate_dist(sen, "/Users/owl/calib/2/ir.csv", "/Users/owl/calib/2/sonar.csv", end = 10)
