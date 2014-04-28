# rover_basics.py

import rover

r = rover.Rover()
ir_data, sonar_data = r.scan(1, 90, 90)
ir_data, sonar_data = r.scan(1, 0, 20)
ir_data, sonar_data = r.scan(1, 20, 0)

input("")
