rm -rf build
mkdir build;
cd build
qmake ../*.pro;
make clean;
make all;
cd ..
