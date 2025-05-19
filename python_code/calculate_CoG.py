import csv
import numpy as np
import matplotlib.pyplot as plt

def main():
    d = 516
    w = 226
    F1_data = []
    F2_data = []
    F3_data = []
    F4_data = []
    theta_data = []
    phi_data = []
    row_num = 0

    with open("CoG_data.csv", 'r', encoding='utf-8') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            if row_num == 0:
                F1_data.extend([float(x) for x in row])
                row_num +=1
            elif row_num == 1:
                F2_data.extend([float(x) for x in row])
                row_num +=1
            elif row_num == 2:
                F3_data.extend([float(x) for x in row])
                row_num +=1
            elif row_num == 3:
                F4_data.extend([float(x) for x in row])
                row_num +=1
            elif row_num == 4:
                theta_data.extend([float(x) for x in row])
                row_num +=1
            else:
                phi_data.extend([float(x) for x in row])
                row_num = 0

    F_R = np.array(F1_data) + np.array(F4_data)
    F_F = np.array(F2_data) + np.array(F3_data)
    Cos_theta = np.cos(np.array(theta_data))
    Sin_theta = np.sin(np.array(theta_data))
    ratio = np.array([(F_F * d * Cos_theta) / (F_R + F_F)]).T
    A_matrix = np.vstack((Cos_theta, Sin_theta)).T
    l_h = np.linalg.solve(A_matrix.T @ A_matrix, A_matrix.T @ ratio)
    print(l_h)

    # F_r = np.array(F1_data) + np.array(F2_data)
    # F_l = np.array(F3_data) + np.array(F4_data)
    # diff_F = np.array([F_l - F_r]).T
    # b_vector = w * np.array([F_l]).T
    # s = diff_F.T @ b_vector / (diff_F.T @ diff_F)
    # print(s)

    fig1 = plt.figure()
    ax1 = fig1.add_subplot(111)
    ax1.grid(True)
    ax1.set_xlabel("pitch angle(rad)")
    ax1.set_ylabel("Force (Kg)")
    ax1.scatter(theta_data, F_F, color="orange", label="Force(front)")
    ax1.scatter(theta_data, F_R, color="purple", label="Force(rear)")
    ax1.legend()

    fig2 = plt.figure()
    ax2 = fig2.add_subplot(111)
    ax2.grid(True)
    ax2.set_xlabel("pitch angle(rad)")
    ax2.set_ylabel("ratio")
    ax2.scatter(theta_data, ratio[:,0], label="F_r*d*cos(theta)/(F_R+F_L)")
    ax2.legend()
    plt.show()

if __name__ == "__main__":
    main()