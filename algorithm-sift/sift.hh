#pragma once

#include "api.hh"
#include "sift_global.hh"

extern "C"
{
    const ApiAlgorithm* SIFT_EXPORT getApi();
    const LibraryInfo* SIFT_EXPORT info();
}
