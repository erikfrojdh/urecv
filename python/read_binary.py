import argparse
import numpy as np
import matplotlib.pyplot as plt
plt.ion()

n_rows = 1024
n_cols = 1024

parser = argparse.ArgumentParser()
parser.add_argument("fname")
args = parser.parse_args()


with open(args.fname, 'rb') as f:
    #read footer
    f.seek(-16, 2) #Last two ints
    # n_frames = np.fromfile(f, dtype = np.int64, count = 1)[0]
    # footer_size = np.fromfile(f, dtype = np.int64, count = 1)[0]
    n_frames, footer_size = np.fromfile(f, dtype=np.int64, count = 2)
    print(f"footer_size: {footer_size}")
    print(f"n_frames: {n_frames}")
    f.seek(-footer_size, 2)
    frame_nr = np.fromfile(f, dtype = np.int64, count = n_frames)

    #back to front of file and read data
    print(f'Reading: {n_frames} images')
    f.seek(0)
    data = np.fromfile(f, dtype = np.uint16, count = n_rows*n_cols*n_frames).reshape(n_frames, n_rows, n_cols)
    
fig, ax = plt.subplots(1,1)
im = ax.imshow(data[5])


