#pragma once
#include "sls_detector_defs.h"
using PacketHeader = slsDetectorDefs::sls_detector_header;
constexpr size_t nrow = 512;
constexpr size_t ncol = 1024;
constexpr int payload_size = 8192;
constexpr ssize_t packet_size = sizeof(PacketHeader) + payload_size;
constexpr size_t frame_size = nrow*ncol*sizeof(uint16_t);