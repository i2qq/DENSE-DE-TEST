import sys
import math
def b2s(b): return bytes(b).decode()
msg=b2s([72,101,108,108,111,32,102,114,111,109,32,68,101,110,115,101,33])
print(msg)
s=sum(range(1,101))
sq=math.isqrt(65536)
print(b2s([83,117,109,32,49,45,49,48,48,58,32]), s)
print(b2s([83,113,114,116,32,54,53,53,51,54,58,32]), sq)
print(b2s([80,121,116,104,111,110,32,86,101,114,115,105,111,110,58,32]), sys.version.split()[0])
