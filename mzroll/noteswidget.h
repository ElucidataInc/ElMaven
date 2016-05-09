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

public slots:
         void showNotes();
	 void showSelectedNote();
 	 void showNote(int noteid);
 	 void showNote(UserNote* note);

         QList<UserNote*> getNotes(float mzmin, float mzmax);
         void updateAllNotes();

     private slots:
         void readRemoteData(const QHttpResponseHeader &);
         QList<UserNote*> getRemoteNotes();
         void editNote();
         void editRemoteNote();
         void fetchRemoteNotes();

private:

         QList<UserNote*> _notes;
         int connectionId;
         QHttp http;
         QXmlStreamReader xml;
         UserNote* selectedNote;
         MainWindow* _mainwindow;
};

#endif // NOTESWIDGET.h
