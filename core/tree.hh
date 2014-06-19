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
    typedef void  (*Insert)  (void*, Descriptor*);
    typedef int   (*Nearest) (void*, const Descriptor*, Descriptor*, Descriptor*);
    typedef void  (*GetDefaultValues)(VectorDouble*);
    typedef void  (*GetParamNames)(QStringList*);

    static Create  _create;
    static Clear   _clear;
    static Insert  _insert;
    static Nearest _nearest;
    static GetDefaultValues _getDefaultValues;
    static GetParamNames _getParamNames;

    static uint _mutex;

    void* _data;

public:
    Tree(uint size);
    ~Tree();

    void push(Descriptor &vector);
    bool compareWith(Descriptor descriptor);

    static QStringList paramNames();
    static VectorDouble defaultValues();

    static bool haveInstances() { return _mutex != 0; }
};

typedef QSharedPointer<Tree> TreePtr;

#endif // TREE_HH
