TEMPLATE = subdirs

SUBDIRS += \
    algorithm-sift \
    comparator-kdtree \
    core \
    ui

ui.depends = core
core.depends = algorithm-sift comparator-kdtree

CONFIG += ordered
