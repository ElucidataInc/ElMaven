#ifndef MULTISELECTCOMBOBOX_H
#define MULTISELECTCOMBOBOX_H

#include <QComboBox>
#include <QListWidget>

class MultiSelectComboBox : public QComboBox
{
    Q_OBJECT

public:
    MultiSelectComboBox(QWidget* parent = nullptr);
    void addItem(const QString& text,
                 const QVariant &userData = QVariant());
    void addItem(const QIcon &icon,
                 const QString& text,
                 const QVariant &userData = QVariant());
    void addItems(const QStringList& text);
    QStringList selectedTexts() const;
    int count() const;
    void showPopup() override;
    void hidePopup() override;

signals:
    void selectionChanged();

public slots:
    void clear();
    void setCurrentText(const QString& text);
    void setCurrentTexts(const QStringList& texts);
    void selectAll();

protected:
    bool eventFilter(QObject* object, QEvent* event) override;

private:
    QListWidget* _listWidget;
    QLineEdit* _lineEdit;

    void _handleItemClick(int index);
    void _updatePopupSize();
    void _insertItem(QCheckBox *checkBox);
};

#endif // MULTISELECTCOMBOBOX_H
