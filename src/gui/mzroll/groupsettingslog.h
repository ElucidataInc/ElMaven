#ifndef GROUPSETTINGSLOG_H
#define GROUPSETTINGSLOG_H

#include <QDialog>

class PeakGroup;

namespace Ui {
class GroupSettingsLog;
}

class GroupSettingsLog : public QDialog
{
    Q_OBJECT

public:
    explicit GroupSettingsLog(QWidget *parent, shared_ptr<PeakGroup> group);
    ~GroupSettingsLog();

private:
    Ui::GroupSettingsLog *ui;
    shared_ptr<PeakGroup> _group;

    void _displayGroupSettings();
};

#endif // GROUPSETTINGSLOG_H
