# test.py - script with which test functionality with

import sentinel

sen = sentinel.Sentinel("/dev/tty.ElementSerial-ElementSe-17")
sen.ping()
sen.echo("")
sen.echo("short_string")
sen.echo("start" + 100 * "foobar" + "end")  # Tests echo on a long string.
