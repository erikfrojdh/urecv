import numpy as np
import matplotlib.pyplot as plt

plt.ion()

with open("run_d0_f0_1.raw", "rb") as f:
    header = np.fromfile(f, dtype = np.uint8, count = 112)
    data = np.fromfile(f, dtype = np.uint16, count = 512*1024).reshape(512, 1024)


with open("test_0.bin", "rb") as f:
    data2 = np.fromfile(f, dtype = np.uint16, count = 512*1024).reshape(512, 1024)