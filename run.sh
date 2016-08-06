OMP_CANCELLATION=true
qmake -qt=qt4 -o Makefile build.pro
make -j $(nproc)


if [ -f MavenTests/test.xml ]; then
	rm MavenTests/test.xml
fi

if [ -f bin/MavenTests ]; then
	./bin/MavenTests -xml -o MavenTests/test.xml
fi

