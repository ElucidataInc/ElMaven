OMP_CANCELLATION=true
qmake -qt=qt4 -o Makefile build.pro
make -j $(nproc)
