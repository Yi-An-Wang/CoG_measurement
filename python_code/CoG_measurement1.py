import csv
import serial
import time

def main():
    file_path = "output.csv"
    
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
        else:
            values = line.split(",")
            if len(values) == 7:
                ax, ay, az = map(float, values[0:3])
                load1, load2, load3, load4 = map(float, values[3:7])
                data[ii] = [ax, ay, az, load1, load2, load3, load4]
                ii += 1
        
            if ii >= 10:
                with open(file_path, mode='w', newline='', encoding='utf-8') as file:
                    writer = csv.writer(file)
                    writer.writerow(["ax", "ay", "az", "load1", "load2", "load3", "load4"])
                    writer.writerows(data)
                print(f"Data has been written to {file_path}")
                break


