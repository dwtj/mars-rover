# rover_basics.py

import rover
import numpy as np

r = rover.Rover()
ir_data, sonar_data = r.scan(20, 90, 90)
scan_data = np.concatenate((ir_data, sonar_data), axis = 0)
r._plot_scan_data(scan_data)


#ir_data, sonar_data = r.scan(20, 0, 20)
#ir_data, sonar_data = r.scan(20, 20, 0)

input("")
