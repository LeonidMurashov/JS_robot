import serial
import getkeys
ser = serial.Serial('COM3', 9600, timeout=2)

# Citation: Box Of Hats (https://github.com/Box-Of-Hats )

import win32api as wapi
import time

from win32con import VK_MENU

keyList = ["\b"]
for char in "ABCDEFGHIJKLMNOPQRSTUVWXYZ":
    keyList.append(char)

def key_check():
    keys = []
    for key in keyList:
        if wapi.GetAsyncKeyState(ord(key)):
            keys.append(key)

    if wapi.GetAsyncKeyState(VK_MENU):
        keys.append("ALT")
    return keys
 


while True:
	keys = key_check()
	ser.reset_input_buffer()
	if len(keys) > 0:
		if 'W' in keys:
			n = 0
		elif 'A' in keys:
			n = 1
		elif 'D' in keys:
			n = 2
		elif 'S' in keys:
			n = 3
		elif 'Q' in keys:
			n = 4
		elif 'E' in keys:
			n = 5
		elif 'R' in keys:
			n = 6
		ser.write(bytearray(str(n), 'utf-8'))
	time.sleep(0.1)