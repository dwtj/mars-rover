# lcd.py

import comm
from enum import IntEnum


class LCDCommand(IntEnum):
    init = 0
    puts = 1
    clear = 2


def init():
    """ Initializes the rover's LCD subsystem for use.  """
    send_message(MsgType.command, Subsystem.lcd, LCDCommand.init, None)
    receive_message(MsgType.command, Subsystem.lcd, LCDCommand.init, False)



def puts(s):
    """ Appends the given string `s` to the rover's LCD display. """
    b = bytes(s, 'utf-8')
    send_message(MsgType.command, Subsystem.lcd, LCDCommand.puts, b)
    recieve_message(MsgType.command, Subsystem.lcd, LCDCommand.puts, False)

    raise NotImplementedError



def clear():
    """ Clears the rover's LCD display. """
    send_message(MsgType.command, Subsystem.lcd, LCDCommand.clear, None)
    recieve_message(MsgType.command, Subsystem.lcd, LCDCommand.clear, False)
