#include "SimpleQueue.h"
#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <future>
#include <thread>

TEST_CASE("Basic operations") {
    SimpleQueue<int> queue(3);

    // push three ints
    REQUIRE(queue.sizeGuess() == 0);
    int val = 5;
    REQUIRE(queue.push(val) == true);
    REQUIRE(queue.sizeGuess() == 1);
    val = 8;
    REQUIRE(queue.push(val) == true);
    REQUIRE(queue.sizeGuess() == 2);
    val = 12;
    REQUIRE(queue.push(val) == true);
    REQUIRE(queue.sizeGuess() == 3);
    val = 123;
    REQUIRE(queue.push(val) == false); // full
    REQUIRE(queue.sizeGuess() == 3);

    // pop the same ints
    int rval = 0;
    REQUIRE(queue.pop(rval) == true);
    REQUIRE(rval == 5);
    REQUIRE(queue.sizeGuess() == 2);
    REQUIRE(queue.pop(rval) == true);
    REQUIRE(rval == 8);
    REQUIRE(queue.sizeGuess() == 1);
    REQUIRE(queue.pop(rval) == true);
    REQUIRE(rval == 12);
    REQUIRE(queue.sizeGuess() == 0);
    REQUIRE(queue.pop(rval) == false); // empty
    REQUIRE(rval == 12);
    REQUIRE(queue.sizeGuess() == 0);
}

void produce(SimpleQueue<int> &q, int n) {
    int val = 0;
    for (int i = 0; i < n; ++i) {
        val = i;
        while (!q.push(val))
            ;
    }
}
int consume(SimpleQueue<int> &q, int n, int &sum) {
    int val = 0;
    for (int i = 0; i < n; ++i) {
        while (!q.pop(val))
            ;
        sum += val;
    }
    return sum;
}

TEST_CASE("Threads") {
    int n = 1000;
    int sum = 0;
    SimpleQueue<int> queue(5);
    std::thread t1{&produce, std::ref(queue), n + 1};
    std::thread t2{&consume, std::ref(queue), n + 1, std::ref(sum)};
    t1.join();
    t2.join();

    REQUIRE(sum == n * (n + 1) / 2);
}