# urecv - A small receiver for a single jungfrau module

This is a project to build a small and simple receiver for a single JUNGFRAU module. It's very much in alpha stage so use it at your own risk. 

Dependencies
--------------
* C++17 compiler (although this is more for exploring than needing specific features)
* libfmt
* libzmq

Features
-----------

* Receives UDP packets from Jungfrau 
* Streams out data over zmq 
* Sets reciver thread prioiory 
* Pin threads to specific core



 
Build
-----------------

```bash
    git clone https://github.com/erikfrojdh/urecv.git
    mkdir build && cd build
    cmake ../urecv
    make -j4

```

Usage
-----------

```bash

    #Listen to udp packets on 193.168.1.29 port 50001
    ./urecv 192.168.1.29:50001 

    #optional zmq endpoint
    ./urecv 192.168.1.29:50001 tcp://*:4545
    ./urecv 192.168.1.29:50001 ipc:///tmp/987

```