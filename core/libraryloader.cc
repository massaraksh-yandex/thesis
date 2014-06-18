#include "libraryloader.hh"
#include "algorithm.hh"
#include "tree.hh"

#include <QDir>

#include <stdexcept>

void LibraryLoader::initAlgorithm()
{
    Algorithm::_create = (Algorithm::Create)   _siftLib.resolve("create");
    Algorithm::_clear  = (Algorithm::Clear)    _siftLib.resolve("clear");
    Algorithm::_build  = (Algorithm::Build)    _siftLib.resolve("build");
    Algorithm::_get    = (Algorithm::GetParams)_siftLib.resolve("getParams");
}

void LibraryLoader::initComparator()
{
    Tree::_create  = (Tree::Create) _comparatorLib.resolve("create");
    Tree::_clear   = (Tree::Clear)  _comparatorLib.resolve("clear");
    Tree::_insert  = (Tree::Insert) _comparatorLib.resolve("insert");
    Tree::_nearest = (Tree::Nearest)_comparatorLib.resolve("nearest");
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

        initComparator();
    } else if(info.type == LibAlgorithm) {
        if(Algorithm::haveInstances())
            throw std::logic_error("Cannot load library now: Almgorith instances are using");

        _comparatorLib.unload();
        _comparatorLib.setFileName(info.path);
        if(!_siftLib.load())
            return false;

        initAlgorithm();
    }

    return true;
}

LibraryInfoList LibraryLoader::info() const
{
    LibraryInfoList list;
    QLibrary lib;

    for(QString path : _paths) {
        QDir dir(path);
        for(QString file : dir.entryList()) {
            if(QLibrary::isLibrary(file)) {
                lib.setFileName(file);
                if(lib.load())
                {
                    LibraryInfo inf;
                    Info func = lib.resolve("info");
                    func(&inf);
                    inf.path = file;
                    list.append(inf);
                    lib.unload();
                }
            }
        }
    }

    return list;
}

