import serial
import time

ser = serial.Serial('/dev/ttyACM0', 115200, timeout = 1)
time.sleep(2)

while True:
    command = input("command: ") #v, w, p, I
    value = input("value: ")
    value = int(value)
    value = list(value.to_bytes(2,'big',signed = True))
    packet = bytearray([ord('s'),1,ord(command)]+value+[ord('e')])
    print ("sending: ", packet, "...")
    ser.write(packet)
    rec = ser.read(100)
    print(rec)