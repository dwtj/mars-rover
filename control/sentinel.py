# sentinel.py - Defines the sentinel class.

import threading
import queue
import logging

import serial

from codes import Signal, MesgID, SubsysID




DEFAULT_SERIAL_PORT = "/dev/tty.ElementSerial-ElementSe"
DATA_FRAME_MAX_LEN = 100




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
        while self.ser.read() != b'':
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




    def tx_mesg(self, mesg_id, subsys_id=None, command_id=None, data=None):
        """ Sends a message to the rover as described by the arguments.

        - `mesg_id` must be the `MesgID` of the message to be sent.
        - `subsys_id` must be `None` or a `SubsysID`. This is the value to be
          sent in the subsystem ID byte. If this is `None`, then no such byte
          will be sent in this message.
        - `command_id` is either `None` or an int that is a valid command code
          (of the subsystem identified by `subsys_id`). This is the value to be
          sent in the command ID byte. If this is `None`, then no such byte
          will be sent in this message. Note that `command_id` is `None` if and
          only if `subsys_id` is `None`. 
        - `data` must be either `None` or a `bytes` object. This is the data to
          be sent in the sequence of data frames. In the case that this is
          `None`, no data frames will be sent.

        The message being sent will illicit a subsequent response message (of
        the same type). Reading this response should be handled by `rx_mesg()`.

        You should expect that no checks of correctness are performed on the
        args. If you pass garbage to this function, garbage may well be sent
        to the rover.
        """

        if self.is_watching:
            mesg = "You cannot send a message when the sentinel is watching."
            raise Exception(mesg)

        # Some validation for function arguments:
        if type(mesg_id) != MesgID:
            raise Exception("The `mesg_id` is not of type `MesgID`.")
        if subsys_id != None and type(subsys_id) != SubsysID:
            raise Exception("The `subsys_id` is not of type `SubsysID`.")

        iff_mesg = "`subsys_id` is None if and only if `command_id` is None."
        if subsys_id == None and command_id != None:
            raise Exception(iff_mesg)
        if subsys_id != None and command_id == None:
            raise Exception(iff_mesg)

        self.write_int(Signal.start)
        self.write_int(mesg_id)

        if subsys_id is not None:
            self.write_int(subsys_id)
            self.write_int(command_id)
            if data is not None:
                self.write_frames(data)

        self.write_int(Signal.stop)




    def rx_mesg(self, mesg_id, subsys_id=None, command_id=None, has_data=False):
        """ Recieves a message from the rover, and expects it to have the
        format specified by the given arguments.

        - `mesg_id` must be a `MesgID`. This is the expected message type of
          the message being received.
        - `subsys_id` must be `None` or a `SubsysID`. This is the expected
          Subsystem ID of the message being received. If this is `None`, then
          no Subsystem ID byte is expected.
        - `command_id` must be `None` or an int that is a valid command code of
          the given `subsys_id`. This is the expected value of the Command ID
          byte of the message being received. Note that `command_id` is `None`
          if and only if `subsys_id` is `None`. 
        - `has_data` must be a boolean indicating whether or not a sequence of
          data frames is expected in the message being received.

        This function returns any data that was included with the response
        message as a `bytes` object.

        If there isn't a timely response message, or if the response has
        unexpected features, an `Exception` is raised.
        """

        if self.read_int() != Signal.start:
            raise Exception("Did not receive the expected Start Signal.")

        if self.read_int() != t:
            raise Exception("Did not receive the expected Message ID.")

        if (subsys_id != None) and (self.ser.read_int() != subsys_id):
            raise Exception("Did not receive the expected Subsystem ID.")

        if (command_id != None) and (self.ser.read_int() != command_id):
            raise Exception("Did not receive the expected Command ID.")

        if has_data:
            rv = read_data()

        if self.read_int() != Signal.stop:
            raise Exception("Did not receive the expected Stop Signal.")




    def read(self, size = 1):
        """
        Reads and returns the `bytes` object (of the given `size`) from the
        `read()` method of the `Serial` object encapsulated by this `Sentinel`
        object. See `Serial.read()` for details.
        
        The `timeout` is hard-coded to be 1 second.
        """

        if self.is_watching:
            raise Exception("You cannot read when the sentinel is watching.")

        self.ser.timeout = 1
        return self.ser.read(size)




    def readinto(self, b):
        """
        Reads from the encapsulated `Serial` object into `b` (a `bytearray`
        object) using `Serial.readinto()`. Returns the number of bytes read
        into `b`.

        The `timeout` is hard-coded to be 1 second.
        """

        if self.is_watching:
            raise Exception("You cannot read when the sentinel is watching.")

        self.ser.timeout = 1
        return self.ser.readinto(b)

 


    def read_int(self):
        """
        A simple wrapper for `Sentinel.read()` that reads a single byte from
        the encapsulated `Serial` object and returns it as an unsigned integer.
        If there was nothing to read from the serial connection, then `None` is
        returned.

        The `timeout` is hard-coded to be 1 second.
        """

        # TODO: Find a much more efficient way!

        if self.is_watching:
            raise Exception("You cannot read when the sentinel is watching.")

        b = bytearray(1)
        n = readinto(b)
        return b[0] if n == 1 else None




    def write(self, data):
        """
        Passes the given `data` (a `bytes` or `bytearray` object) to the
        `write()` method of the `Serial` object encapsulated by this `Sentinel`
        object. See `Serial.write()` for details.

        The `write_timeout` is hard-coded to be 3 seconds.
        """

        if self.is_watching:
            raise Exception("You cannot write when the sentinel is watching.")

        self.ser.write_timeout = 3
        return self.ser.write(data)




    def write_int(self, i):
        """
        A simple wrapper for `Sentinel.write()` that writes a single `int` in
        the range [0..255] to the encapsulated `Serial` object.

        The `write_timeout` is hard-coded to be 3 seconds.
        """

        # TODO: Find a much more efficient way!

        if self.is_watching:
            raise Exception("You cannot write when the sentinel is watching.")

        if type(i) != int or i < 0 or i > 255:
            raise Exception("The argument is not an 8-bit unsigned integer.")

        b = bytearray(1)
        b[0] = i
        self.ser.write(b)




    def write_frames(self, data):
        """ Takes the given data, frames it, and sends it across the serial
        connection to the rover. """
        self.write(seninel._frame_data(data))




    def read_frames(self):
        """ Reads a sequence of data frames from the serial connection and
        returns the data bytes (without the framing bytes). """
        
        # A list of data frames without the three framing bytes (i.e. the
        # "length", "real length", and "more data" bytes):
        frames = []
        
        # In each iteration, read a data frame and add it to the `frames` list
        # until there are no more data frames to be read.
        more_frames = True
        while more_frames:
            frame_length = self.ser.read_int()
            frame = bytearray(frame_length)
            if self.ser.readinto(frame) != frame_length:
                raise Exception("Did not receive enough data bytes.")
            
            # Add only the good bytes to de-framed data:
            num_good_bytes = self.ser.read_int()
            frames.append(frame[0 : num_good_bytes])
            
            # Check to see if we should keep going:
            more_frames = False if self.ser.read() == b'\x00' else True
            
        return b''.join(frames)



    def start_watch(self):
        """
        Sends a message to the `aux` thread to tell it to start watching for
        unexpected messages from `rover`. Blocks until confirmation is recieved
        from `aux` that the watch is actually started.
        """

        self.logger.debug("Sending a message to start watching the connection.")
        e = threading.Event()
        self.aux_queue.put((_start, e))
        e.wait()




    def stop_watch(self):
        """
        Sends a message to the `aux` thread to tell it to stop watching for
        unexpected messages from `rover`. Blocks until confirmation is recieved
        from `aux` that the watch is actually stopped.
        """

        self.logger.debug("Sending a message to stop watching the connection.")
        e = threading.Event()
        self.aux_queue.put((_stop, e))
        e.wait()




    def ping(self):
        """ Sends a `ping` message to `rover` and expects a `ping` message in
        response. """

        self.stop_watch()

        self.logger.info("Sending a `ping` message to `rover`.")
        self.tx_mesg(MesgID.ping)
        self.rx_mesg(MesgID.ping)
        self.logger.info("Received a `ping` message from `rover`.")

        self.start_watch()




    def echo(self, s):
        """
        Sends a message of type `echo` along with the supplied string `s`.
        Expects an identical message to be returned, i.e., a message of type
        `echo` along with this same string `s`.
        """

        self.stop_watch()

        self.logger.info("Sending an `echo` message to `rover`.")
        sent_data = bytes(s, 'utf-8')

        tx_mesg(MesgID.echo, data = sent_data)
        returned_data = rx_mesg(MesgID.echo, data = True)

        if sent_data != returned_data:
            raise Exception("A different string was returned from the rover.")
        self.logger.info("Received correct `echo` message from the `rover`.")

        self.start_watch()




    def printout(self, stream):
        """ Indefinitely write to `stdout` data coming from the `rover`. """

        buf = bytearray(512)

        while True:
            n = self.readinto(b)
            s = buf[0:n].decode('utf-8')
            stream.write(s)
            time.sleep(0.1)




    def heartbeat(self):
        """ Sends ping and receives ping signals to the rover indefinitely. """

        while True:
            self.ping()
            sys.stdout.write('.')
            time.sleep(1)




    def seed_rng(self, seed):
        """ Sends the given `seed` to the `rover` to seed its psuedo-random
        number generator. """

        raise NotImplementedError
        # TODO




    def _rover_error_handler(self, data):
        """ Interprets the data segment of an error sent from the `rover` and
        handles it appropriately. """

        raise NotImplementedError




    def _frame_data(d):
        """ A (pure) helper function that takes the bytes object `d` and
        returns a `bytes` object with added data framing bytes. """

        # The number of full frames to be sent:
        full_frames = len(d) // DATA_FRAME_MAX_LEN

        # The final partially-full frame to be sent:
        partial_frame_len = len(d) % DATA_FRAME_MAX_LEN
        has_partial_frame = False if partial_frame_len == 0 else True

        frames = []
        for i in range(full_frames):
            # `start` is index of first byte of `d` to be copied to this `frame`:
            start = i * DATA_FRAME_MAX_LEN
            frame = bytearray(DATA_FRAME_MAX_LEN + 3)

            # Add data and framing info to this `frame`:
            frame[0] = DATA_FRAME_MAX_LEN
            frame[1:1+DATA_FRAME_MAX_LEN] = d[start:start+DATA_FRAME_MAX_LEN]
            frame[DATA_FRAME_MAX_LEN + 1] = DATA_FRAME_MAX_LEN
            frame[DATA_FRAME_MAX_LEN + 2] = True

            # Add this newly formed frame to the list of frames to be sent.
            frames.append(frame)

        if has_partial_frame:
            start = full_frames * DATA_FRAME_MAX_LEN
            frame = bytearray(partial_frame_len + 3)

            frame[0] = partial_frame_len
            frame[1 : 1 + partial_frame_len] = d[start : ]
        
        # Set the last byte of the last frame (whether full or partial) to
        # indicate that there are no more frames:
        frames[-1][-1] = False

        return b''.join(frames)




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
     
        if sig[0] == Signal.start:
            self.logger.info("Receiving an unexpected message...")
            n = self.ser.readinto(mesg)  # Read Message ID
            mesg = int(mesg[0])  # Reuse the reference.
            if n == 1:
                if mesg in {MesgID.error, MesgID.echo}:
                    d = comm.read_data()
                n = self.ser.readinto(sig)  # Read stop signal byte.
                if n == 1 and sig[0] == Signal.stop:
                    valid = True
     
        if valid:
            if mesg == MesgID.ping:
                self.tx_mesg(MesgID.ping)
                self.logger.info("Received ping message. Responding to it...")
            elif mesg == MesgID.echo:
                self.logger.info("Received echo message. Responding to it...")
                self.tx_mesg(MesgID.echo, data = d)
            elif mesg == MesgID.error:
                self.logger.warning("Received error message. Handling it...")
                self._rover_error_handler(d)
        else:
            raise Exception("A malformed message was received from `rover`.")
