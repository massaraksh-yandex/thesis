#ifndef TREE_HH
#define TREE_HH

#include "global_defines.hh"
#include "comparator_global.hh"

#include <QObject>
#include <QVector>

class LibraryLoader;
//class kdtree;
//class COMPARATORSHARED_EXPORT Tree : public QObject
//{
//    Q_OBJECT

//    kdtree* _data;

//public:
//    Tree(uint size, QObject *parent = 0);
//    ~Tree();

//    void push(Descriptor vector, int realIndex);

//    bool compareWith(Descriptor descriptor, double threshold);
//};

class Tree
{
    typedef void* (*fnCreate)  (int);
    typedef void  (*fnClear)   (void*);
    typedef void  (*fnInsert)  (void*, double*);
    typedef int   (*fnNearest) (void*, double*, double*, double*);

    static fnCreate _create;
    static fnClear _clear;
    static fnInsert _insert;
    static fnNearest _nearest;

    void* _data;

    friend class LibraryLoader;
public:
    Tree(uint size);
    ~Tree();

    void push(Descriptor vector);
    bool compareWith(Descriptor descriptor, double threshold);
};

#endif // TREE_HH
