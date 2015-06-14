import struct
import os

length = 24000 # taken from uboot

data = struct.pack('II', length, 0x40200800)
data += open('start.bin', 'rb').read()
assert(len(data) < length)
data += bytes([0]) * (length - len(data))
open('MLO', 'wb+').write(data)

