import serial

def main():
    ser = serial.Serial('COM6', 115200)
    while True:
        line = ser.readline().decode('utf-8').strip()
        print(line)

if __name__ == "__main__":
    main()