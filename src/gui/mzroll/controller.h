#include <QObject>

class MainWindow;

class Controller:  public QObject
{
    Q_OBJECT
    public:
        Controller();
        MainWindow* getMainWindow() { return mw; }





    private:
        MainWindow* mw;
};
