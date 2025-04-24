import csv
import serial
import time

def main():
    file_path = "output.csv"
    with open(file_path, mode='w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        writer.writerow(["ax", "ay", "az", "load1", "load2", "load3", "load4"])
    
    data = [None] * 10
    
    ser = serial.Serial('COM5', 115200)
    time.sleep(2)

    ser.write(b"Start\n")
    print("Start command sent to the device.")

    ii = 0
    while True:
        line = ser.readline().decode('utf-8').strip()
        print(line)
        
        if line == "End":
            print("End command received. Stopping data collection.")
            break

        elif line == "Start":
            print("Start command received. Continuing data collection.")
            continue    
        else:
            values = line.split(",")
            if len(values) == 7:
                ax, ay, az = map(float, values[0:3])
                load1, load2, load3, load4 = map(float, values[3:7])
                data[ii] = [ax, ay, az, load1, load2, load3, load4]
                ii += 1
        
    with open(file_path, mode='a', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        writer.writerows(data[:ii])

def test():
    file_path = "output.csv"
    data = [[1, 2, 3, 4, 5, 6, 7], [8, 9, 10, 11, 12, 13, 14],[15, 16, 17, 18, 19, 20, 21]]

    with open(file_path, mode='w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        writer.writerow(["ax", "ay", "az", "load1", "load2", "load3", "load4"])
    
    with open(file_path, mode='a', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        writer.writerows(data[:2])
       
if __name__ == "__main__":
    test()