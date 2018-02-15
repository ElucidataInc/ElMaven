TEMPLATE = subdirs
CONFIG += ordered qt thread


SUBDIRS += libneural libcdfread  libcsvparser pugixml/src  libpillow libpls ErrorHandling
!macx {
    SUBDIRS += libplog
}
