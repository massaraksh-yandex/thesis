TEMPLATE = subdirs

SUBDIRS += \
    sift \
    comparator \
    core \
    ui

ui.depends = core
core.depends = sift comparator

CONFIG += ordered
