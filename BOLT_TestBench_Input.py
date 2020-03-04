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
if ser.is_open():
# ****************** CSV Opening ****************** #
    while True:
        input_filename = input("Provide a .csv file (ex. input.csv):\t")
        if os.path.isfile('filename.txt'):
            input_file = open(input_filename, 'r')
            break
# ****************** CSV Opening ****************** #

# ****************** CSV Reading & TXT Writing ****************** #
    for line in input_file:
        line_data = line.split(",")
        output_file = open("BOLT_TestBench_Output.txt", 'wt')
        for data in line_data:
            ser.write(bytes(data))
            output_file.write(data + "\n")
        output = ser.read_until("\n").decode("utf-8")
        output_file.write(output + "\n")
        output_file.close()
# ****************** CSV Reading & TXT Writing ****************** #

# ****************** Reset Loop ****************** #
        while True:
            reset_key = input("type reset to reset:\t")
            if reset_key == "reset":
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
BS1 -> Break Switch 1
BS2 -> Break Switch 2
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
TS
TR

"""

print(ser)
