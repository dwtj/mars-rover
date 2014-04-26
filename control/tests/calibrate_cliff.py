import sentinel
import sensors

sen = sentinel.Sentinel("/dev/tty.ElementSerial-ElementSe")

input("Place all cliff sensors over a cliff...")
sensors.calibrate_cliff(sen, "/Users/owl/calib/3/cliff/black.csv")

input("Place all cliff sensors over the white tape...")
sensors.calibrate_cliff(sen, "/Users/owl/calib/3/cliff/white.csv")

input("Place all cliff sensors over the grey floor tile...")
sensors.calibrate_cliff(sen, "/Users/owl/calib/3/cliff/gray.csv")
