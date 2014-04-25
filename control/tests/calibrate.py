import sentinel
import sensors

sen = sentinel.Sentinel("/dev/tty.ElementSerial-ElementSe-3")

sensors.calibrate_dist(sen, "/Users/owl/calib/2/ir.csv", "/Users/owl/calib/2/sonar.csv", end = 10)
