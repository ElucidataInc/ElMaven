OMP_CANCELLATION=true
qmake -qt=qt4 -o Makefile build.pro
make -j $(nproc)

qmake -qt=qt4 -o MavenTests/Makefile build.pro CONFIG+=test
make -C MavenTests/ all $(nproc)

if [ -f MavenTests/test.xml ]; then
	rm MavenTests/test.xml
fi

if [ -f MavenTests/MavenTests ]; then
	./MavenTests/MavenTests -xml -o MavenTests/test.xml
fi

