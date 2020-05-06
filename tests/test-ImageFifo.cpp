
#include "ImageFifo.h"
#include "ImageView.h"
#include <catch2/catch.hpp>
#include <numeric>
using ur::ImageFifo;
using ur::ImageView;
TEST_CASE("Construct an ImageFifo") {
    constexpr size_t img_size = 8000;
    ImageFifo fifo(100, img_size);

    REQUIRE(fifo.size() == 100);
    REQUIRE(fifo.numFreeSlots() == 100);
    REQUIRE(fifo.numFilledSlots() == 0);
    REQUIRE(fifo.image_size() == img_size);
    const auto size = fifo.image_size();

    SECTION("Pop one push one") {
        ImageView img = fifo.pop_free();
        REQUIRE(img.frameNumber == -1);
        img.frameNumber = 17;
        std::fill_n(img.data, size, '0');
        fifo.push_image(img);
        ImageView img2 = fifo.pop_image();
        REQUIRE(img2.frameNumber == 17);
    }

    SECTION("Touch all memory to see if sanitizer fires") {
        for (size_t i = 0; i < fifo.size(); ++i) {
            ImageView img = fifo.pop_free();
            img.frameNumber = i;
            std::fill_n(img.data, size, char(2));
            fifo.push_image(img);
        }

        for (size_t i = 0; i < fifo.size(); ++i) {
            ImageView img = fifo.pop_image();
            auto sum = std::accumulate(img.data, img.data + size, 0U);
            REQUIRE(sum == 16000);
            REQUIRE(img.frameNumber == i);
            fifo.push_free(img);
        }

        REQUIRE(fifo.numFilledSlots() == 0);
        REQUIRE(fifo.numFreeSlots() == 100);
    }
}
