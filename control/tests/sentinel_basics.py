# test.py - script with which test functionality with

import sentinel

sen = sentinel.Sentinel("/dev/tty.ElementSerial-ElementSe")
sen.ping()
sen.ping()

sen.stop_watch()
sen.write(b'\x01\x02\x06foobar\x06\x00\x02')  # prompts an echo
sen.write(b'\x01\x02\x0Cfoobarfoobar\x0C\x00\x02')  # prompts an echo
sen.start_watch()

#sen.echo("")
sen.echo("short_string")
sen.echo("start" + 100 * "foobar" + "end")  # Tests echo on a long string.
