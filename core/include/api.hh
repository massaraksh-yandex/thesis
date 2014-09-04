#pragma once

#include "global_defines.hh"

struct Api
{
    typedef void  (*Clear) (void*);
    typedef void  (*GetDefaultValues)(VectorDouble*);
    typedef void  (*GetParamNames)(QStringList*);

    GetDefaultValues getDefaultValues;
    GetParamNames getParamNames;
    Clear clear;
    int version;

    virtual ~Api() { }
};

struct ApiAlgorithm : public Api
{
    typedef void* (*Create)(CImageUnsigned* image, const VectorDouble*);
    typedef void  (*Build) (void*, DescriptorArray*, KeypointList*);
    typedef void  (*GetParams)(void*, VectorDouble*);

    Create create;
    Build build;
    GetParams getParams;
};

struct ApiTree : public Api
{
    typedef void* (*Create)  (unsigned);
    typedef void  (*Insert)  (void*, Keypoint*);
    typedef int   (*Nearest) (void*, const Descriptor *descriptor,
                              Keypoint* firstNearest, Keypoint* secondNearest);

    Create  create;
    Insert  insert;
    Nearest nearest;

    int version;
};

