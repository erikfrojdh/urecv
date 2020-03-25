#include "defs.h"
#include <string>
class UdpSocket{
    int sockfd = -1;
    ssize_t packet_size;
public:
    UdpSocket(const std::string& node, const std::string& port, size_t packet_size);
    ~UdpSocket();
    void ReceivePacket(void *dst, PacketHeader& header);

};