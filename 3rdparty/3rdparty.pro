include($$mac_compiler)

TEMPLATE = subdirs
CONFIG += ordered qt thread

SUBDIRS += obiwarp       \
           libneural     \
           libcdfread    \
           libcsvparser  \
           pugixml/src   \
           libpls        \
           Logger        \
           ErrorHandling \
           libsvm        \
           NimbleDSP     \
           libmgf

macx {
  SUBDIRS -= libcdfread
}
