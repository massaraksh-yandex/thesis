#ifndef TREE_HH
#define TREE_HH

#include "global_defines.hh"
#include "comparator_global.hh"

#include <QObject>
#include <QVector>

class kdtree;
class COMPARATORSHARED_EXPORT Tree : public QObject
{
    Q_OBJECT

    kdtree* _data;

public:
    Tree(uint size, QObject *parent = 0);
    ~Tree();

    void push(Descriptor vector, int realIndex);

    bool compareWith(Descriptor descriptor, double threshold);
};

#endif // TREE_HH
