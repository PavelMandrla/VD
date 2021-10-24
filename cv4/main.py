import cv2 as cv
import numpy as np
from matplotlib import pyplot as plt
import matplotlib.cm


def load_ymls(dir):
    result = []
    for i in range(1000):
        filename = "%s/u%s.yml" % (dir, f"{i:05d}")
        fs = cv.FileStorage(filename, cv.FILE_STORAGE_READ)
        result.append(fs.getNode("flow").mat())
    return result

def K1(x, v, dt=1):
    return v(x) * dt

def K2(x, v, dt=1):
    return v(x + K1(x, v, dt)) * dt

def runge_kutta(x, v):
    return x + (K1(x, v) + K2(x, v)) / 2

cmap = matplotlib.cm.get_cmap('magma')
mats = load_ymls("./data")
print(len(mats))
for mat in mats:
    print(mat.shape)


    #laplacian = cv.Laplacian(mat, cv.CV_64F)
    sobel_x = cv.Sobel(mat[:, :, 0],cv.CV_64F,1,0,ksize=3)
    sobel_y = cv.Sobel(mat[:, :, 1],cv.CV_64F,1,0,ksize=3)

    #divergence = sobel_x.copy()
    result = np.zeros((sobel_x.shape[0], sobel_x.shape[1], 4), np.uint8)
    for x in range(sobel_x.shape[0]):
        for y in range(sobel_x.shape[1]):
            f_color = cmap((sobel_x[x, y] + sobel_y[x, y])/2.0)
            result[x][y] = (255 * f_color[0], 255 * f_color[1], 255 * f_color[2], 255)
           #divergence[x, y] += sobel_y[x, y]
            #divergence[x, y] /= 2.0

    #print(sobel_x.shape)
    cv.imshow('sobel_x', sobel_x)
    cv.imshow('sobel_y', sobel_y)
    cv.imshow('divergence', result)
    '''
    plt.subplot(2, 2, 1), plt.imshow(map, cmap='gray')
    plt.title('Original'), plt.xticks([]), plt.yticks([])
    plt.subplot(2, 2, 2), plt.imshow(laplacian, cmap='gray')
    plt.title('Laplacian'), plt.xticks([]), plt.yticks([])
    plt.subplot(2, 2, 3), plt.imshow(sobel_x, cmap='gray')
    plt.title('Sobel X'), plt.xticks([]), plt.yticks([])
    plt.subplot(2, 2, 4), plt.imshow(sobel_y, cmap='gray')
    plt.title('Sobel Y'), plt.xticks([]), plt.yticks([])
    '''
    cv.waitKey(20)