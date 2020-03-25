import zmq
import numpy as np
import matplotlib.pyplot as plt
plt.ion()
bitmask = np.array([0x3FFF], dtype=np.uint16)
context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.connect("tcp://localhost:4545")
socket.setsockopt(zmq.SUBSCRIBE, b"")

n_frames = 10

frame_nr = np.zeros(n_frames)
data = np.zeros((n_frames, 512, 1024))
caught_frames = 0

while(caught_frames < n_frames):
    buf = socket.recv()
    if len(buf):
        frame_nr[caught_frames] = np.frombuffer(buf, dtype=np.int64)[0]
        buf = socket.recv()
        data[caught_frames] = np.frombuffer(buf, dtype  = np.uint16).reshape(512, 1024)
        caught_frames += 1





# socket.disconnect()
# context.destroy()