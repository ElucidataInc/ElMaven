#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include <ui_librarymanager.h>

class MainWindow;

class LibraryManager : public QDialog,
                       public Ui_LibraryManager
{
    Q_OBJECT

public:
    explicit LibraryManager(MainWindow *parent = nullptr);

private:
    /**
     * @brief A pointer to the parent main window of this dialog.
     */
    MainWindow* _mw;
};

#endif // LIBRARYMANAGER_H
