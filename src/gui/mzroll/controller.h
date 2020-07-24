#include <QObject>
#include <QVariant>

#include "projectdatabase.h"

class DownloadManager;
class IsotopeDialogSettings;
class AdductsDialogSettings;
class PollyIntegration;
class MainWindow;
class PeakDetectionSettings;
class OptionsDialogSettings;
#ifndef Q_OS_LINUX
class AutoUpdater;
#endif

class Controller:  public QObject
{
    Q_OBJECT
public:
    Controller();
    ~Controller();
    MainWindow* getMainWindow() { return _mw; }
#ifndef Q_OS_LINUX
    AutoUpdater* getUpdater() { return _updater; }
#endif
    PollyIntegration* iPolly;

public Q_SLOTS:
    void updateUi();
    void resetMP(QList<QString> keys);

private:
    MainWindow* _mw;
    DownloadManager* _dlManager;
#ifndef Q_OS_LINUX
    AutoUpdater* _updater;
#endif

    void _updateMavenParameters(const QString& key, const QVariant& value);

    template<typename T>
    void _syncMpWithUi(T* dialogPtr);

    template<typename T>
    void _updateDialogSettings(T* settings);

private Q_SLOTS:
    void _updateIsotopeDialogSettings(IsotopeDialogSettings* id);
    void _updateAdductsDialogSettings(AdductsDialogSettings* id);
    void _updatePeakDetectionSettings(PeakDetectionSettings* pd);
    void _updateOptionsDialogSettings(OptionsDialogSettings* od);
    void _updateSettingsForSave(const QString& k, const QVariant& v);
    void _updateSettingsFromLoad(const map<string, variant>& settingsMap);
};
