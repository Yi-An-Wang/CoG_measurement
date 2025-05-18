import serial
import time
from collections import deque
import matplotlib.pyplot as plt
import math
import csv

def get_MCU_data():
    rec_len = 30
    recorded_data = [deque([0]*rec_len, maxlen=rec_len), deque([0]*rec_len, maxlen=rec_len), deque([0]*rec_len, maxlen=rec_len), deque([0]*rec_len, maxlen=rec_len), # force 1~4
                     deque([0]*rec_len, maxlen=rec_len), deque([0]*rec_len, maxlen=rec_len)] # pitch & roll
    count_num = 0
    print(f'\r[{"█"*count_num}{" "*(rec_len-count_num)}] {count_num}/{rec_len}', end='')

    ser = serial.Serial('COM3', 115200)
    N = 100
    t_data = deque([0]*N, maxlen=N)

    raw_weight_1 = deque([0]*N, maxlen=N)
    raw_weight_2 = deque([0]*N, maxlen=N)
    raw_weight_3 = deque([0]*N, maxlen=N)
    raw_weight_4 = deque([0]*N, maxlen=N)
    est_weight_1 = deque([0]*N, maxlen=N)
    est_weight_2 = deque([0]*N, maxlen=N)
    est_weight_3 = deque([0]*N, maxlen=N)
    est_weight_4 = deque([0]*N, maxlen=N)

    raw_pitch_data = deque([0]*N, maxlen=N)
    raw_roll_data = deque([0]*N, maxlen=N)
    est_pitch_data = deque([0]*N, maxlen=N)
    est_roll_data = deque([0]*N, maxlen=N)

    fig_Forces = plt.figure()
    axis_1 = fig_Forces.add_subplot(221)
    axis_2 = fig_Forces.add_subplot(222)
    axis_3 = fig_Forces.add_subplot(223)
    axis_4 = fig_Forces.add_subplot(224)
    axis_1.grid(True)
    axis_2.grid(True)
    axis_3.grid(True)
    axis_4.grid(True)
    line_raw_F1, = axis_1.plot(t_data, raw_weight_1, label="raw_F1(Kg)")
    line_est_F1, = axis_1.plot(t_data, est_weight_1, label="est_F1(Kg)")
    line_raw_F2, = axis_2.plot(t_data, raw_weight_2, label="raw_F2(Kg)")
    line_est_F2, = axis_2.plot(t_data, est_weight_2, label="est_F2(Kg)")
    line_raw_F3, = axis_3.plot(t_data, raw_weight_3, label="raw_F3(Kg)")
    line_est_F3, = axis_3.plot(t_data, est_weight_3, label="est_F3(Kg)")
    line_raw_F4, = axis_4.plot(t_data, raw_weight_4, label="raw_F4(Kg)")
    line_est_F4, = axis_4.plot(t_data, est_weight_4, label="est_F4(Kg)")
    axis_1.legend()
    axis_2.legend()
    axis_3.legend()
    axis_4.legend()

    fig_Angle = plt.figure()
    axis_pitch = fig_Angle.add_subplot(221)
    axis_roll = fig_Angle.add_subplot(222)
    axis_pitch.grid(True)
    axis_roll.grid(True)
    polar_pitch = fig_Angle.add_subplot(223, polar=True)
    polar_roll = fig_Angle.add_subplot(224, polar=True)
    line_raw_pitch, = axis_pitch.plot(t_data, raw_pitch_data, label="raw_pitch")
    line_raw_roll, = axis_roll.plot(t_data, raw_roll_data, label="raw_roll")
    line_est_pitch, = axis_pitch.plot(t_data, est_pitch_data, label="est_pitch")
    line_est_roll, = axis_roll.plot(t_data, est_roll_data, label="est_roll")
    axis_pitch.legend()
    axis_roll.legend()

    pitch_line, = polar_pitch.plot([0, math.pi], [2, 2], color="red")
    polar_roll.plot([0, math.pi], [2, 2], color="orange", alpha=0.5, linewidth=5)
    roll_line, = polar_roll.plot([0, math.pi], [2, 2], color="red")

    t_pre = time.time()
    t = 0.0
    plt.ion()
    while True:
        line = ser.readline().decode('utf-8').strip()
        # print(line)
        values = line.split(",")
        raw_F1, raw_F2, raw_F3, raw_F4 = map(float, values[0:4])
        est_F1, est_F2, est_F3, est_F4 = map(float, values[4:8])
        raw_pitch, raw_roll, est_pitch, est_roll = map(float, values[8:12])
        reset, record, break_python = map(int, values[12:15])

        t_now = time.time()
        t = t + t_now - t_pre
        t_data.append(t)
        t_pre = t_now

        raw_weight_1.append(raw_F1)
        raw_weight_2.append(raw_F2)
        raw_weight_3.append(raw_F3)
        raw_weight_4.append(raw_F4)
        est_weight_1.append(est_F1)
        est_weight_2.append(est_F2)
        est_weight_3.append(est_F3)
        est_weight_4.append(est_F4)

        raw_pitch_data.append(raw_pitch * 180 / math.pi)
        raw_roll_data.append(raw_roll * 180 / math.pi)
        est_pitch_data.append(est_pitch * 180 / math.pi)
        est_roll_data.append(est_roll * 180 / math.pi)

        line_raw_F1.set_data(t_data, raw_weight_1)
        line_est_F1.set_data(t_data, est_weight_1)
        line_raw_F2.set_data(t_data, raw_weight_2)
        line_est_F2.set_data(t_data, est_weight_2)
        line_raw_F3.set_data(t_data, raw_weight_3)
        line_est_F3.set_data(t_data, est_weight_3)
        line_raw_F4.set_data(t_data, raw_weight_4)
        line_est_F4.set_data(t_data, est_weight_4)

        line_raw_pitch.set_data(t_data, raw_pitch_data)
        line_raw_roll.set_data(t_data, raw_roll_data)
        line_est_pitch.set_data(t_data, est_pitch_data)
        line_est_roll.set_data(t_data, est_roll_data)

        pitch_line.set_data([est_pitch, math.pi+est_pitch], [2, 2])
        roll_line.set_data([est_roll, math.pi+est_roll], [2, 2])

        axis_1.relim()
        axis_1.autoscale_view()
        axis_2.relim()
        axis_2.autoscale_view()
        axis_3.relim()
        axis_3.autoscale_view()
        axis_4.relim()
        axis_4.autoscale_view()

        axis_pitch.relim()
        axis_pitch.autoscale_view()
        axis_roll.relim()
        axis_roll.autoscale_view()

        plt.pause(0.01)

        if reset == 1:
            print("Reset tare!!")
            with open("CoG_data.csv", "a+", newline='') as csvfile:
                writer = csv.writer(csvfile)
                writer.writerows(recorded_data)
        if record == 1:
            recorded_data[0].append(est_F1)
            recorded_data[1].append(est_F2)
            recorded_data[2].append(est_F3)
            recorded_data[3].append(est_F4)
            recorded_data[4].append(est_pitch)
            recorded_data[5].append(est_roll)
            count_num += 1
            if count_num > rec_len:
                count_num = rec_len
            print(f'\r[{"█"*count_num}{" "*(rec_len-count_num)}] {count_num}/{rec_len}', end='')
        if break_python == 1:
            break

    plt.ioff()

if __name__ == "__main__":
    get_MCU_data()