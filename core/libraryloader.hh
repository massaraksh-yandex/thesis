#ifndef LIBRARYLOADER_HH
#define LIBRARYLOADER_HH

#include <QLibrary>
#include <QString>
#include <QList>
#include "../global_defines.hh"

class LibraryLoader
{
public:
    typedef void (*Info)(QString*, LibraryType*);

private:
    QLibrary _siftLib;
    QLibrary _comparatorLib;

    QStringList _paths;

    void initAlgorithm();
    void initComparator();

public:
    LibraryLoader();

    bool load(LibraryInfo info);

    LibraryInfoList info() const;

    QStringList path() const { return _paths; }
    void setPath(QStringList list) { _paths = list; }
};

#endif // LIBRARYLOADER_HH
