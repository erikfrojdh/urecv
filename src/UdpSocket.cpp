#include "UdpSocket.h"
#include <cstring> //memset
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

UdpSocket::UdpSocket(const std::string &node, const std::string &port,
                     int packet_size)
    : packet_size(packet_size) {
    // open socket
    struct addrinfo hints {};
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    struct addrinfo *res = nullptr;
    if (getaddrinfo(node.c_str(), port.c_str(), &hints, &res))
        throw std::runtime_error("Get addrinfo failed");
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
        throw std::runtime_error("Failed to open socket\n");
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        close(sockfd);
        throw std::runtime_error("failed to bind socket\n");
    }
    freeaddrinfo(res);
}

UdpSocket::~UdpSocket() {
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
}

void UdpSocket::ReceivePacket(void *dst, PacketHeader &header) {
    while (recvfrom(sockfd, dst, packet_size, 0, nullptr, nullptr) !=
           packet_size)
        ;
    memcpy(&header, dst, sizeof(header));
}

void UdpSocket::setBufferSize(size_t size) {
    socklen_t optlen = sizeof(size);
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, optlen)) {
        throw std::runtime_error("Could not set UDP socket buffer size");
    }
}

size_t UdpSocket::bufferSize() const {
    uint64_t size = 0;
    socklen_t optlen = sizeof(uint64_t);
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, &optlen);
    return size/2;
}
