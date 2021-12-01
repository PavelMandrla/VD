import numpy as np
import csv
from datetime import datetime

def get_points():
    points = []
    path = '../cv1/data_processed.csv'
    with open(path) as f:
        reader = csv.reader(f, delimiter=",")
        for i, line in enumerate(reader):
            date = datetime.strptime(line[0], '%Y-%m-%d')
            #print(date.timetuple().tm_yday)
            points.append(((date.year - 2020) * 366 + date.timetuple().tm_yday, float(line[1])))
            #print('line[{}] = {}'.format(i, line)
    return points

print(get_points())

#points = [(1,1), (2,2), (3,2), (4,1)]
points = get_points()
n = 4

def getA(points, n):
    result = []
    for x in range(n)[::-1]:
        row = []
        for y in range(n)[::-1]:
            row.append(sum([p_x ** (x + y) for p_x, p_y in points]))
        result.append(row)
    return np.array(result)

def getV(points, n):
    result = []
    for i in range(n)[::-1]:
        result.append(sum([(x**i)* y for x, y in points]))
    return np.transpose(np.array(result))

A = getA(points, n)
v = getV(points, n)

A_inv = np.linalg.inv(A)
print(A)
print(v)
print(A_inv.dot(v))