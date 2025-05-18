import csv
from collections import deque
import time

def main():
    # test_data = deque([[12, 3, 55, 1, 0], [12.1, 0.299, 55.12, 1, 0]])
    test_data = [[1000]*30, [110]*30, [200]*30, [210]*30, [0.001]*30, [10.221]*30]

    start_time = time.time()
    with open("datas.csv", "a+", newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerows(test_data)
    end_time = time.time()
    print(f"total time consume: {end_time - start_time}")

def main2():
    with open("datas.csv", 'r', encoding='utf-8') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            print(row)

if __name__ == "__main__":
    main()