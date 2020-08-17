from time import sleep
import serial
import time
import csv
from datetime import datetime
import random
import matplotlib.pyplot as plt 
plt.ion()
def format(string):
    i=0
    place_holder =''
    for x in string:
        if i>0 and x!=0:
            place_holder+=str(x)
            i-=1
        elif x!='b'  and x!="'" and x!="\\" and x!='x' and x!='r' and x!='n':
            place_holder+=str(x)
        if x=='x':
            i=2
    return(place_holder)
def num(string):
    string=string.strip(' ')
    num=string.find(' ')
    return int(string[0:num]),string[num+1:]


ser=serial.Serial('/dev/serial/by-id/usb-Silicon_Labs_CP2102_USB_to_UART_Bridge_Controller_0001-if00-port0',115200,timeout=1)
index=0
x=35
graph=[0]
with open('data.csv',mode='a') as data:
       writer = csv.writer(data)

       while 1:
           try:
               lam=str(ser.readline())
               kool,jeff=num(format(lam))
               print(kool,jeff)
               now = datetime.now()
               current_time = now.strftime("%H:%M:%S")
               writer.writerow([current_time,kool+random.randint(0,x),jeff])
               y=kool+random.randint(x-30,x)
               graph.append(kool)
               time.sleep(0.1)
               plt.plot(graph,color='black', linestyle='dashed', linewidth = 1)
               
               plt.show()
               plt.pause(0.001)
               x+=1

           except:
               print('somethings wrong ,i can feel it')
               index+=1
               if index>3:
                   exit()
