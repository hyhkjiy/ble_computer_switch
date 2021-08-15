# A BLE switch for computer

Gatttool example in the raspberry pi:

```python
import pexpect
import struct
import time
import sys

IMU_MAC_ADDRESS = "<address>"
HANDLE = "0x0010"

if __name__ == '__main__':
    gatt = pexpect.spawn("gatttool -t random -b " + IMU_MAC_ADDRESS + " -I")
    gatt.sendline("connect")
    gatt.expect("Connection successful")

    gatt.sendline("char-write-cmd 10 01")
    gatt.sendline("disconnect")
```