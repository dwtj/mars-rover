# comm.py - The heart of the communication system between `control` and `rover`.

import sys
import queue

#import numpy as np

from codes import Signal, MesgID, SubsysID
import sentinel

# The sentinel object through which communication with the rover is performed:
sen = sentinel.Sentinel()




def heartbeat():
    """ Sends ping and receives ping signals to the rover indefinitely. """

    while True:
        ping()
        sys.stdout.write('.')
        sleep(1)



def ping():
    global sen
    sen.stop_watch()
    tx_mesg(MesgID.ping)
    rx_mesg(MesgID.ping)
    sen.start_watch()

  


def echo(s):
    """
    Sends a message of type `echo` along with the supplied string `s`.
    Expects an identical message to be returned, i.e., a message of type `echo`
    along with this same string `s`.
    """

    b = bytes(s, 'utf-8')
    tx_mesg(MesgID.echo, data = b)
    rv = rx_mesg(MesgID.echo, data = True)
    if b != rv:
        raise Exception("A different string was returned from the rover.")




def tty(stream):
    """
    Indefinitely writes to `stream` (a subclass of `io.IOBase`) whatever data
    is coming from the rover.
    """

    global ser

    while True:
        b = ser.read()
        s = b.decode('utf-8')
        stream.write(s)




def manual_ping():
    global ser
    ser.write(b'\x01')
    ser.write(b'\x04')
    ser.write(b'\x02')
    print(ser.read())
    print(ser.read())
    print(ser.read())




def main():
    """
    if len(sys.argv) != 2:
        sys.stderr.write("Expected one argument, the path of the serial tty.")
        exit()
    """
    ping()




if __name__ == "__main__":
    main()
