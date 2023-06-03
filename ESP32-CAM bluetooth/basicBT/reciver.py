

from operator import le
import serial
ser = serial.Serial('/dev/rfcomm0') 

while True:
    length = str(ser.readline())
    #print(type(length))
    length = length[2:-5]
    print(length)
    if length == "start":break

size  = int(ser.readline())
img_buff = ser.read(size)


img_f = open("imagen.jpg","wb")
img_f.write(img_buff)
img_f.close()