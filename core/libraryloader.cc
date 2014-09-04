#include <QDir>
#include "libraryloader.hh"
#include "algorithm.hh"
#include "tree.hh"

#include <stdexcept>

QString LibraryLoader::initAlgorithm(bool real)
{
    Algorithm::Create            f1  = (Algorithm::Create)_siftLib.resolve("create");
    Algorithm::Clear             f2  = (Algorithm::Clear)_siftLib.resolve("clear");
    Algorithm::Build             f3  = (Algorithm::Build)_siftLib.resolve("build");
    Algorithm::GetParams         f4  = (Algorithm::GetParams)_siftLib.resolve("getParams");
    Algorithm::GetDefaultValues  f5  = (Algorithm::GetDefaultValues)_siftLib.resolve("getDefaultValues");
    Algorithm::GetParamNames     f6  = (Algorithm::GetParamNames)_siftLib.resolve("getParamNames");

    QString result;

    if(!f1)
        result += "Missed \'creare\'\n";
    if(!f2)
        result += "Missed \'clear\'\n";
    if(!f3)
        result += "Missed \'build\'\n";
    if(!f4)
        result += "Missed \'getParams\'\n";
    if(!f5)
        result += "Missed \'getDefaultValues\'\n";
    if(!f6)
        result += "Missed \'getParamNames\'\n";

    if(real)
    {
        Algorithm::api->create           = f1;
        Algorithm::api->clear            = f2;
        Algorithm::_build            = f3;
        Algorithm::_getParams        = f4;
        Algorithm::_getDefaultValues = f5;
        Algorithm::_getParamNames    = f6;
    }

    return result;
}

QString LibraryLoader::initComparator(bool real)
{
    Tree::Create            f1 = (Tree::Create)   _comparatorLib.resolve("create");
    Tree::Clear             f2 = (Tree::Clear)    _comparatorLib.resolve("clear");
    Tree::Insert            f3 = (Tree::Insert)   _comparatorLib.resolve("insert");
    Tree::Nearest           f4 = (Tree::Nearest)  _comparatorLib.resolve("nearest");
    Tree::GetParamNames     f5 = (Tree::GetParamNames)  _comparatorLib.resolve("getParamNames");
    Tree::GetDefaultValues  f6 = (Tree::GetDefaultValues)  _comparatorLib.resolve("getDefaultValues");

    QString result;

    if(!f1)
        result += "Missed \'creare\'\n";
    if(!f2)
        result += "Missed \'clear\'\n";
    if(!f3)
        result += "Missed \'insert'\n";
    if(!f4)
        result += "Missed \'nearest'\n";
    if(!f5)
        result += "Missed \'getParamNames'\n";
    if(!f6)
        result += "Missed \'getDefaultValues'\n";

    if(real)
    {
        Tree::_create           = f1;
        Tree::_clear            = f2;
        Tree::_insert           = f3;
        Tree::_nearest          = f4;
        Tree::_getParamNames    = f5;
        Tree::_getDefaultValues = f6;
    }

    return result;
}

LibraryLoader::LibraryLoader(QString searchPath)
{
    _paths.append(searchPath);
}

bool LibraryLoader::load(LibraryInfo info)
{
    if(info.type == LibComparator) {
        if(Tree::haveInstances())
            throw std::logic_error("Cannot load library now: Tree instances are using");

        _comparatorLib.unload();
        _comparatorLib.setFileName(info.path);
        if(!_comparatorLib.load())
            return false;

        initComparator(true);

    } else if(info.type == LibAlgorithm) {
        if(Algorithm::haveInstanses())
            throw std::logic_error("Cannot load library now: Almgorith instances are using");

        _comparatorLib.unload();
        _comparatorLib.setFileName(info.path);
        if(!_siftLib.load())
            return false;

        Algorithm::api =
        initAlgorithm(true);
    }

    return true;
}

LibraryInfoList LibraryLoader::info()
{
    LibraryInfoList list;
    QLibrary lib;

    for(QString path : _paths) {
        QDir dir(path);
        for(QString file : dir.entryList()) {
            if(QLibrary::isLibrary(file)) {

                LibraryInfo inf;

                inf.path = file;
                lib.setFileName(file);
                if(lib.load())
                {
                    GetLibraryAPIVersion apiVersion = (GetLibraryAPIVersion)lib.resolve("getLibraryAPIVersion");
                    if(!apiVersion) {
                        inf.statusMessage = "Missed function \'getLibraryAPIVersion\'\n";
                        goto END;
                    }

                    LibraryInfoFunc func = (LibraryInfoFunc)lib.resolve("info");
                    if(!func) {
                        inf.statusMessage += "Missed function \'info'\n";
                        goto END;
                    }

                    func(&inf);

                    switch (inf.type) {
                    case LibAlgorithm:
                        inf.statusMessage += initAlgorithm(false);
                        break;
                    case LibComparator:
                        inf.statusMessage += initComparator(false);
                        break;
                    default:
                        inf.statusMessage += "Unknown library type\n";
                        goto END;
                        break;
                    }
                    inf.valid = true;
                }
                END:;
                list.append(inf);
                lib.unload();
            }
        }
    }

    return list;
}

