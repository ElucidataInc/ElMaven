make -C MavenTests/ clean
make -C MavenTests/ all -j4
rm MavenTests/test.xml
./MavenTests/MavenTests -xml -o MavenTests/test.xml
