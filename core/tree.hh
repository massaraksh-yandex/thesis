#ifndef TREE_HH
#define TREE_HH

#include "global_core.hh"
#include <QObject>
#include <QVector>

class LibraryLoader;

class Tree
{
    friend class LibraryLoader;

    typedef void* (*Create)  (unsigned);
    typedef void  (*Clear)   (void*);
    typedef void  (*Insert)  (void*, const double*);
    typedef int   (*Nearest) (void*, double*, double*, double*);

    static Create  _create;
    static Clear   _clear;
    static Insert  _insert;
    static Nearest _nearest;

    static uint _mutex;

    void* _data;

public:
    Tree(uint size);
    ~Tree();

    void push(Descriptor vector);
    bool compareWith(Descriptor descriptor, double threshold);

    static bool haveInstances() { return _mutex != 0; }
};

typedef Tree* TreePtr;

#endif // TREE_HH
