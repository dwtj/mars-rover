# comm.py - The heart of the communication system between `control` and `rover`.

import sys
import queue

#import numpy as np

from codes import Signal, MesgID, SubsysID
import sentinel


MAX_DATA_FRAME_LEN = 100


# The sentinel object through which communication with the rover is performed:
sen = sentinel.Sentinel()


def tx_mesg(t, subsys_id = None, command_id = None, data = None):
    """ Sends a message to the rover as described by the arguments.

    - `t` must be a `MesgID`. This is the message type of the message to be
      sent.
    - `subsys_id` must be `None` or a `SubsysID`. This is the value to be sent
      in the subsystem ID byte. If this is `None`, then no such byte will be
      sent in this message.
    - `command_id` is either `None` or an int that is a valid command code (of
      the given `subsys_id`. This is the value to be sent in the command ID
      byte. If this is `None`, then no such byte will be sent in this message.
      Note that `command_id` is `None` if and only if `subsys_id` is `None`. 
    - `data` must be either `None` or a `bytes` object. This is the data to be
      sent in the sequence of data frames. In the case that this is `None`, no
      data frames will be sent.

    The message being sent will illicit a subsequent response message (of the
    same type). Reading this response should be handled by `rx_mesg()`.

    You should expect that no checks of correctness are performed on the args.
    If you pass garbage to this function, garbage may well be sent to the rover.
    """

    global sen

    # TODO: rename `t` to be `mesg_id`.

    # Some validation for function arguments:
    if type(t) != MesgID:
        raise Exception()
    if subsys_id != None and type(subsys_id) != SubsysID:
        raise Exception()

    # `subsys_id` is `None` iff `command_id` is `None`:
    if subsys_id == None and command_id != None:
        raise Exception()
    if subsys_id != None and command_id == None:
        raise Exception()

    sentinel.write_int(Signal.start)
    sentinel.write_int(t)

    if subsys_id is not None:
        sentinel.write_int(subsys_id)
        sentinel.write_int(command_id)
        if data is not None:
            ser.write(frame_data(data))

    sentinel.write_int(Signal.stop)




def rx_mesg(t, subsys_id = None, command_id = None, has_data = False):
    """ Recieves a message from the rover, and expects it to have the format
    specified by the given arguments.

    - `t` must be a `MesgID`. This is the expected message type of the
      message being received.
    - `subsys_id` must be `None` or a `SubsysID`. This is the expected Subsystem
      ID of the message being received. If this is `None`, then no Subsystem ID
      byte is expected.
    - `command_id` must be `None` or an int that is a valid command code of the
      given `subsys_id`. This is the expected value of the Command ID byte of
      the message being received. Note that `command_id` is `None` if and only
      if `subsys_id` is `None`. 
    - `has_data` must be a boolean indicating whether or not a sequence of data
      frames is expected in the message being received.

    This function returns any data that was included with the response message
    as a `bytes` object.

    If there isn't a timely response message, or if the response has
    unexpected features, an `Exception` is raised.
    """

    global sen

    if sen.read_int() != Signal.start:
        raise Exception("Did not receive the expected Start Signal.")

    if sen.read_int() != t:
        raise Exception("Did not receive the expected Message ID.")

    if (subsys_id != None) and (sen.read_int() != subsys_id):
        raise Exception("Did not receive the expected Subsystem ID.")

    if (command_id != None) and (sen.read_int() != command_id):
        raise Exception("Did not receive the expected Command ID.")

    if has_data:
        rv = read_data()

    if sen.read_int() != Signal.stop:
        raise Exception("Did not receive the expected Stop Signal.")




def frame_data(d):
    """ A helper function that takes the bytes object `d` and returns a `bytes`
    object with added data framing bytes. """

    # The number of full frames to be sent:
    full_frames = len(d) // MAX_DATA_FRAME_LEN

    # The final partially-full frame to be sent:
    partial_frame_len = len(d) % MAX_DATA_FRAME_LEN
    has_partial_frame = False if partial_frame_len == 0 else True

    frames = []
    for i in range(full_frames):
        # `start` is index of first byte of `d` to be copied into this `frame`.
        start = i * MAX_DATA_FRAME_LEN
        frame = bytearray(MAX_DATA_FRAME_LEN + 3)

        # Add data and framing info to this `frame`:
        frame[0] = MAX_DATA_FRAME_LEN
        frame[1: 1 + MAX_DATA_FRAME_LEN] = d[start: start + MAX_DATA_FRAME_LEN]
        frame[MAX_DATA_FRAME_LEN + 1] = MAX_DATA_FRAME_LEN
        frame[MAX_DATA_FRAME_LEN + 2] = True

        # Add this newly formed frame to the list of frames to be sent.
        frames.append(frame)

    if has_partial_frame:
        start = full_frames * MAX_DATA_FRAME_LEN
        frame = bytearray(partial_frame_len + 3)

        frame[0] = partial_frame_len
        frame[1 : 1 + partial_frame_len] = d[start : ]
    
    # Set the last byte of the last frame (whether full or partial) to indicate
    # that there are no more frames.
    frames[-1][-1] = False

    return b''.join(frames)




def read_data():
    """ Reads data frame and returns data bytes. """
    
    global sen

    # A list of data frames without the three framing bytes (i.e. the "length",
    # "real length", and "more data" bytes):
    frames = []
    
    # In each iteration, read a data frame and add it to the `frames` list
    # until there are no more data frames to be read.
    more_frames = True
    while more_frames:
        frame_length = sen.read_int()
        frame = bytearray(frame_length)
        if sen.readinto(frame) != frame_length:
            raise Exception("Did not receive enough data bytes.")
        
        # Add only the good bytes to de-framed data:
        num_good_bytes = ser.read_int()
        frames.append(frame[0 : num_good_bytes])
        
        # Check to see if we should keep going:
        more_frames = False if ser.read() == b'\x00' else True
        
    return b''.join(frames)




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




def handle_rover_error(data):
    """
    Interprets the data segment of an error sent from the `rover` and responds
    to it as appropriate.
    """

    raise NotImplementedError()




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
