# Design of the comm Module #

It should be the purpose of `comm.py` (the communications module of `control`) to transmit commands to the rover, while other python modules should describe control logic, perform data analysis, and render the GUI.

It should be the purpose of `comm.c` (the communications module of `rover`) to follow these commands. It will manage command and control actions issued by `comm.py` and to package response data. In particular, `comm.c` should offer up to `comm.py` the following capabilities in each of the relevant subsystems (i.e. `lcd`, `oi`, `sonar`, `servo`, and `ir`):

- Initialize a subsystem.
- Calibrate a subsystem.
- Send commands to a subsystem which will stream a list of responses back.

## Message Protocol ##

<table>
	<tr>
		<td>
			Start
		</td>
		<td>
			MesgID
		</td>
		<td>
			SubsysID
		</td>
		<td>
			Subsystem Command
		</td>
		<td>
			Data Length, n
		</td>
		<td>
			Data Byte 0
		</td>
		<td>
			Data Byte 1
		</td>
		<td>
			...	
		</td>
		<td>
			Data Byte n - 1
		</td>
		<td>
			Real Data Length
		</td>
		<td>
			Another Data Frame?
		</td>
		<td>
			...
		</td>
		<td>
			Stop
		</td>
	</tr>
</table>

Start, MesgID and Stop are essential in every message. SubsysID, the subsystem command (e.g. an element from LCDCommand, OICommand, etc) the data frame (Data Length, Data Byte, and "Another Data Frame?") may be necessary, depending on MesgID and SubsysID. 



## Design Assumptions ##

`control` can only ever send one command at a time, and the `rover` can only be executing this one command at any time.



## Subsystem Initialization ##

`control` should be able to initialize each of the following `rover` subsystems:

- LCD
- Open Interface
- Sonar
- Servo
- IR
- RNG



## Subsystem Calibration ##

`control` should be able to calibrate each of the following `rover` subsystems:

- Sonar
- Servo
- IR



## Subsystem Commands ##

`control` should be able to issue commands to each of the following `rover` subsystems:

- LCD
- Open Interface
- Sonar
- Servo
- IR

Until a `rover` subsystem completes the given command, the subsystem will be delivering a stream of data to be transmitted back to `control`. These may be sensor readings or status signals.



### LCD commands ###

`control` need only be able to perform two commands on the lcd display: clear it and write a string to it. The response stream could just be signaling that when it is finally complete.

- `init(sen)`
- `puts(sen)`
- `clear(sen)`



### Open Interface Commands ###

- `init(sen)`
- `move(sen, speed = 500, distance = 3000, stream = False)`
- `rotate(sen, angle)`
- `play_song(sen)`



### Sonar Commands ###

- `init(sen)`
- `calibrate(sen)`
- `readings(n, raw = True, rand = False, timestamps = False)`



### Servo Commands ###

- `init(sen)`
- `calibrate(sen)`
- `state(sen, s = None)`
- `angle(sen, angle, wait = true)`
- `pulse(sen, p)`



### IR commands ###

- `init(sen)`
- `calibrate(sen)`
- `readings(sen, n, raw = True, rand = False, timestamps = False)`



## GUI ##

The GUI would ideally include a python interpreter at which we can invoke individual commands. An issued command/query should return a list of data. It should also display a plot of the data from the robot.


