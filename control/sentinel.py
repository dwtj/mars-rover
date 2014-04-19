# sentinel.py - Defines the sentinel class.

import threading
import queue
import logging

import serial


DEFAULT_SERIAL_PORT = "/dev/tty.ElementSerial-ElementSe"


class Sentinel():
    """ Manages a serial connection and watches for unexpected messages. """

    # The `aux` thread which is used to watch for unexpected messages:
    aux = None

    # The queue by which messages are passed from the main thread to `aux`:
    aux_queue = None

    # A logger for reporting what `aux` finds.
    aux_log = None

    # The serial connection to the rover.
    ser = None

    # A boolean flag indicating whether the `aux` thread is currently watching.
    is_watching = False

    # These objects are passed from the main execution thread to `aux` via
    # `aux_queue` in order to signal to `aux` that it should either stop or
    # start watching the connection asynchronously:
    _stop = object()
    _start = object()

    def __init__(self, port = DEFAULT_SERIAL_PORT):
        """ Instantiates a sentinel object on the given serial port. """ 

        # Create the Serial, Queue, and Logger to be used throughout this
        # sentinel's lifespan:

        self.ser = serial.Serial()
        self.ser.port = port
        self.ser.baudrate = 57600
        self.ser.stopbits = serial.STOPBITS_TWO

        self.aux_queue = queue.Queue()

        self.logger = logging.getLogger("sentinel")
        self.logger.setLevel("INFO")


        # Clear whatever data might have already been in the serial buffer:

        self.ser.timeout = 0
        count = 0
        while ser.read() != b'':
            count += 1
        if count > 0:
            self.logger.warning("The serial buffer was not initially empty.")
            self.logger.warning(str(count) + " old bytes were discarded.")


        # Start the `aux` thread:

        self.aux = threading.Thread(target = _watchman, args = (self,))
        self.aux.daemon = True
        self.aux.start()
        self.is_watching = False  # The `aux` thread is not initially watching.




    def __del__(self):
        self.stop_watch()
        self.ser.close()




    def read(self, size = 1):
        """
        Reads and returns the `bytes` object (of the given `size`) from
        the `read()` method of the `Serial` object encapsulated by this
        `Sentinel object. See `Serial.read()` for details.
        
        The timeout is hard-coded to be 1 second.
        """

        if self.is_watching:
            raise Exception("You cannot read when the sentinel is watching.")

        self.ser.timeout = 1
        return self.ser.read(size)




    def write(self, data):
        """
        Passes the given `data` (a `bytes` or `bytearray` object) to the
        `write()` method of the `Serial` object encapsulated by this `Sentinel`
        object. See `Serial.write()` for details.

        The write_timeout is hard-coded to be 3 seconds.
        """

        if self.is_watching:
            raise Exception("You cannot write when the sentinel is watching.")

        self.ser.write_timeout = 3
        return self.ser.write(data)





    def start_watch(self):
        """
        Sends a message to the `aux` thread to tell it to start watching for
        unexpected messages from `rover`. Blocks until confirmation is recieved
        from `aux` that the watch is actually started.
        """

        e = threading.Event()
        self.aux_queue.put((_start, e))
        e.wait()




    def stop_watch(self):
        """
        Sends a message to the `aux` thread to tell it to stop watching for
        unexpected messages from `rover`. Blocks until confirmation is recieved
        from `aux` that the watch is actually stopped.
        """

        e = threading.Event()
        self.aux_queue.put((_stop, e))
        e.wait()




    def _rover_error_handler(self, data):
        raise NotImplementedError




    def _watchman(self):
        """
        This function is run in the auxiliary thread to watch for unexpected
        messages sent from the `rover`. (It should never be run in the main
        execution thread.)

        This thread is put to sleep and awoken by messages sent from the main
        thread and passed via the `aux_queue`. Signals are only passed into
        this queue from the `start_watch()` and `stop_watch()` functions.

        If this thread receives either `ping` or `echo` messages from `rover`,
        then this is reported to `logger` and an appropriate response is
        returned to `rover`.

        If this thread receives an error message from `rover`, then this is
        reported to `logger`, and an `Exception` is raised.
        """

        logger.info("Thread started.")

        handlers = {
            False : self._not_watching_handler,
            True : self._watching_handler
        }
        
        while (True):
            # Check for queued messages, and update `is_watching` if needed:
            handlers[self.is_watching]()

            # Look for unexpected messages from `rover`, if currently watching:
            if self.is_watching:
                self._look()




    def _not_watching_handler(self):
        """ A helper function used by `_watchman` in the `aux` thread. """

        try:
            (sig, evt) = self.aux_queue.get(block = True, timeout = 0.2)

            if sig is self._start:
                self.is_watching = True  # Start watching.
            # If the message was anything else, then there is nothing to do.

            evt.set()

        except queue.Empty:
            # No message arrived in the queue, so just keep not watching.
            pass




    def _watching_handler(self):
        """ A helper function used by `_watchman` in the `aux` thread. """

        try:
            # Check to see if a `_stop` signal has arrived:
            (sig, evt) = self.aux_queue.get_nowait()

            if sig is self._stop:
                self.is_watching = False  # Stop watching:
            # If the message was anything else, then there is nothing to do.

            evt.set()
                
        except:
            # No message arrived in the queue, so just keep watching.
            pass





    def _look(self):
        """ A helper function used by `_watchman` in the `aux` thread to listen
        for a single message from the rover.

        While listening, the byte-wise timeout is 0.2 seconds.

        This function handles four distinct cases:

        - A ping message is sent from `rover`.
        - An echo message is sent from `rover`.
        - An invalid message (i.e. malformed) is sent from `rover`.
        - An error message is sent from `rover`.

        In all cases, the user is notified by messages to `stderr`. In the first
        two cases, an appropriate response message is sent to the `rover`.
        """

        self.ser.timeout = 0.2

        # Any start and stop control bytes are read into here:
        sig = bytearray(1)  

        # Any Message ID byte is read into here:
        mesg = bytearray(1)

        # Any deframed data bytes will be saved here:
        d = None

        # Indicates whether the message received was valid:
        valid = False

        # Read start signal byte.
        n = self.ser.readinto(sig)
        if n == 0:
            return  # No bytes were transmitted from `rover` before the timeout.
     
        if sig[0] == comm.Signal.start:
            self.logger.info("Receiving an unexpected message...")
            n = self.ser.readinto(mesg)  # Read Message ID
            mesg = int(mesg[0])  # Reuse the reference.
            if n == 1:
                if mesg in {comm.Message.error, comm.Message.echo}:
                    d = comm.read_data()
                n = self.ser.readinto(sig)  # Read stop signal byte.
                if n == 1 and sig[0] == comm.Signal.stop:
                    valid = True
     
        if valid:
            if mesg == comm.Message.ping:
                comm.tx_mesg(comm.Message.ping)
                self.logger.info("Received ping message. Responding to it...")
            elif mesg == comm.Message.echo:
                self.logger.info("Received echo message. Responding to it...")
                comm.tx_mesg(comm.Message.echo, data = d)
            elif mesg == comm.Message.error:
                self.logger.warning("Received error message. Handling it...")
                self._rover_error_handler(d)
        else:
            raise Exception("A malformed message was received from `rover`.")