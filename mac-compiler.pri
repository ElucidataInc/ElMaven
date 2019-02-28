macx {
    # Although the user is using llvm/clang, Qt might still choose to use XCode's clang
    # We can't use XCode's clang since it lacks OpenMP support
    QMAKE_CC = $$system( which clang)
    QMAKE_CXX = $$system( which clang++)
}
