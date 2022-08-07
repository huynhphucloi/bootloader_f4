import serial
import struct
import os

# Status Response
ACK = 0x11
NACK = 0x22

# Length Command
ERASE_COMMAND_LEN = 2
START_COMMAND_LEN = 6
WRITE_COMMAND_LEN = 2


def openPort(port):
    global ser
    try:
        ser = serial.Serial("COM4", 115200, timeout=2)
        print("Open port success!")
    except:
        print("Fail open port!!!")


def read_serial_port(length):
    read_value = ser.read(length)
    return read_value


def purge_serial_port():
    ser.reset_input_buffer()


def Write_to_serial_port(value, *length):
    data = struct.pack('>B', value)
    ser.write(data)


def erase_flash():
    data = []
    data.append(1)
    data.append(0x01)
    for i in data[0:]:
        Write_to_serial_port(i, 1)
    result = read_serial_port(2)
    print(result)
    if result[1] == 0x11:
        print("Erase success!")
        return
    print("Erase Error!")


def boot_to_ota():
    data = []
    data.append(1)
    data.append(0x05)
    for i in data[0:]:
        Write_to_serial_port(i, 1)
    result = read_serial_port(2)
    print(result)
    if result[1] == 0x11:
        print("Boot success!")
        return
    print("Boot Error!")


def read_binary_file():
    location = input("Enter the location file:")
    status = 0
    data = []
    for i in range(255):
        data.append(0)
    length_remain = os.path.getsize(location)
    data[1] = 0x04
    f = open(location, 'rb')
    print('file size: %s' % length_remain)
    while(length_remain):
        if(length_remain >= 128):
            length_read = 128
        else:
            length_read = length_remain

        data[0] = WRITE_COMMAND_LEN + length_read
        data[2] = length_read

        for i in range(length_read):
            binary_file = bytearray(f.read(1))
            data[3+i] = int(binary_file[0])

        Write_to_serial_port(data[0], 1)
        for i in data[1:int(data[0]+1)]:
            Write_to_serial_port(i, 1)
        result = read_serial_port(2)
        if result[1] == 0x22:
            print("Write Fail!")
            status = 0
            return
        else:
            print("Write success: %s byte\r\n" % length_read)
            status = 1
        length_remain -= length_read

    if status == 1:
        boot_to_ota()


def menu():
    print('--------------------------------\r\n')
    print('|          MENU                |\r\n')
    print("|   1. Erase OTA sector        |\r\n")
    print("|   2. Update OTA              |\r\n")
    print('--------------------------------\r\n')
    choose = input("Enter the option:")

    if choose == "1":
        erase_flash()
    elif choose == "2":
        read_binary_file()
    else:
        print("Select again!")


name = input("Enter the Port Name of your device(Ex: COM3):")
openPort(name)

while 1:
    menu()
