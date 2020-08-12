from time import sleep
import serial
import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
plt.ion()
fig = plt.figure()

data=[0]
y=[]
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
    i=0
    num=string.find(' ')
    place=''
    while i<num:
        place+=string[i]
        i+=1
    lamda=0
    index=0
    for i in place[::-1]:
        lamda+=int(i)*10**index
        index+=1
    return(lamda)
def animate():
    xar=data
    yar=y
    ax1.clear()
    ax1.plot(xar,yar)
ser=serial.Serial('/dev/serial/by-id/usb-Silicon_Labs_CP2102_USB_to_UART_Bridge_Controller_0001-if00-port0',115200,timeout=1)
while 1:
    lam=str(ser.readline())
    kool=num(format(lam))
    data.append(kool)
    print(data)
    plt.plot(data)
    plt.ylabel('number')
    plt.show()
    plt.pause(0.01)
