TEMPLATE = subdirs
CONFIG += ordered qt thread

macx{
#    QMAKE_CXX = /usr/local/opt/llvm@3.7/lib/llvm-3.7/bin/clang++
}

SUBDIRS += 3rdparty src tests/MavenTests CrashReporter
