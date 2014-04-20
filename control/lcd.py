# lcd.py

from codes import MesgID, SubsysID, LCDCommand



def init(sen):
    """ Initializes the rover's LCD subsystem for use.  """
    sen.stop_watch()
    sen.tx_mesg(MesgID.command, SubsysID.lcd, LCDCommand.init, None)
    sen.rx_mesg(MesgID.command, SubsysID.lcd, LCDCommand.init, False)
    sen.start_watch()



def puts(sen, string):
    """ Appends the given string `s` to the rover's LCD display. """
    sen.stop_watch()
    b = bytes(s, 'utf-8')
    sen.tx_mesg(MesgID.command, SubsysID.lcd, LCDCommand.puts, b)
    sen.rx_mesg(MesgID.command, SubsysID.lcd, LCDCommand.puts, False)
    sen.start_watch()

    raise NotImplementedError



def clear(s):
    """ Clears the rover's LCD display. """
    sen.stop_watch()
    sen.tx_mesg(MesgID.command, SubsysID.lcd, LCDCommand.clear, None)
    sen.rx_mesg(MesgID.command, SubsysID.lcd, LCDCommand.clear, False)
    sen.start_watch()
