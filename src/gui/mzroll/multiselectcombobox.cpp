#include "multiselectcombobox.h"
#include <QCheckBox>
#include <QEvent>
#include <QLineEdit>

MultiSelectComboBox::MultiSelectComboBox(QWidget* parent)
    : QComboBox(parent),
      _listWidget(new QListWidget(parent)),
      _lineEdit(new QLineEdit(this))
{
    _lineEdit->setReadOnly(true);
    _lineEdit->installEventFilter(this);

    setModel(_listWidget->model());
    setView(_listWidget);
    setLineEdit(_lineEdit);

    _listWidget->setStyleSheet("QListView::item { margin: 2px; }");
    setStyleSheet(
        "QComboBox QAbstractItemView { "
        "selection-background-color: #eee; "
        "}");

    this->view()->setMinimumWidth(400);

    connect(this,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
            this,
            &MultiSelectComboBox::_handleItemClick);
}

void MultiSelectComboBox::hidePopup()
{
    int width = this->width();
    int height = _listWidget->height();
    int x = QCursor::pos().x() - mapToGlobal(geometry().topLeft()).x()
            + geometry().x();
    int y = QCursor::pos().y() - mapToGlobal(geometry().topLeft()).y()
            + geometry().y();

    if (x >= 0 && x <= width && y >= this->height()
        && y <= height + this->height()) {
        // one of the items was clicked, do not hide popup
    } else {
        QComboBox::hidePopup();
    }
}

void MultiSelectComboBox::showPopup()
{
    QComboBox::showPopup();
    _updatePopupSize();
}

void MultiSelectComboBox::_updatePopupSize()
{
    if (count() > 0) {
        auto rowHeight = _listWidget->sizeHintForRow(0);
        auto height = rowHeight * count();
        _listWidget->viewport()->setFixedHeight(height);
        _listWidget->setFixedHeight(height);
    }
}

void MultiSelectComboBox::_insertItem(QCheckBox* checkBox)
{
    QListWidgetItem* listWidgetItem = new QListWidgetItem(_listWidget);
    _listWidget->addItem(listWidgetItem);
    _listWidget->setItemWidget(listWidgetItem, checkBox);

    connect(checkBox, &QCheckBox::stateChanged, this, [this] {
        auto selectedValues = selectedTexts();
        // ignoring relabelling from count
        if (!selectedValues.contains("Re-Label")
            && selectedValues.size() == count() - 1) {
            _lineEdit->setText("All");
            _lineEdit->setToolTip("All values are selected");
        } else if (selectedValues.size() == 0) {
            _lineEdit->setText("None");
            _lineEdit->setToolTip("No values are selected");
        } else {
            _lineEdit->setText(selectedValues.join(", "));
            _lineEdit->setToolTip(_lineEdit->text());
        }
        _lineEdit->home(false);
        emit selectionChanged();
    });

    _updatePopupSize();
}

void MultiSelectComboBox::addItem(const QString& text, const QVariant& userData)
{
    QCheckBox* checkBox = new QCheckBox(this);
    checkBox->setText(text);
    _insertItem(checkBox);
}

void MultiSelectComboBox::addItem(const QIcon& icon,
                                  const QString& text,
                                  const QVariant& userData)
{
    QCheckBox* checkBox = new QCheckBox(this);
    checkBox->setIcon(icon);
    checkBox->setText(text);
    checkBox->setStyleSheet("QCheckBox { spacing: 2px; }");
    _insertItem(checkBox);
}

void MultiSelectComboBox::addItems(const QStringList& texts)
{
    for (const auto& string : texts)
        addItem(string);
}

QStringList MultiSelectComboBox::selectedTexts() const
{
    QStringList selectedTexts;
    for (int i = 0; i < count(); ++i) {
        QWidget* widget = _listWidget->itemWidget(_listWidget->item(i));
        QCheckBox* checkBox = static_cast<QCheckBox*>(widget);

        if (checkBox->isChecked())
            selectedTexts << checkBox->text();
    }

    return selectedTexts;
}

int MultiSelectComboBox::count() const
{
    return _listWidget->count();
}

void MultiSelectComboBox::_handleItemClick(int index)
{
    QWidget* widget = _listWidget->itemWidget(_listWidget->item(index));
    QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
    checkBox->setChecked(!checkBox->isChecked());
}

void MultiSelectComboBox::clear()
{
    _listWidget->clear();
}

bool MultiSelectComboBox::eventFilter(QObject* object, QEvent* event)
{
    if (object == _lineEdit && event->type() == QEvent::MouseButtonRelease) {
        showPopup();
        return false;
    }
    return false;
}

void MultiSelectComboBox::setCurrentText(const QString& text)
{
    // ignore single text setter, inherited from regular combo box
}

void MultiSelectComboBox::setCurrentTexts(const QStringList& texts)
{
    // Split texts into the label and percentage in peaktable
    QStringList selectedTexts;
    for (auto text : texts) {
        auto splitted = text.split("(");
        selectedTexts << splitted.at(0);
    }

    for (int i = 0; i < count() - 1; ++i) {
        QWidget* widget = _listWidget->itemWidget(_listWidget->item(i));
        QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
        QString checkBoxString = checkBox->text();
        auto splitted = checkBoxString.split("(");
        if (selectedTexts.contains(splitted.at(0)))
            checkBox->setChecked(true);
    }
    Q_EMIT(&QCheckBox::stateChanged);
}

void MultiSelectComboBox::uncheckRelabel()
{
    QWidget* widget = _listWidget->itemWidget(_listWidget->item(6));
    QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
    checkBox->setChecked(false);
    Q_EMIT(&QCheckBox::stateChanged);
}

void MultiSelectComboBox::selectAll()
{
    for (int i = 0; i < count() - 1; ++i) {
        QWidget* widget = _listWidget->itemWidget(_listWidget->item(i));
        QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
        checkBox->setChecked(true);
    }
    // uncheck re-label option
    QWidget* widget = _listWidget->itemWidget(_listWidget->item(6));
    QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
    checkBox->setChecked(false);
    Q_EMIT(&QCheckBox::stateChanged);
}
