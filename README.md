# urecv - A small receiver for a single jungfrau module

This is a small and simple receiver for a single JUNGFRAU module. It's very much in alpha stage so use it at your own risk. Actually so far it has **only been tested with the virtual server**. 

Dependencies
--------------
* C++17 compiler (although this is more for exploring than needing specific features)
* libfmt
* libzmq

Features
-----------

* Small, currently ~325 sloc
* Receives UDP packets from Jungfrau 
* Streams out data over zmq 
* Sets reciver thread prioiory 
* Pin threads to specific core
* Includes lock free spsc queue (based on [folly/ProducerConsumerQueue](https://github.com/facebook/folly/blob/master/folly/ProducerConsumerQueue.h))



 
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

Roadmap
---------

Even though the idea of this project is exploratory rather than making production grade software there are a few things in the pipeline. 

 * Making thread pinning configurable 
 * Potentially add blocking push/pop to the queue


Bugs/Limitations
------------------
 
 * Only streams out the current frame after the first packet of the next frame has arrived. 