#pragma once
#include "defs.h"
#include <string>
class UdpSocket{
    int sockfd = -1;
    int packet_size;
public:
    UdpSocket(const std::string& node, const std::string& port, int packet_size);
    ~UdpSocket();
    bool receivePacket(void *dst, PacketHeader& header);
    void setBufferSize(size_t size);
    size_t bufferSize() const;
    void shutdown();
};