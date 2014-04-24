# lcd.py

from codes import MesgID, SubsysID, LCDCommand



def init(sen):
    """ Initializes the rover's LCD subsystem for use.  """
    sen.tx_mesg(MesgID.command, SubsysID.lcd, LCDCommand.init, None)
    sen.rx_mesg(MesgID.command, SubsysID.lcd, LCDCommand.init, False)



def puts(sen, string):
    """ Appends the given string `s` to the rover's LCD display. """
    b = bytes(s, 'utf-8')
    sen.tx_mesg(MesgID.command, SubsysID.lcd, LCDCommand.puts, b)
    sen.rx_mesg(MesgID.command, SubsysID.lcd, LCDCommand.puts, False)

    raise NotImplementedError



def clear(sen):
    """ Clears the rover's LCD display. """
    sen.tx_mesg(MesgID.command, SubsysID.lcd, LCDCommand.clear, None)
    sen.rx_mesg(MesgID.command, SubsysID.lcd, LCDCommand.clear, False)
