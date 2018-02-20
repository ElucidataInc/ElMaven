make distclean
rm -rf build/lib/*
make -C MavenTests/ distclean
rm -rf bin/maven_dev* bin/El_Maven* bin/peakdetector bin/mzWatcher docs/html docs/latex 
rm -rf $HOME/.config/mzRoll
find . -type d | grep tmp | xargs rm -rfd
rm -frd ../coverage
rm -rf ../coverage.info