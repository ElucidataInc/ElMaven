#include <QObject>
#include <QVariant>

class MainWindow;
class PeakDetectionSettings;

class Controller:  public QObject
{
    Q_OBJECT
    public:
        Controller();
        MainWindow* getMainWindow() { return mw; }

    public Q_SLOTS:
      void updatePeakDetectionSettings(PeakDetectionSettings* pd);


    private:
        void updateMavenParameters(const QString& key, const QVariant& value);


    private:
        MainWindow* mw;
};
