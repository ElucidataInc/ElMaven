/********************************************************************************
** Form generated from reading UI file 'noteswidget.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NOTESWIDGET_H
#define UI_NOTESWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NotesWidget
{
public:
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
    QTextEdit *noteEdit;
    QFrame *buttonFrame;
    QGridLayout *gridLayout_2;
    QPushButton *editButton;
    QSpacerItem *horizontalSpacer;
    QLineEdit *titleEdit;
    QLabel *titleLabel;
    QPushButton *fetchButton;
    QTreeWidget *treeWidget;

    void setupUi(QDockWidget *NotesWidget)
    {
        if (NotesWidget->objectName().isEmpty())
            NotesWidget->setObjectName(QString::fromUtf8("NotesWidget"));
        NotesWidget->resize(440, 586);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        gridLayout = new QGridLayout(dockWidgetContents);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        noteEdit = new QTextEdit(dockWidgetContents);
        noteEdit->setObjectName(QString::fromUtf8("noteEdit"));

        gridLayout->addWidget(noteEdit, 4, 0, 1, 2);

        buttonFrame = new QFrame(dockWidgetContents);
        buttonFrame->setObjectName(QString::fromUtf8("buttonFrame"));
        buttonFrame->setFrameShape(QFrame::NoFrame);
        buttonFrame->setFrameShadow(QFrame::Plain);
        gridLayout_2 = new QGridLayout(buttonFrame);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, -1, 0);
        editButton = new QPushButton(buttonFrame);
        editButton->setObjectName(QString::fromUtf8("editButton"));
        editButton->setEnabled(true);

        gridLayout_2->addWidget(editButton, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 1, 0, 1, 1);


        gridLayout->addWidget(buttonFrame, 5, 0, 1, 2);

        titleEdit = new QLineEdit(dockWidgetContents);
        titleEdit->setObjectName(QString::fromUtf8("titleEdit"));

        gridLayout->addWidget(titleEdit, 3, 0, 1, 2);

        titleLabel = new QLabel(dockWidgetContents);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));

        gridLayout->addWidget(titleLabel, 2, 0, 1, 2);

        fetchButton = new QPushButton(dockWidgetContents);
        fetchButton->setObjectName(QString::fromUtf8("fetchButton"));

        gridLayout->addWidget(fetchButton, 0, 0, 1, 1);

        treeWidget = new QTreeWidget(dockWidgetContents);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setLineWidth(1);
        treeWidget->setItemsExpandable(true);
        treeWidget->setSortingEnabled(true);
        treeWidget->setWordWrap(true);
        treeWidget->setColumnCount(3);
        treeWidget->header()->setStretchLastSection(true);

        gridLayout->addWidget(treeWidget, 1, 0, 1, 1);

        NotesWidget->setWidget(dockWidgetContents);

        retranslateUi(NotesWidget);
        QObject::connect(noteEdit, SIGNAL(textChanged()), buttonFrame, SLOT(show()));
        QObject::connect(titleEdit, SIGNAL(textChanged(QString)), buttonFrame, SLOT(show()));

        QMetaObject::connectSlotsByName(NotesWidget);
    } // setupUi

    void retranslateUi(QDockWidget *NotesWidget)
    {
        NotesWidget->setWindowTitle(QApplication::translate("NotesWidget", "Notes", 0, QApplication::UnicodeUTF8));
        editButton->setText(QApplication::translate("NotesWidget", "Edit Note", 0, QApplication::UnicodeUTF8));
        titleLabel->setText(QApplication::translate("NotesWidget", "Note Title:", 0, QApplication::UnicodeUTF8));
        fetchButton->setText(QApplication::translate("NotesWidget", "Fetch Notes", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(2, QApplication::translate("NotesWidget", "Title", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("NotesWidget", "rt", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("NotesWidget", "m/z", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class NotesWidget: public Ui_NotesWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NOTESWIDGET_H
