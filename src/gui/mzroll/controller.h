#include <QObject>
#include <QVariant>

class MainWindow;
class PeakDetectionSettings;
class OptionsDialogSettings;

class Controller:  public QObject
{
    Q_OBJECT
    public:
        Controller();
        ~Controller();
        MainWindow* getMainWindow() { return mw; }

    public Q_SLOTS:
      void updatePeakDetectionSettings(PeakDetectionSettings* pd);
      void updateOptionsDialogSettings(OptionsDialogSettings* od);
      void updateUi();


    private:
        void updateMavenParameters(const QString& key, const QVariant& value);
        template <typename T>
        void syncMpWithUi(T* dialogPtr);


    private:
        MainWindow* mw;
};
