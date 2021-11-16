import cv2 as cv
import numpy as np

width = 500
x_count = 16
max_shift = (width / (x_count - 1)) / 5
scale = 50.0

def f(x, y):
    return np.sin(x) * np.cos(y)

def f_(x, y):
    return ((f(float(x)/scale, float(y)/scale) + 1) / 2.0) * 256

class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def shift_point(self):
        if self.x != 0 and self.x != width:
            self.x += int(np.random.uniform(-max_shift, max_shift))
        if self.y != 0 and self.y != width:
            self.y += int(np.random.uniform(-max_shift, max_shift))

    def val(self):
        return f_(self.x, self.y)

    @classmethod
    def generate_points(cls):
        points = []
        for x in np.linspace(0, width, x_count):
            for y in np.linspace(0, width, x_count):
                p = Point(x, y)
                p.shift_point()
                points.append(p)
        return points

    def __mul__(self, other):
        return Point(self.x * other, self.y * other)

    def __add__(self, other):
        return Point(self.x + other.x, self.y + other.y)

    def __sub__(self, other):
        return Point(self.x - other.x, self.y - other.y)

    def get_vec_len(self):
        return np.sqrt(self.x**2 + self.y**2)

    def as_np_arr(self):
        return np.array([self.x, self.y])


class Tetragon:
    bases = [
        lambda r, s: (1 - r) * (1 - s),
        lambda r, s: r * (1 - s),
        lambda r, s: r * s,
        lambda r, s: (1 - r) * s,
    ]

    def __init__(self, p1, p2, p3, p4):
        #self.points = [p1, p2, p4, p3]
        self.points = [p1, p2, p4, p3]

    def draw(self, img, color):
        pts = np.array([[[int(p.x), int(p.y)] for p in self.points]])
        cv.fillPoly(img, pts, color)
       # for p in self.points:
        #    cv.circle(img, (int(p.x), int(p.y)), 3, 128, -1)

    def val(self, x, y):
        r, s = tetragon.t_inv(y, x)
        return sum([self.points[i].val() * self.bases[i](r, s) for i in range(4)])
        #return self.points[0].val()

    @classmethod
    def generate_tetragons(cls):
        tetragons = []
        seg_map = np.zeros([width, width], dtype=np.ubyte)

        points = Point.generate_points()
        for x in range(x_count - 1):
            for y in range(x_count - 1):
                i = x + (y * x_count)
                p1 = points[i]
                p2 = points[i + 1]
                p3 = points[i + x_count]
                p4 = points[i + x_count + 1]
                tetragon = Tetragon(p1, p2, p3, p4)
                tetragon.draw(seg_map, len(tetragons))
                tetragons.append(tetragon)
        return tetragons, seg_map

    def t(self, r, s):
        return sum([p * Tetragon.bases[i](r, s) for i, p in enumerate(self.points)], Point(0.0, 0.0))
        #return (self.points[0]*(1-r) + self.points[1]*r) * (1-s) + (self.points[2]*r + self.points[3]*(1-r)) * s

    def t_dr(self, r, s, p):
        return (s - 1) * (p[0] - p[1]) + s * (p[2] - p[3])

    def t_ds(self, r, s, p):
        return (r - 1) * (p[0] - p[3]) + r * (p[2] - p[1])

    def get_J(self, r, s):
        p_x = [p.x for p in self.points]
        p_y = [p.y for p in self.points]
        return np.array([[self.t_dr(r, s, p_x), self.t_ds(r, s, p_x)], [self.t_dr(r, s, p_y), self.t_ds(r, s, p_y)]])

    def t_inv(self, x, y):          # r, s náleží <0, 1>
        p = Point(x, y)
        rs = np.array([0.5, 0.5])
        point_dif = self.t(rs[0], rs[1]) - p

        while point_dif.get_vec_len() > 0.1:
            J = self.get_J(rs[0], rs[1])
            J_inv = np.linalg.pinv(J)

            rs -= J_inv.dot(point_dif.as_np_arr())
            point_dif = self.t(rs[0], rs[1]) - p
        return rs[0], rs[1]


template = np.zeros([width, width], dtype=np.uint8)
for x in range(template.shape[0]):
    for y in range(template.shape[1]):
        template[y][x] = f_(x, y)
cv.imshow("template", template)

#t = Tetragon(Point(1.0, 0.0), Point(3.0, 0.25), Point(0.0, 3.5), Point(4.0, 3.0))
#print(t.t_inv(1.8, 2.7))


tetragons, seg_map = Tetragon.generate_tetragons()


result = np.zeros([width, width], dtype=np.uint8)

'''for t in tetragons:
    centre = t.t(0.5, 0.5)
    print(t.t_inv(centre.x, centre.y))
    cv.circle(seg_map, (int(centre.x), int(centre.y)), 3, 128, -1)
    centre = t.t(0.3, 0.3)
    print(t.t_inv(centre.x, centre.y))
    cv.circle(seg_map, (int(centre.x), int(centre.y)), 3, 128, -1)
    centre = t.t(0.8, 0.3)
    print(t.t_inv(centre.x, centre.y))
    cv.circle(seg_map, (int(centre.x), int(centre.y)), 3, 128, -1)
    centre = t.t(0.3, 0.8)
    print(t.t_inv(centre.x, centre.y))
    cv.circle(seg_map, (int(centre.x), int(centre.y)), 3, 128, -1)
    centre = t.t(0.8, 0.8)
    print(t.t_inv(centre.x, centre.y))
    cv.circle(seg_map, (int(centre.x), int(centre.y)), 3, 128, -1)
'''

for x in range(result.shape[0]):
    for y in range(result.shape[1]):
        #print(int(seg_map[y][x]))
        tetragon = tetragons[int(seg_map[y][x])]
        result[y][x] = tetragon.val(y, x)
        #r, s = tetragon.t_inv(x, y)
        #print('     %d'%y)
    print(x)
cv.imshow("seg map", seg_map)
cv.imshow("result", result)

cv.waitKey(0)
