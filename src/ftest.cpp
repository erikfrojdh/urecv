

#include "FileWriterDirect.h"
#include "defs.h"
#include "utils.h"

#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <chrono>
#include <fmt/format.h>

int main(){
    Image img;
    //alternative to posix_memalign
    img.data = static_cast<std::byte*>(std::aligned_alloc(IO_ALIGNMENT, FRAME_SIZE));

    FileWriterDirect f("test");
    auto t0 = std::chrono::steady_clock::now();
    for(int i = 0; i<500; ++i){
        img.frameNumber = i;
        f.write(img);
    }
    std::chrono::duration<double, std::milli> dt(std::chrono::steady_clock::now() - t0);
    auto mbps = (FRAME_SIZE*500/1024/1024)/(dt.count()/1000.);
    fmt::print("Write took: {}ms, speed: {} MB/s\n", dt.count(), mbps);
    std::free(img.data);
    
}