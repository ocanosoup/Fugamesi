import serial
import sys
filename = ""
step = float(250)
if(len(sys.argv)>2):
    step = int(sys.argv[2])
if(len(sys.argv)>1):
    filename = sys.argv[1]
    f = open(filename, 'w')
rhy = ""
ser = serial.Serial('/dev/ttyACM0', 115200)
line = None
if(ser.isOpen()):
    line=ser.readline()
while(line is not None and ser.isOpen()):
    try:
        out = int(line)
    except ValueError:
        break
    while(out > 1.5*step):
        rhy+="m"
        out = out-step
    if(.5*step <= out <= 1.5*step):
        rhy+="on"
    elif(0 <= out <= .5*step):
        rhy+="n"
    print(rhy)
    if(ser.isOpen()):
        line = ser.readline()
while(rhy.endswith('m')):
    rhy = rhy[:-1]
if(rhy.endswith('n')):
    rhy += 'o'
rhy = rhy.replace("non","ndn")
rhy = rhy.replace("nm", "no")
rhy = str(len(rhy)) + rhy
ser.write(rhy.encode())
if(len(filename)>1):
    f.write(rhy)
    f.close()

