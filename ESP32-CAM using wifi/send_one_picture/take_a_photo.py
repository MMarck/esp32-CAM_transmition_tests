#this program write on serial and recive a picture

import serial
import time
import datetime

serial_bt = serial.Serial('/dev/rfcomm0') 
start_time_total = datetime.datetime.now()

serial_bt.write(b'x')


time.sleep( 1 )
start_time_recive = datetime.datetime.now()
img_buff =b''
while True:
    serial_bt.write(b'c')

    data = str(serial_bt.readline())
    data = data[2:-5]
    print(data)
    if data == "START":
        size  = int(serial_bt.readline())
        print("Recived " + str(size) +" bits")
        img_buff += serial_bt.read(size)
        print("img buffer len: " + str(len(img_buff)) + " bits")
        # print(img_buff)
        
    if data == "END":
        img_f = open("picture.jpg","wb")
        img_f.write(img_buff)
        img_f.close()
        
        break

end_time_total = datetime.datetime.now()
total = (end_time_total - start_time_recive)
trans = (end_time_total - start_time_total)

print("Transmition time: ", total.seconds, total.microseconds)
print("Total time: ", trans.seconds, trans.microseconds)
