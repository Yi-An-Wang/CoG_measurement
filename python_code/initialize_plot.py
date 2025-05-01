import serial
import time
from collections import deque
import matplotlib.pyplot as plt

def main():
    print("Initializing serial communication...")

    time.sleep(2)  # Wait for the serial connection to establish
    ser = serial.Serial('COM6', 115200)

    N = 100
    t_data = deque([0]*N, maxlen=N)
    F1_data = deque([0]*N, maxlen=N)
    F2_data = deque([0]*N, maxlen=N)
    F3_data = deque([0]*N, maxlen=N)
    F4_data = deque([0]*N, maxlen=N)
    ax_data = deque([0]*N, maxlen=N)
    ay_data = deque([0]*N, maxlen=N)
    az_data = deque([0]*N, maxlen=N)

    fig = plt.figure()
    ax_F = fig.add_subplot(211)
    ax_a = fig.add_subplot(212)
    ax_F.grid(True)
    ax_a.grid(True)
    line_F1, = ax_F.plot(t_data, F1_data, label="F1")
    line_F2, = ax_F.plot(t_data, F2_data, label="F2")
    line_F3, = ax_F.plot(t_data, F3_data, label="F3")
    line_F4, = ax_F.plot(t_data, F4_data, label="F4")
    line_ax, = ax_a.plot(t_data, ax_data, label="acc_x")
    line_ay, = ax_a.plot(t_data, ay_data, label="acc_y")
    line_az, = ax_a.plot(t_data, az_data, label="acc_z")
    ax_F.legend()
    ax_a.legend()

    t_pre = time.time()
    t = 0.0
    plt.ion()
    while True:
        line = ser.readline().decode('utf-8').strip()
        print(line)
        values = line.split(",")
        F1, F2, F3, F4 = map(float, values[0:4])
        ax, ay, az = map(float, values[4:7])
        t_now = time.time()
        t = t + t_now-t_pre
        t_data.append(t)
        t_pre = t_now
        F1_data.append(F1)
        F2_data.append(F2)
        F3_data.append(F3)
        F4_data.append(F4)
        ax_data.append(ax)
        ay_data.append(ay)
        az_data.append(az)

        line_F1.set_ydata(F1_data)
        line_F2.set_ydata(F2_data)
        line_F3.set_ydata(F3_data)
        line_F4.set_ydata(F4_data)
        line_ax.set_ydata(ax_data)
        line_ay.set_ydata(ay_data)
        line_az.set_ydata(az_data)

        plt.pause(0.01)
    plt.ioff()

if __name__ == "__main__":
        main()