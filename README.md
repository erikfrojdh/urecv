# urecv - A small receiver for a single jungfrau module

This is a project to build a small and simple receiver for a single JUNGFRAU module. 

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



 