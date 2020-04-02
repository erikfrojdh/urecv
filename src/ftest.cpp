

#include "File.h"
#include "DirectWriter.h"
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

    size_t nFrames = 10000;
    File<DirectWriter> f("test", nFrames);
    
    auto t0 = std::chrono::steady_clock::now();
    for(int i = 0; i<nFrames; ++i){
        img.frameNumber = i;
        f.write(img);
    }
    std::chrono::duration<double, std::milli> dt(std::chrono::steady_clock::now() - t0);
    auto mbps = (FRAME_SIZE*nFrames/1024/1024)/(dt.count()/1000.);
    fmt::print("Write took: {}ms, speed: {} MB/s\n", dt.count(), mbps);
    std::free(img.data);
    
}