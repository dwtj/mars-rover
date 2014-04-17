# oi.py

import comm
from enum import IntEnum


class OICommand(IntEnum):
    init = 0



def init():
    """
    Initializes the rover's open interface subsystem, that is, the open-
    interface over USART communication system between the ATmega128 and and the
    iRobot create.
    """

    send_message(MsgType.command, Subsystem.oi, OICommand.init, None)
    recieve_message(MsgType.command, Subsystem.oi, OICommand.init, False)




# TODO: Design other functions.
