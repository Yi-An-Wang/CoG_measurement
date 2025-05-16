import csv
from collections import deque

def main():
    test_data = deque([[12, 3, 55, 1, 0], [12.1, 0.299, 55.12, 1, 0]])

    with open("datas.csv", "a+", newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerows(test_data)

if __name__ == "__main__":
    main()