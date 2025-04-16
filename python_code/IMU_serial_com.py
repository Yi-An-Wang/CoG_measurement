import serial
import time
from collections import deque
import matplotlib.pyplot as plt

def main():
    ser = serial.Serial('COM5', 115200)

    N = 100
    t_data = deque([0]*N, maxlen=N)
    ax_data = deque([0]*N, maxlen=N)
    ay_data = deque([0]*N, maxlen=N)
    az_data = deque([0]*N, maxlen=N)
    gx_data = deque([0]*N, maxlen=N)
    gy_data = deque([0]*N, maxlen=N)
    gz_data = deque([0]*N, maxlen=N)
    mx_data = deque([0]*N, maxlen=N)
    my_data = deque([0]*N, maxlen=N)
    mz_data = deque([0]*N, maxlen=N)

    fig = plt.figure()
    ax_a = fig.add_subplot(311)
    ax_g = fig.add_subplot(312)
    ax_m = fig.add_subplot(313)
    ax_a.grid(True)
    ax_g.grid(True)
    ax_m.grid(True)
    line_ax, = ax_a.plot(t_data, ax_data, label="acc_x")
    line_ay, = ax_a.plot(t_data, ay_data, label="acc_y")
    line_az, = ax_a.plot(t_data, az_data, label="acc_z")
    line_gx, = ax_g.plot(t_data, gx_data, label="gyr_x")
    line_gy, = ax_g.plot(t_data, gy_data, label="gry_y")
    line_gz, = ax_g.plot(t_data, gz_data, label="gry_z")
    line_mx, = ax_m.plot(t_data, mx_data, label="mag_x")
    line_my, = ax_m.plot(t_data, my_data, label="mag_y")
    line_mz, = ax_m.plot(t_data, mz_data, label="mag_z")
    ax_a.legend()
    ax_g.legend()
    ax_m.legend()

    t_pre = time.time()
    t = 0.0
    plt.ion()
    while True:
        line = ser.readline().decode('utf-8').strip()
        print(line)
        values = line.split(",")
        ax, ay, az = map(float, values[1:4])
        gx, gy, gz = map(float, values[4:7])
        mx, my, mz = map(float, values[7:10])
        t_now = time.time()
        t = t + t_now-t_pre
        t_data.append(t)
        t_pre = t_now
        ax_data.append(ax)
        ay_data.append(ay)
        az_data.append(az)
        gx_data.append(gx)
        gy_data.append(gy)
        gz_data.append(gz)
        mx_data.append(mx)
        my_data.append(my)
        mz_data.append(mz)
        # print(t_data[98], t_data[99])

        line_ax.set_data(t_data, ax_data)
        line_ay.set_data(t_data, ay_data)
        line_az.set_data(t_data, az_data)
        line_gx.set_data(t_data, gx_data)
        line_gy.set_data(t_data, gy_data)
        line_gz.set_data(t_data, gz_data)
        line_mx.set_data(t_data, mx_data)
        line_my.set_data(t_data, my_data)
        line_mz.set_data(t_data, mz_data)
        plt.draw()
        ax_a.relim()
        ax_a.autoscale_view()
        ax_g.relim()
        ax_g.autoscale_view()
        ax_m.relim()
        ax_m.autoscale_view()
        plt.pause(0.01)

if __name__ == "__main__":
    main()