#ifndef NOTESWIDGET_H
#define NOTESWIDGET_H

#include "globals.h"
#include "stable.h"
#include "ui_noteswidget.h"
#include "mainwindow.h"

class NotesWidget : public QDockWidget, public Ui_NotesWidget 
{
    Q_OBJECT

public:
    NotesWidget(QWidget *parent = 0);
    ~NotesWidget();

public Q_SLOTS:
         void showNotes();
	 void showSelectedNote();
 	 void showNote(int noteid);
 	 void showNote(UserNote* note);

         QList<UserNote*> getNotes(float mzmin, float mzmax);
         void updateAllNotes();

     private Q_SLOTS:
         void readRemoteData(QNetworkReply* reply);
         QList<UserNote*> getRemoteNotes();
         void editNote();
         void editRemoteNote();
         void fetchRemoteNotes();

private:

         QList<UserNote*> _notes;
         int connectionId;
         QNetworkAccessManager* m_manager;
         QXmlStreamReader xml;
         UserNote* selectedNote;
         MainWindow* _mainwindow;
};

#endif // NOTESWIDGET.h
