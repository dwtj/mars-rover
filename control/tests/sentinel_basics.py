# test.py - script with which test functionality with

import sentinel

sen = sentinel.Sentinel("/dev/tty.ElementSerial-ElementSe")
sen.ping()
sen.ping()

sen.write(b'\x01\x02\x06foobar\x06\x00\x02')  # prompts an echo
sen.write(b'\x01\x02\x0Cfoobarfoobar\x0C\x00\x02')  # prompts an echo
sen.write(b'\x01\x02\x0a' + b'0123456789' + b'\x0a\x01' + b'\x01A\x01\x00\x02') # prompts an echo with two frames

sen.echo("")
sen.echo("short_string")
sen.echo("start" + 100 * "foobar" + "end")  # Tests echo on a long string.
