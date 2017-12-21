TEMPLATE = subdirs
CONFIG += ordered qt thread


SUBDIRS += libneural libcdfread  libcsvparser pugixml/src  libpillow  libdate libpls
!macx {
    SUBDIRS += libplog
}
