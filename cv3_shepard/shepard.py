#!/usr/bin/env python3
# -*- coding: utf-8 -*-

__author__    = "Tomas Fabian"
__copyright__ = "(c)2021 VSB-TUO, FEECS, Dept. of Computer Science"
__email__     = "tomas.fabian@vsb.cz"
__version__   = "0.1.0"                                                               

import numpy as np
import matplotlib.pyplot as plt

"""
pip install numpy
pip install matplotlib
"""


def main():
  # data sampled in (non-regular) rectangular grid
  '''
  (X[i+1, j], Y[i+1, j])     (X[i+1, j+1], Y[i+1, j+1])
                       +-----+
                       |     |
                       +-----+
      (X[i, j], Y[i, j])     (X[i, j+1], Y[i, j+1])
  '''
  N = 5  
  x = np.linspace(0.0, 1.0, num=N)
  y = np.expand_dims(np.linspace(1.0, 1.0, num=N), 1)   
  X = x * y  
  Y = np.transpose(X)
  # TODO replace C with the results of Shepard's interpolation of your own dataset
  C = np.random.normal(0.0, 1.0, size=(N, N))
  print(C)  

  # create a figure containing a single or more axes.
  fig, ax = plt.subplots(1, 1, figsize=(5, 5))
  
  ax.set_title("scattered point interpolation")
  ax.set_xlabel("x (-)")
  ax.set_ylabel("y (-)")
  ax.set(xlim=(0.0, 1.0), ylim=(0.0, 1.0))
  ax.set_xticks(x)  
  ax.set_yticks(x)
  ax.set_aspect('equal')  
  plt.pcolormesh(X, Y, C, cmap='viridis', shading="nearest")
  
  plt.colorbar()
  plt.grid(True, which="major", axis="both", color="lightgray", linestyle="dashed", linewidth=1)
  
  plt.show()
  

if __name__ == "__main__":    
  main()
