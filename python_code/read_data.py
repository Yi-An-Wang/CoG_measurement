import serial
import time
from collections import deque
import matplotlib.pyplot as plt

def main():
    ser = serial.Serial('COM3', 115200)
    while True:
        line = ser.readline().decode('utf-8').strip()
        print(line)

def read_hx711():
    ser =serial.Serial('COM3', 115200)
    N = 100
    t_data = deque([0]*N, maxlen=N)
    estimated_weight = deque([0]*N, maxlen=N)
    raw_weight = deque([0]*N, maxlen=N)
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.grid(True)
    line_estimated_weight, = ax.plot(t_data, estimated_weight, label="estimated_weight")
    line_raw_weight, = ax.plot(t_data, raw_weight, label="raw_weight")
    ax.legend()

    t_pre = time.time()
    t = 0.0
    plt.ion()
    while True:
        line = ser.readline().decode('utf-8').strip()
        print("values:", line)
        values = line.split(",")
        estimated_weight_value = float(values[0])
        raw_weight_value = float(values[1])

        t_now = time.time()
        t = t + t_now - t_pre
        t_data.append(t)
        t_pre = t_now

        estimated_weight.append(estimated_weight_value)
        raw_weight.append(raw_weight_value)
        line_estimated_weight.set_data(t_data, estimated_weight)
        line_raw_weight.set_data(t_data, raw_weight)
        ax.relim()
        ax.autoscale_view()
        plt.pause(0.01)

def read_MMA8451():
    ser = serial.Serial('COM3', 115200)
    N = 100
    t_data = deque([0]*N, maxlen=N)
    ax_data = deque([0]*N, maxlen=N)
    ay_data = deque([0]*N, maxlen=N)
    az_data = deque([0]*N, maxlen=N)
    est_ax_data = deque([0]*N, maxlen=N)
    est_ay_data = deque([0]*N, maxlen=N)
    est_az_data = deque([0]*N, maxlen=N)

    fig = plt.figure()
    axis_x = fig.add_subplot(311)
    axis_y = fig.add_subplot(312)
    axis_z = fig.add_subplot(313)
    axis_x.grid(True)
    axis_y.grid(True)
    axis_z.grid(True)
    line_ax, = axis_x.plot(t_data, ax_data, label="acc_x")
    line_ay, = axis_y.plot(t_data, ay_data, label="acc_y") 
    line_az, = axis_z.plot(t_data, az_data, label="acc_z")
    line_est_ax, = axis_x.plot(t_data, est_ax_data, label="est_acc_x")
    line_est_ay, = axis_y.plot(t_data, est_ay_data, label="est_acc_y")
    line_est_az, = axis_z.plot(t_data, est_az_data, label="est_acc_z")
    axis_x.legend()
    axis_y.legend()
    axis_z.legend()

    t_pre = time.time()
    t = 0.0
    plt.ion()
    while True:
        line = ser.readline().decode('utf-8').strip()
        print(line)
        values = line.split(",")
        ax, ay, az = map(float, values[0:3])
        est_ax, est_ay, est_az = map(float, values[3:6])
        t_now = time.time()
        t = t + t_now - t_pre
        t_data.append(t)
        t_pre = t_now
        ax_data.append(ax)
        ay_data.append(ay)
        az_data.append(az)
        est_ax_data.append(est_ax)
        est_ay_data.append(est_ay)
        est_az_data.append(est_az)

        line_ax.set_data(t_data, ax_data)
        line_ay.set_data(t_data, ay_data)
        line_az.set_data(t_data, az_data)
        line_est_ax.set_data(t_data, est_ax_data)
        line_est_ay.set_data(t_data, est_ay_data)
        line_est_az.set_data(t_data, est_az_data)

        axis_x.relim()
        axis_x.autoscale_view()
        axis_y.relim()
        axis_y.autoscale_view()
        axis_z.relim()
        axis_z.autoscale_view()

        plt.pause(0.01)

if __name__ == "__main__":
    # main()
    # read_hx711()
    read_MMA8451()