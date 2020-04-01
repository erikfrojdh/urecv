# urecv - A small receiver for a single jungfrau module

This is a small and simple receiver for a single JUNGFRAU module. It's very much in alpha stage so use it at your own risk. It supports streaming out data on zmq or writing to disk.

Dependencies
--------------
* C++17 compiler (although this is more for exploring than needing specific features)
* libfmt
* libzmq

Features
-----------

* Small, currently ~530 sloc
* Receives UDP packets from Jungfrau 
* Streams out data over zmq or writes to file
* Sets receiver thread priority 
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
#This receives frames put them in a buffer and later zero out
#use
./urecv 192.168.1.29:50001 

#Stream out data over zmq
./urecv 192.168.1.29:50001 -s tcp://*:4545
./urecv 192.168.1.29:50001 -s ipc:///tmp/987

#Write data to file writes to basename_0.bin basename_1.bin etc
./urecv 192.168.1.29:50001 -w basename

```

Roadmap
---------

Even though the idea of this project is exploratory rather than making production grade software there are a few things in the pipeline. 

 * Making thread pinning configurable 
 * Tests, tests, tests...


Bugs/Limitations
------------------
 
 * Only streams out the current frame after the first packet of the next frame has arrived. 