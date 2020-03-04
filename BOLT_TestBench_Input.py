import serial
import sys
import csv
import os

# ****************** Open Serial Port ****************** #
ser = serial.Serial()
ser.baudrate = 115200  # int(input("Provide a Baudrate (ex. 9600, 115200, etc.):\t"))
ser.port = 'COM' + (input("Provide a COM Port (ex. 6, 7, 8, etc.):\t"))
# ****************** Open Serial Port ****************** #

"""
operating under the assumption that:
byte size is 8
parity is off
stop bit is 1
baud rate is 115200
"""
if ser.is_open:
# ****************** CSV Opening ****************** #
    while True:
        input_filename = input("Provide a .csv file (ex. input.csv):\t")
        if os.path.isfile('filename.txt'):
            input_file = open(input_filename, 'r')
            break
# ****************** CSV Opening ****************** #

# ****************** CSV Reading & TXT Writing ****************** #
    with open("BOLT_TestBench_Output.txt", "w") as output_file:
        for line in input_file.read():
            line_data = line.split(",")
            for data in line_data:
                ser.write(bytes(data))
                output_file.write(data + "\n")
            output = ser.read_until("\n").decode("utf-8")
            output_file.write(output + "\n")
            output_file.close()
# ****************** CSV Reading & TXT Writing ****************** #

# ****************** Reset Loop ****************** #
        while True: #We may not need this
            input("Press enter to continue to next test.....")
            break
# ****************** Reset Loop ****************** #

    input_file.close()
ser.close()

"""
Basis for README.txt...

Shorthand:
FW Sp -> Front Wheel Speed
BW Sp -> Back Wheel Speed
FW Sus -> Front Wheel Suspension
BW Sus -> Back Wheel Suspension
BS1 -> Brake Switch 1
BS2 -> Brake Switch 2
PS1 -> Profile Switch 1
PS2 -> Profile Switch 2
PS3 -> Profile Switch 3
TS -> Throttle Switch
TR -> Throttle Request

The structure of .csv input is as follows per row:
FW Sp, BW Sp, FW Sus, BW Sus, BS1, BS2, TS

The structure of .txt output is as follows per line:
FW Sp
BW Sp
FW Sus
BW Sus
BS1
BS2
PS1
PS2
PS3
TS
TR

"""

print(ser)
