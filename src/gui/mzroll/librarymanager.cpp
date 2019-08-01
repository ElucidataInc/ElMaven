#include "librarymanager.h"
#include "mainwindow.h"

LibraryManager::LibraryManager(MainWindow* parent)
    : QDialog(parent), _mw(parent)
{
    setupUi(this);
}
