import numpy as np

def load_points():
    data = '../cv1/'

points = [(1,1), (2,2), (3,2), (4,1)]
n = 3



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