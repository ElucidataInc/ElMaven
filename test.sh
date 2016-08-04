make -C MavenTests/ clean
make -C MavenTests/ all -j4
./MavenTests/MavenTests -xml -o MavenTests/test.xml
