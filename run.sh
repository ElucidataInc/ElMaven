OMP_CANCELLATION=true
qmake
make -j $(nproc)
bin/maven_dev_769
