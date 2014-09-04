#ifndef LIBRARYLOADER_HH
#define LIBRARYLOADER_HH

#include <QLibrary>
#include <QString>
#include <QList>
#include "include/global_defines.hh"

class LibraryLoader
{
    QLibrary _siftLib;
    QLibrary _comparatorLib;

    QStringList _paths;

public:
    LibraryLoader(QString searchPath);

    bool load(LibraryInfo info);

    LibraryInfoList info();

    QStringList path() const { return _paths; }
    void setPath(QStringList list) { _paths = list; }
};

#endif // LIBRARYLOADER_HH
