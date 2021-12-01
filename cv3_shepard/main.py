import numpy as np
import cv2 as cv

from annoy import AnnoyIndex
from enum import Enum

import matplotlib.cm

from numpy.random import normal as norm

N = 150
width = 100 * 2
sc = 40
f_scale = 50
tree_count = 50
k = 5
max_rad = 50

cmap = matplotlib.cm.get_cmap('magma')

def gaussian(x, mu=0, sig=1):
    return np.exp(-np.power(x - mu, 2.) / (2 * np.power(sig, 2.)))


def f_(x, y):
    return gaussian(x, mu=0, sig=sc) * gaussian(y, mu=0, sig=sc) * 256

class Point:
    @classmethod
    def generate(cls, count, scale):
        index = AnnoyIndex(2, 'euclidean')
        points = []
        for i in range(count):
            p = Point(norm(scale=scale), norm(scale=scale))
            index.add_item(i, p.as_list())
            points.append(p)
        index.build(tree_count, -1)
        return points, index

    def __init__(self, x, y):
        self.x = x
        self.y = y

    def dist(self, p):
        return np.sqrt((self.x - p.x)**2 + (self.y - p.y)**2)

    def as_list(self):
        return [self.x, self.y]

    def get_mat_coords(self):
        return int(self.x) + width/2, int(self.y) + width/2

    def phi(self, p):
        return 1 / ((p.x - self.x)**2 + (p.y - self.y)**2)

    def f(self):
        return f_(self.x, self.y)

    def phi_FN(self, p, r_p):
        d = self.dist(p)
        top = max(0, r_p - d)
        bottom = r_p * d
        return (top/bottom)**2


def get_mat_value(points, x, y, index):
    x = float(x - width/2) + 0.5
    y = float(y - width/2) + 0.5
    upper_sum = 0.0
    lower_sum = 0.0

    neighbors_i = index.get_nns_by_vector([x, y], k)
    p = Point(x, y)
    r_p = points[neighbors_i[-1]].dist(p)
    if r_p > max_rad:
        return 0
    for i in neighbors_i:
        if (points[i].dist(p)) > max_rad:
            continue
        phi = points[i].phi_FN(Point(x, y), r_p)
        upper_sum += phi * points[i].f()
        lower_sum += phi
    return upper_sum / lower_sum


def get_original():
    result = np.zeros((width, width, 4), np.uint8)
    for x in range(result.shape[0]):
        for y in range(result.shape[1]):
            n_x = float(x - width / 2) + 0.5
            n_y = float(y - width / 2) + 0.5
            value = f_(n_x, n_y)
            print(value)
            f_color = cmap(int(value))
            result[x][y] = (255 * f_color[0], 255 * f_color[1], 255 * f_color[2], 255)
    return result

def shepard_Franke_Neilson():
    points, index = Point.generate(N, sc)
    result = np.zeros((width, width, 4), np.uint8)

    for x in range(result.shape[0]):
        for y in range(result.shape[1]):
            value = get_mat_value(points, x, y, index)
            print(value)
            f_color =  cmap(int(value))
            result[x][y] = (255 * f_color[0], 255 * f_color[1], 255 * f_color[2], 255)
    return result


cv.imshow("original", get_original())
cv.imshow("result", shepard_Franke_Neilson())
cv.waitKey(0)