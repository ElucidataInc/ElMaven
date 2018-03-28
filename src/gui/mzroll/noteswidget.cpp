#include "noteswidget.h"
#include "mainwindow.h"
//http://genomics.princeton.edu/~emelamud/mzroll/notes.xml
//http://genomics-pubs.princeton.edu/mzroll/notes.xml

NotesWidget::NotesWidget(QWidget *parent): QDockWidget(parent)
{

    setupUi(this);
    selectedNote=NULL;
    _mainwindow = (MainWindow*) parent;

    buttonFrame->hide();
    connect(treeWidget,SIGNAL(itemSelectionChanged()), SLOT(showSelectedNote()));
    connect(editButton,SIGNAL(clicked()), this, SLOT(editNote()));

    //remote note fetching on button click
    connect(fetchButton,SIGNAL(clicked()), this, SLOT(updateAllNotes()));
    connect(fetchButton,SIGNAL(clicked()), this, SLOT(showNotes()));

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(readRemoteData(QNetworkReply*)));


    updateAllNotes();
    showNotes();
}

NotesWidget::~NotesWidget()
{
}


void NotesWidget::fetchRemoteNotes()
{
    qDebug() << "fetchRemoteNotes()";
    xml.clear();
    QSettings *settings = _mainwindow->getSettings();

    if ( settings->contains("data_server_url")) {
        QUrl url(settings->value("data_server_url").toString());
        QUrlQuery query;
        query.addQueryItem("action", "fetchnotes");
        query.addQueryItem("format", "xml");
        url.setQuery(query);

        QNetworkRequest request;
        request.setUrl(url);

        QNetworkReply *reply = m_manager->get(request);
        qDebug() << url.toEncoded();
    }
}

void NotesWidget::readRemoteData(QNetworkReply *reply)
{
    if (reply) { //redirect
        xml.addData(reply->readAll());
        _notes << getRemoteNotes();
        showNotes();
    } else {
        reply->abort();
    }
}

// update note desplay
void NotesWidget::showNotes() {
    
    qDebug() << "showNotes()";
    treeWidget->clear();
    Q_FOREACH(UserNote* usernote, _notes ) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, QString::number(usernote->mz));
        item->setText(1, QString::number(usernote->rt));
        item->setText(2, usernote->title);
        item->setData(0,Qt::UserRole,QVariant::fromValue(usernote));
        treeWidget->addTopLevelItem(item);
       // qDebug() << usernote->title;
    }
    treeWidget->update();
}


//
// NOTE FETCH OPERATIONS
//
void NotesWidget::updateAllNotes() {
    
    qDeleteAll(_notes);
    _notes.clear();
 //   _notes << getLocalNotes(); //local notes
    //fetchRemoteNotes();       //remote notes
}

QList<UserNote*> NotesWidget::getNotes(float mzmin, float mzmax) {
    QList<UserNote*>subset;

    Q_FOREACH(UserNote* usernote, _notes ) {
        if ( usernote && usernote->mz >= mzmin && usernote->mz <= mzmax)
            subset << usernote;
    }
    return subset;
}



QList<UserNote*> NotesWidget::getRemoteNotes()
{
    //qDebug() << "NotesWidget::getRemoteNotes()";
    UserNote *usernote=NULL;
    QString currentTag;
    QList<UserNote*>remotenotes;

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == "item"){
                usernote = new UserNote();
                usernote->remoteNote=true;
            }
            currentTag = xml.name().toString().toLower();
        } else if (xml.isEndElement()) {
            if (xml.name() == "item") {
                if (usernote) remotenotes << usernote;
           //     qDebug() << "new remote note..";
            }
        }  else if (xml.isCharacters() && !xml.isWhitespace()) {
	    if (usernote == NULL ){
                qDebug() << "Parse Error: ";
		return remotenotes;
	    }
            else if (currentTag == "noteid")
                usernote->noteid = xml.text().toString().toInt();
            else if (currentTag == "mz")
                usernote->mz = xml.text().toString().toDouble();
            else if (currentTag == "mzmin")
                usernote->mzmin = xml.text().toString().toDouble();
            else if (currentTag == "mzmax")
                usernote->mzmax = xml.text().toString().toDouble();
            else if (currentTag == "rt")
                usernote->rt = xml.text().toString().toDouble();
            else if (currentTag == "intensity")
                usernote->intensity = xml.text().toString().toDouble();
            else if (currentTag == "compound_id")
                usernote->compound_id = xml.text().toString().toDouble();
            else if (currentTag == "compound_name")
                usernote->compound_name = xml.text().toString().toDouble();
            else if (currentTag == "srm_id")
                usernote->srm_id = xml.text().toString().toDouble();
            else if (currentTag == "title")
                usernote->title += xml.text().toString();
            else if (currentTag == "link")
                usernote->link += xml.text().toString();
            else if (currentTag == "description")
                usernote->description += xml.text().toString();
            else if (currentTag == "pubDate")
                usernote->creationTime += xml.text().toString();
            else if (currentTag == "author")
                usernote->author += xml.text().toString();
            else if (currentTag == "guid")
                usernote->guid += xml.text().toString();

        }
    }

    if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
        qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
    }
    return remotenotes;
}

void NotesWidget::editNote() {
    
    if(!selectedNote) return;

    QUrl url(_mainwindow->getSettings()->value("data_server_url").toString());

    QUrlQuery query;
    QString newTitle = titleEdit->text();
    if (newTitle != selectedNote->title){
      selectedNote->title = newTitle;
      query.addQueryItem("title", newTitle);
    }

    QString newDesc = noteEdit->toPlainText();
    if (newDesc != selectedNote->description){
      selectedNote->description = newDesc;
      query.addQueryItem("description", newDesc);
    }

    url.setQuery(query);
    if (url.hasQuery()) {
      query.addQueryItem("action", "editnote");
      query.addQueryItem("noteid", QString::number(selectedNote->noteid));
      QDesktopServices::openUrl(url);
    } // else no changes
    return;
}

void NotesWidget::editRemoteNote() {

    QSettings *settings = _mainwindow->getSettings();

    if ( settings->contains("data_server_url")) {
        Q_FOREACH( QTreeWidgetItem* item, treeWidget->selectedItems() ) {
            QVariant v =   item->data(0,Qt::UserRole);
            UserNote* usernote =  v.value<UserNote*>();

	    usernote->title = titleEdit->text();
            usernote->description = noteEdit->toPlainText();

            QUrl url(settings->value("data_server_url").toString());
            QUrlQuery query;
            query.addQueryItem("action", "editnote");
            query.addQueryItem("noteid", QString::number(usernote->noteid));
            url.setQuery(query);
            QDesktopServices::openUrl(url);
            return;
        }
    }
}


// NOTE DISPLAY
void NotesWidget::showNote(int noteid) {
  //  qDebug() << "showNote() " << noteid;
    Q_FOREACH(UserNote* usernote, _notes ) {
        if (usernote && usernote->noteid == noteid ) {
            qDebug() << "showingNote() " << usernote;
            showNote(usernote);
            return;
        }
    }
}

void NotesWidget::showNote(UserNote* usernote) {
    if (!usernote) return;

    QString title = tr("Notes: Author:%1 m/z:%2 rt:%3 I:%2").arg(
            usernote->author,
            QString::number(usernote->mz),
            QString::number(usernote->rt),
            QString::number(usernote->intensity)
            );


    //if (!usernote->link.isEmpty() ) { title += tr("<a href=\"%1\">Link</a>").arg(usernote->link); }
    titleLabel->setText(title);
    titleEdit->setText(usernote->title);
    noteEdit->setPlainText(usernote->description);
    noteEdit->show();
    buttonFrame->hide();
    selectedNote=usernote;

}

void NotesWidget::showSelectedNote()
{
    Q_FOREACH(QTreeWidgetItem* item, treeWidget->selectedItems() ) {

        QVariant v =   item->data(0,Qt::UserRole);
        UserNote* usernote =  v.value<UserNote*>();
        showNote(usernote);
        mzSlice slice(usernote->mzmin,usernote->mzmax,usernote->rt-2,usernote->rt+2);
         qDebug() << "showSelectedNote()" << usernote->mzmin << " " << usernote->mzmax;
        _mainwindow->getEicWidget()->setMzSlice(slice);
        return;
    }
}



