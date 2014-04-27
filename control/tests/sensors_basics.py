# sensors_basics.py

import numpy as np
import matplotlib.pyplot as plt

import sensors

sonar_conv = sensors.gen_sonar_converter("/Users/owl/calib2/8/sonar.csv", True)
ir_conv = sensors.gen_ir_converter("/Users/owl/calib2/8/ir.csv", True)
input("")
