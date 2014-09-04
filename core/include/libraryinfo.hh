#pragma once
#include <QString>

struct LibraryInfo
{
    enum LibraryType
    {
        Unknown,
        Algorithm,
        Comparator
    };

    QString path;
    LibraryType type;
    QString info;
    QString statusMessage;
    bool valid;

    LibraryInfo() : type(Unknown), valid(false) { }
};
typedef QList<LibraryInfo> LibraryInfoList;

