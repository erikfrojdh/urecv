

#include "DirectWriter.h"
#include "File.h"
#include "FwriteWriter.h"
#include "ImageView.h"
#include "defs.h"
#include "utils.h"

#include <chrono>
#include <cstdlib>
#include <fcntl.h>
#include <fmt/format.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // std::string fname{"test"};
    // size_t nFrames = 1000;
    // if ( argc ==2 || argc == 3 ){
    //     fname = argv[1];
    // }
    // if (argc == 3){
    //     nFrames = std::stoi(argv[2]);
    // }
    // fmt::print("Writing: {} frames to {}\n", nFrames, fname);
    // ImageView img;
    // //alternative to posix_memalign
    // // img.data = static_cast<std::byte*>(std::aligned_alloc(IO_ALIGNMENT,
    // FRAME_SIZE)); posix_memalign(&img.data, IO_ALIGNMENT, FRAME_SIZE);

    // File<FwriteWriter> f(fname, nFrames);

    // auto t0 = std::chrono::steady_clock::now();
    // for(int i = 0; i<nFrames; ++i){
    //     img.frameNumber = i;
    //     f.write(img);
    // }
    // std::chrono::duration<double, std::milli>
    // dt(std::chrono::steady_clock::now() - t0); auto mbps =
    // (FRAME_SIZE*nFrames/1024/1024)/(dt.count()/1000.); fmt::print("Write
    // took: {}ms, speed: {} MB/s\n", dt.count(), mbps); std::free(img.data);
}