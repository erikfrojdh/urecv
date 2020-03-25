#include "SimpleQueue.h"
#include "defs.h"
#include "utils.h"

class Receiver{
    size_t frame_queue_size_;
    SimpleQueue<Image> free_queue_;
    SimpleQueue<Image> data_queue_;
    std::byte *data_;
    void fillFreeQueue();
public:
    Receiver(size_t frame_queue_size = QUEUE_SIZE);
    ~Receiver();
    void ReceivePackets(const std::string &node, const std::string &port);
    void StreamImages(const std::string &endpoint);

};