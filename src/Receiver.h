#include "SimpleQueue.h"
#include "UdpSocket.h"
#include "defs.h"
#include "utils.h"
#include <memory>

class Receiver {
    size_t frame_queue_size_;
    SimpleQueue<Image> free_queue_;
    SimpleQueue<Image> data_queue_;
    std::byte *data_;
    std::atomic<bool> stopped_{false};
    std::atomic<bool> receiver_done_{false}; // watch out for cache lines
    std::unique_ptr<UdpSocket> sock;
    void fillFreeQueue();

  public:
    Receiver(const std::string &node, const std::string &port,
             size_t frame_queue_size = QUEUE_SIZE);
    ~Receiver();
    void receivePackets(int cpu);
    void streamImages(const std::string &endpoint);
    void writeImages(const std::string &basename);
    void finish();
};
