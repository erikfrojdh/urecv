#pragma once
#include <atomic>
constexpr std::size_t hardware_destructive_interference_size = 128;

template <typename T> class SimpleQueue {
    //Benchmark and investigate effects of padding
    char pad0_[hardware_destructive_interference_size];
    alignas(hardware_destructive_interference_size) std::atomic<uint32_t> readIndex{0};
    alignas(hardware_destructive_interference_size) std::atomic<uint32_t> writeIndex{0};
    std::size_t size;
    T *records;
  public:
    SimpleQueue(uint32_t qsize) : size(qsize+1), records(new T[size]) {}
    ~SimpleQueue() { delete[] records; }

    std::size_t  sizeGuess() const {
        int ret = writeIndex.load(std::memory_order_acquire) -
                  readIndex.load(std::memory_order_acquire);
        if (ret < 0) {
            ret += size;
        }
        return ret;
    }

    bool push(T &element) {
        auto const currentWrite = writeIndex.load(std::memory_order_relaxed);
        auto nextRecord = currentWrite + 1;
        if (nextRecord == size) {
            nextRecord = 0;
        }
        if (nextRecord != writeIndex.load(std::memory_order_acquire)) {
            records[currentWrite] = element;
            writeIndex.store(nextRecord, std::memory_order_release);
            return true;
        }
        return false;
    }

    bool pop(T &record) {
        auto const currentRead = readIndex.load(std::memory_order_relaxed);
        if (currentRead == writeIndex.load(std::memory_order_acquire)) {
            return false;
        }
        auto nextRecord = currentRead + 1;
        if (nextRecord == size) {
            nextRecord = 0;
        }
        record = records[currentRead];
        readIndex.store(nextRecord, std::memory_order_release);
        return true;
    }
};