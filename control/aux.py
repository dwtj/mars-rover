# aux.py - The auxiliary thread used for listening to unexpected messages.

from threading import Thread
import queue
import logging

import serial

import comm


# The `aux` thread itself:
aux = None


# The queue by which signals are passed from the main execution thread to `aux`:
signal_queue = None


# A logger for reporting what `aux` finds.
logger = None


# These objects are passed from the main execution thread to `aux` via `q` in
# order to signal to `aux` that it should either stop or start listening:
_stop = object()
_start = object()


# TODO: refactor start so that instantiation of the thread happens elsewhere.
def start(s = None):
    """
    Starts `aux`. Assumes that `aux` has never been started or that it was
    previously stopped. If this is being invoked for the first time, i.e., if
    `aux` is being initialized, then the serial connection to be read must be
    passed as argument `s`.
    """

    global aux, ser, signal_queue, logger, _start

    if aux is None or signal_queue is None:

        if type(s) is not serial.Serial:
            raise Exception("")

        # This is the first time that this function has been called.
        signal_queue = queue.Queue()

        logger = logging.getLogger("aux")
        logger.setLevel("INFO")

        aux = Thread(target = unexpected_listener, args = (s,), daemon = True)
        aux.start()

    signal_queue.put(_start)
    signal_queue.join()  # Blocks until this signal is processed.




def stop():
    """ Starts `aux`. Assumes that `aux` was previously stopped. """

    global signal_queue, _stop
    signal_queue.put(_stop)
    signal_queue.join()  # Blocks until this signal is processed.




def listen(ser):
    """ A helper function used by `unexpected_listener` in the `aux` thread
    to listen for a single message from the rover.

    While listening, the byte-wise timeout is 0.1 seconds.

    This function handles four distinct cases:

    - A ping message is sent from `rover`.
    - An echo message is sent from `rover`.
    - An invalid message (i.e. malformed) is sent from `rover`.
    - An error message is sent from `rover`.

    In all cases, the user is notified by messages to `stderr`. In the first
    two cases, an appropriate response message is sent to the `rover`.
    """

    ser.timeout = 0.1

    # Any start and stop control bytes are read into here:
    sig = bytearray(1)  

    # Any Message ID byte is read into here:
    mesg = bytearray(1)

    # Any deframed data bytes will be saved here:
    d = None

    # Indicates whether the message received was valid:
    valid = False

    # Read start signal byte.
    n = ser.readinto(sig)
    if n == 0:
        return  # No bytes were transmitted from `rover` before the timeout.
 
    if sig[0] == comm.Signal.start:
        logger.info("Receiving an unexpected message...")
        n = ser.readinto(mesg)  # Read Message ID
        mesg = int(mesg[0])  # Reuse the reference.
        if n == 1:
            if mesg in {comm.Message.error, comm.Message.echo}:
                d = comm.read_data()
            n = ser.readinto(sig)  # Read stop signal byte.
            if n == 1 and sig[0] == comm.Signal.stop:
                valid = True
 
    if valid:
        if mesg == comm.Message.ping:
            comm.tx_mesg(comm.Message.ping)
            logger.info("Received a ping message. Responding to it now...")
        elif mesg == comm.Message.echo:
            logger.info("Received an echo message. Responding to it now...")
            comm.tx_mesg(comm.Message.echo, data = d)
        elif mesg == comm.Message.error:
            logger.info("Received an error message. Handling it now...")
            handle_rover_error(d)
    else:
        raise Exception("A malformed message was received from `rover`.")



def unexpected_listener(ser):
    """
    This function is run in the auxiliary thread to listen for unexpected
    messages sent from the `rover`. It should never be run in the main exec
    thread.

    This thread is put to sleep and awoken by the main thread by signals being
    passed via the `signal_queue`. Signals are only passed into this queue
    from the `aux.start()` and `aux.stop()` functions.

    If this thread receives either `ping` or `echo` messages, then this is
    reported and an appropriate response is returned to `rover`. If they are
    `Signal.error` messages, then this is reported, and an `Exception` is
    raised.
    """

    if type(ser) is not serial.Serial:
        raise Exception("")


    logger.info("Thread started.")
    
    while (True):
        # Block until `signal_queue` contains the `_start` signal:
        sig = signal_queue.get(True, None)
        if sig is not _start:
            Exception("The initial signal to start `aux` was not as expected.")

        # Inform the main thread that `aux` has started:
        sig = None
        signal_queue.task_done()
        
        # Keep listening to both `ser` and to `signal_queue` in turn until a
        # stop signal is delivered via the queue. Report anything that happens
        # on `ser`.
        while sig is not _stop:
            listen(ser)
            try:
                sig = signal_queue.get(block = False)
            except queue.Empty:
                sig = None

        # Inform the main thread that `aux` has stopped:
        signal_queue.task_done()
