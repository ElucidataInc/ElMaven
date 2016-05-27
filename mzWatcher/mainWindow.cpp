#include "mainWindow.h"
//--#include <QHistogramSlider.h>

MainWindow::MainWindow(QWidget* parent):QMainWindow(parent) {

        //initailize
        timerId=0;


        readSettings();

	extention = settings->value("extention").toString();
	sourceFolder = settings->value("sourceFolder").toString();
	destFolder   = settings->value("destFolder").toString();
	convertCommand = settings->value("convertCommand").toString();
	maxDayDiff=settings->value("maxDayDiff").toInt();

	centralWidget = new QWidget(parent);
	guiForm = new Ui_mzWatcherGui();
	guiForm->setupUi(centralWidget);
	setCentralWidget(centralWidget);
	centralWidget->setVisible(true);

	guiForm->sourceFolderEdit->setText(sourceFolder);
	guiForm->destFolderEdit->setText(destFolder);
	guiForm->commandEdit->setText(convertCommand);
        guiForm->extentionEdit->setText(extention);
        guiForm->progressBar->hide();
        guiForm->remoteLoging->setChecked( settings->value("remoteLoging").toBool() );
        guiForm->remoteServerUrl->setText(settings->value("remoteServerUrl").toString());
        guiForm->instrumentId->setText(settings->value("instrumentId").toString());
        guiForm->monitorTimeout->setValue(settings->value("monitorTimeout").toInt());

        guiForm->watchButton->setCheckable(true);
        guiForm->watchButton->setChecked(settings->value("watchButtonState").toBool());


        connect(guiForm->refreshButton,SIGNAL(pressed()),this,SLOT(updateFileList()));
        connect(guiForm->convertButton,SIGNAL(pressed()),this,SLOT(processSelectedFiles()));
        connect(guiForm->watchButton,SIGNAL(toggled(bool)),this,SLOT(monitor()));

        connect(guiForm->sourceFolderEdit,SIGNAL(textEdited(QString)),this,SLOT(getFormValues()));
        connect(guiForm->destFolderEdit,SIGNAL(textEdited(QString)),this,SLOT(getFormValues()));
        connect(guiForm->commandEdit,SIGNAL(textEdited(QString)),this,SLOT(getFormValues()));
	connect(guiForm->extentionEdit,SIGNAL(textEdited(QString)),this,SLOT(getFormValues()));
	connect(guiForm->dayDiffBox,SIGNAL(valueChanged(int)),this,SLOT(getFormValues()));
	connect(guiForm->destFolderButton,SIGNAL(pressed()),this,SLOT(selectDestFolder()));
	connect(guiForm->sourceFolderButton,SIGNAL(pressed()),this,SLOT(selectSourceFolder()));
        connect(guiForm->clearDatabaseButton,SIGNAL(pressed()),this,SLOT(clearTables()));

	converter = new BackgroundThread(this);
	connect(converter,SIGNAL(statusChanged(QString)),this,SLOT(setStatus(QString)));


        QString dbname=QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/mzWatcher.db";

        setStatus("Using Datababase: " + dbname);
        DB = QSqlDatabase::addDatabase("QSQLITE", dbname);
        DB.setDatabaseName(dbname);
        DB.open();
        createTables();
       //clearTables();

       /*
	DB = QSqlDatabase::addDatabase("QODBC");
	DB.setDatabaseName("MetabolomicsDB");

	if (DB.open()) {
		setStatus("Connected to Metabolomics DB!");
	} else {
		setStatus("Failed to connect to MetabolomicsDB");
	}
        */

        guiForm->tabWidget->setCurrentIndex(0);

        showDataFilesTable();
        getFormValues();

        remoteLogMessage("service", "", 0 , "mzWatcher started");
        if(guiForm->watchButton->isChecked()) monitor();
}

void MainWindow::setStatus(QString status) {

        QString nowTime = QTime::currentTime().toString("h:m:s ap");
	//guiForm->statusLabel->setText(status);
        guiForm->logWidget->append(nowTime + ":" + status);
}

bool MainWindow::checkParameters() {
	QDir s(sourceFolder);

	bool ok=true;
	if (!s.exists()) { setStatus("Source Path doesn't exists"); return 0;
		guiForm->sourceFolderEdit->setFrame(true);
		ok=false;
	}

	QDir d(destFolder);
	if (!d.exists()) { setStatus("Destination Path doesn't exists"); return 0;
		guiForm->sourceFolderEdit->setFrame(true);
		ok=false;
	}


	return ok;
}

void MainWindow::stop_startConversion() {

	if (converter->isRunning()) {
		stopConversion();
	} else{
		startConversion();
	}
}

void MainWindow::selectDestFolder() {
	QString dir =QFileDialog::getExistingDirectory(this,".");
	destFolder = dir+ "/";
	guiForm->destFolderEdit->setText(dir);
}

void MainWindow::selectSourceFolder() {
	QString dir = QFileDialog::getExistingDirectory(this,".");
	sourceFolder = dir + "/";
	guiForm->sourceFolderEdit->setText(dir);
}

void MainWindow::stopConversion() {
	if (converter->isRunning()) {
		converter->killProcess();
	}

        int selectedItemsCount = guiForm->treeWidget->selectedItems().size();
}


void MainWindow::startConversion() {
	if (converter->isRunning()) {
		stopConversion();
		return;
	}

        guiForm->convertButton->setText("Stop");

	processSelectedFiles();
	updateFileList();
        guiForm->convertButton->setText("Convert");
}

void MainWindow::processSelectedFiles() {
	QStringList selectedfiles;

	foreach(QTreeWidgetItem* item, guiForm->treeWidget->selectedItems() ) {
                selectedfiles << item->text(2);
	}

        int count=0;
        guiForm->progressBar->show();
        guiForm->progressBar->setMaximum(selectedfiles.size());
	foreach (QString filename, selectedfiles) {
                guiForm->progressBar->setValue(++count);
                processFile(filename);

	}
        guiForm->treeWidget->clearSelection();
        guiForm->progressBar->hide();
}

void MainWindow::monitor() {

        int timeoutMin = guiForm->monitorTimeout->value();
        setStatus("TimeOut=" + QString::number(timeoutMin));
        setStatus("watchButtonState=" + QString::number(guiForm->watchButton->isChecked()));
        setStatus("timeOut=" + QString::number(timeoutMin*1000));


        if (guiForm->watchButton->isChecked() && timeoutMin > 0) {
            if(timerId) this->killTimer(timerId);
            timerId = this->startTimer(timeoutMin*1000); // time in msec
            setStatus("Staring monitoring service." + QString::number(timerId));
        } else if (timerId!=0) {
            setStatus("Killing minitoring service." + QString::number(timerId));

            this->killTimer(timerId);
            timerId=0;
        }

       updateButtonColors();
}

void MainWindow::getFormValues() { 
        //setStatus("Updating Form Values");
	sourceFolder = guiForm->sourceFolderEdit->text();
	destFolder = guiForm->destFolderEdit->text();
	convertCommand=guiForm->commandEdit->text();
	extention = guiForm->extentionEdit->text();
	maxDayDiff = guiForm->dayDiffBox->value();

        //convert windows backslash to unix forward slash
	sourceFolder = sourceFolder.replace("\\","\057"); // not "\/"
        destFolder = destFolder.replace("\\","\057");
	// didn't just do "/" as it messed up the Mac Emacs C++ mode


}

void MainWindow::updateFileList() { 
        //setStatus("Updating file list");
	directoryList.clear();
        guiForm->monitorDial->setStyleSheet("background: yellow;");
	setCursor(Qt::WaitCursor);
        getDBFileList();
        getFileList(sourceFolder);
	showDataFilesTable();
        processChangedFiles();
	setCursor(Qt::ArrowCursor);
        updateButtonColors();

}

void MainWindow::updateButtonColors() {

    if (timerId) {
        guiForm->monitorDial->setStyleSheet("background: green;");
    } else {
        guiForm->monitorDial->setStyleSheet("background: red;");
    }
}

void MainWindow::processChangedFiles() {
    if(!guiForm->watchButton->isChecked()) return;

    int minFileSize = guiForm->minimumsFileSize->value();
    int waitTime    = guiForm->converter_waitTime->value()*60;

    foreach(QString file, dbFiles.keys()) {

        if ( dbFiles[file] != fileList[file]) {
            QFileInfo fi(file);

            QDateTime now = QDateTime::currentDateTime();
            int ageSec = now.secsTo(fi.lastModified())*-1;
            int oneday = 3600*24; // if file is too old.. don't autocovert

            if (ageSec < oneday && ageSec > waitTime and fi.size() > minFileSize && fi.size() != dbFiles[file]) {
                setStatus(tr("Processing changed file: %1 ").arg(file));
                processFile(file);
            }
        }
    }

}


void MainWindow::timerEvent(QTimerEvent* event) {
       getFormValues();
       if(checkParameters()) updateFileList();
}


QString MainWindow::createTempPath() {
	//temporary path
	QString tempPathName = QDir::tempPath() + "/tmp" +  QString::number(qrand());
	QDir tempPath(tempPathName);
	if (! tempPath.exists()) { tempPath.mkpath(tempPathName); tempPath=QDir(tempPathName); }
	return tempPathName;
}

void MainWindow::convertFile(QString file) {
	if (checkParameters() == 0) return;
	QFileInfo sourceFileInfo(file);
	QString   sourceFileName = sourceFileInfo.fileName();
	QString   destFileFormat = guiForm->destFileFormatBox->currentText();

	if(! sourceFileInfo.exists()) return;
        QString tempPathName = createTempPath();
        QString tempConvertedFile = tempPathName + "/" + sourceFileName;
        tempConvertedFile.replace(extention,destFileFormat);

	setStatus("TempFile=" + tempConvertedFile );
	QDir tempPath(tempPathName);

	if (tempPath.exists()) {
		QString command = convertCommand.arg(file,tempConvertedFile);
		converter->setSystemCommand(command);
		converter->start();

		while(converter->isRunning()){
                        guiForm->convertButton->setText("Stop");
			QApplication::processEvents();
		}
		markFileConverted(file);
	}
}

void MainWindow::makeBackupCopy(QString file) {

	QFileInfo sourceFileInfo(file);
	QString   sourceFileDir =  sourceFileInfo.absolutePath();
	QString   sourceFileName = sourceFileInfo.fileName();
        setStatus("\n\n\nProcessing " + sourceFileName);

	//source file is no longer available
	if(! sourceFileInfo.exists()) return;


	QString   destFile = file;
	destFile.replace(sourceFolder,destFolder);
        QFileInfo destFileInfo(destFile);
        QString   destFileDir =  destFileInfo.absolutePath();
        QString   destFileName(destFileDir + "/" + sourceFileName);
        QString destFileFormat = guiForm->destFileFormatBox->currentText();
        QString	  convertedFileName = destFile;
        convertedFileName = convertedFileName.replace(extention,destFileFormat);

        setStatus("Source="+sourceFolder);
        setStatus("Dest="+destFolder);
        setStatus("Dest Dir=" + destFileDir);
        setStatus("Source Dir=" + sourceFileDir);
        setStatus("Source File=" + sourceFileName);
        setStatus("Destination File=" + destFileName);


	if (destFile == file) {	//path is identical..
		setStatus("Dest and Source Path are identical.. copy failed");
	}


	//create destination path
	QDir destPath(destFileDir);
	if (! destPath.exists()) {
		bool ok = destPath.mkpath(destFileDir);
		if( !ok ){
			setStatus("Failed to make path=" + destFolder);
			return;
		}
	}

	//copy file if it doesn't exists, size changed, or modification time changed
        bool makeFileCopy = guiForm->makeCopyCheckBox->isChecked();
        if (makeFileCopy) {
            if(! destFileInfo.exists() ||
               destFileInfo.lastModified() != sourceFileInfo.lastModified() ||
               destFileInfo.size()         != sourceFileInfo.size() )
            {
                setStatus("Making file copy");
                QFile::copy(file, destFileName);
         }}

        //convert file
        if (destPath.exists()) {
                setStatus("Running coverstion");

                QString infile = file.replace("\057", "\\"); // # not "\/"
                QString outfile = convertedFileName.replace("\057","\\");
                QString outputdir = destFileDir.replace("\057","\\");


                if (convertCommand.contains("msconvert",Qt::CaseInsensitive)) {
                        outfile = outputdir;
                }

                QString command = convertCommand.arg(infile,outfile);
                converter->setSystemCommand(command);
                converter->start();

                while(converter->isRunning()){
                        guiForm->convertButton->setText("Stop");
                        QApplication::processEvents();
                }
                guiForm->convertButton->setText("Convert");
                markFileConverted(file);
        }

	//refresh destination file information
	destFileInfo = QFileInfo(destFile);
}

void MainWindow::processFile(QString file) {
	if (checkParameters() == 0) return;

        makeBackupCopy(file);
        //convertFile(file);

}


void MainWindow::getFileList(const QString &fromDir) {
    //	qDebug() << "getFileList() " << fromDir;
    QDir d(fromDir);
    if(!d.exists()) return;	//directory doesn't exists

    //check if directory has already been processed
    QFileInfo dirInfo(fromDir);
    if(directoryList.contains(dirInfo.absoluteFilePath()) == true ) return;
    directoryList.insert(dirInfo.absoluteFilePath());

    QStringList filters;
    QFileInfoList list = d.entryInfoList( filters,
                                          QDir::Files | QDir::Dirs | QDir::NoSymLinks  | QDir::Readable,
                                          QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);
    if (list.size() == 0) return;

    int fileCount=0;
    foreach (QFileInfo fi, list ) {
        fileCount++;
        if(fileCount % 10==0) { QApplication::processEvents(); }
        if (fi.isDir() && ! directoryList.contains(fi.absoluteFilePath()) ) {
            if (fi.fileName() == "." || fi.fileName() == ".." ) continue;
            //qDebug() << "Dir=" << fi.absoluteFilePath();
            getFileList(fi.absoluteFilePath()); //recurse
        } else if (fi.isFile()) {
            QString absfilepath=fi.absoluteFilePath();
           // qint64 fsizedate = fi.size() + fi.lastModified().toTime_t();
            qint64 fsize = fi.size();

            if ( absfilepath.endsWith(extention,Qt::CaseInsensitive)) {
                //update filelist
                fileList[absfilepath]=fsize;
                QDateTime now = QDateTime::currentDateTime();

                if((now.daysTo(fi.lastModified()))*-1 < maxDayDiff ){
                    if(!fileList.contains(absfilepath) || !dbFiles.contains(absfilepath)) {
                        insertFileInfo(absfilepath);
                        remoteLogMessage("newfile", absfilepath, fsize,"New File Detected");
                    }
                }
            }
        }
    }
}

void MainWindow::readSettings() {
          settings = new QSettings("mzWatch", "mzWatch Settings");

         if (! settings || settings->status() != QSettings::NoError ) {
             return;
         }

	 QPoint pos = settings->value("pos", QPoint(200, 200)).toPoint();
	 QSize size = settings->value("size", QSize(400, 400)).toSize();
	 resize(size);
	 move(pos);

	 if( ! settings->contains("extention") )
			 settings->setValue("extention",QString("wiff"));

	 if( ! settings->contains("sourceFolder") )
			 settings->setValue("sourceFolder",QString("C:/Analyst Data/Projects/Metabolomics/"));

	 if( ! settings->contains("destFolder") )
			 settings->setValue("destFolder",QString("Y:/Metabolomics/Data/"));

	 if( ! settings->contains("convertCommand") )
			 settings->setValue("convertCommand",QString("mzWiff.exe --mzXML \"%1\" \"%2\" "));

         if( ! settings->contains("monitorTimeout") )
                         settings->setValue("monitorTimeout", 60);


	 if( ! settings->contains("maxDayDiff") )
			 settings->setValue("maxDayDiff", 5);

         if( ! settings->contains("watchButtonState") )
                         settings->setValue("watchButtonState", false);


 }

 void MainWindow::writeSettings() {
	 settings->setValue("pos", pos());
	 settings->setValue("size", size());
	 settings->setValue("geometry", saveGeometry());
	 settings->setValue("extention",extention);
	 settings->setValue("sourceFolder", sourceFolder);
	 settings->setValue("destFolder", destFolder);
	 settings->setValue("convertCommand", convertCommand);
	 settings->setValue("maxdayDiff", maxDayDiff);
         settings->setValue("remoteServerUrl", guiForm->remoteServerUrl->text());
         settings->setValue("instrumentId", guiForm->instrumentId->text());
         settings->setValue("remoteLoging", guiForm->remoteLoging->isChecked());
         settings->setValue("monitorTimeout", guiForm->monitorTimeout->value());
         settings->setValue("watchButtonState", guiForm->watchButton->isChecked());

	 qDebug() << "Settings saved to " << settings->fileName();
 }

 void MainWindow::clearTables() {
     QSqlQuery query(DB);
     bool ok = query.exec("delete from datafiles");
     if(!ok) setStatus(query.lastError().text());
     query.clear();

     dbFiles.clear();
     fileList.clear();
     directoryList.clear();
     showDataFilesTable();
 }

 void MainWindow::closeEvent(QCloseEvent *event) {
        remoteLogMessage("service", "", 0 , "mzWatcher stopped");
         writeSettings();
	  DB.close();
	  event->accept();
 }


 void MainWindow::createTables() {
   setStatus("Creating Tables");
   QSqlQuery query(DB);
   bool ok = query.exec("create table if not exists datafiles(filename varchar(255), fileConverted int, fileAnalyzed int, fileSize int, modTime timestamp );");
   if(!ok) setStatus(query.lastError().text());
   query.clear();
 }


 void MainWindow::insertFileInfo(QString filename) {
	QSqlQuery query(DB);
	QFileInfo fi(filename);
        QString absfilepath=fi.absoluteFilePath();
        query.prepare("insert into datafiles(filename,fileConverted,fileAnalyzed,fileSize,modTime) values(?,0,0,?,?)");
        query.addBindValue(absfilepath);
	query.addBindValue(fi.size());
	query.addBindValue(fi.lastModified().toString("yyyy-MM-dd hh:mm:ss"));
        if(!query.exec()) setStatus("insertFileInfo: " + query.lastError().text());
	query.clear();
}

 void MainWindow::markFileConverted(QString filename) {
         QSqlQuery query(DB);
         QFileInfo fi(filename);
         QString absfilepath=fi.absoluteFilePath();

         query.prepare(tr("update datafiles set fileConverted=1, fileSize=%1 where filename=\"%2\"")
                       .arg(fi.size())
                       .arg(absfilepath));
         // .arg(fi.lastModified().toString("yyyy-MM-dd hh:mm:ss")


         if(!query.exec()) setStatus("SQL ERROR: " + query.lastError().text());
         query.clear();

         remoteLogMessage("fileconverted", absfilepath,fi.size(), "File Converted");
 }


 void MainWindow::getDBFileList() {
     QSqlQuery query(DB);
     query.prepare("select *, strftime('%s','now') - strftime('%s',modTime) from datafiles where filename like \"%" + extention + "%\"");
     if(!query.exec()) { setStatus("Error: showDataFilesTable:" + query.lastError().text()); return; }
     while (query.next()) {
         QString filename = query.value(0).toString();
         int filesize = query.value(3).toInt();
         QString modTime  = query.value(4).toString();
         dbFiles[filename]=filesize;
     }
 }

 void MainWindow::showDataFilesTable() {
	 QSqlQuery query(DB);
         query.prepare("select *, strftime('%s','now') - strftime('%s',modTime) from datafiles where filename like \"%" + extention + "%\"");
         //query.addBindValue(extention);
         if(!query.exec()) { setStatus("Error: showDataFilesTable:" + query.lastError().text()); return; }

	 guiForm->treeWidget->clear();
	 while (query.next()) {
             QString filename = query.value(0).toString();
             int filesize = query.value(3).toInt();
             QString modTime  = query.value(4).toString();
             int age = query.value(5).toInt();

             bool fileChanged=false;
             if (fileList.contains(filename) && fileList[filename] != filesize) {
                    fileChanged=true;

             }


             if (age > maxDayDiff*24*60*60) continue;
                bool converted   = query.value(1).toBool();

                 QTreeWidgetItem *item = new QTreeWidgetItem(guiForm->treeWidget);
                 item->setText(0, query.value(4).toString());
                 item->setText(1, QString::number( fileList[filename]  - filesize ));
                 item->setText(2,filename);
                 if(converted) item->setBackgroundColor(0,Qt::green);

                 if(fileChanged) {
                     item->setBackground(0,Qt::yellow);
                     remoteLogMessage("filesizechange", filename, fileList[filename], "File Size Changed");
                 }
	 }
	 query.clear();
}


 void MainWindow::remoteLogMessage(QString infotype, QString filename, int fileSize, QString msgText) {

//      setStatus(infotype + "| " + filename + " | " + QString::number(fileSize) + " | " + msgText);

     if (guiForm->watchButton->isChecked() == false) return;
     if (guiForm->remoteLoging->isChecked() == false) return;
     if (guiForm->remoteServerUrl->text().isEmpty()) return;
     if (guiForm->instrumentId->text().isEmpty()) return;


     QString serverURL = guiForm->remoteServerUrl->text();
     QString instrumentId = guiForm->instrumentId->text();

     QUrl url(serverURL);
     url.addQueryItem("action",  "writelog");
     url.addQueryItem("infotype", infotype);
     url.addQueryItem("instrumentId", instrumentId);
     url.addQueryItem("filename", filename);
     url.addQueryItem("filesize", QString::number(fileSize));
     url.addQueryItem("msgText",  msgText);


     http.setHost(url.host());
     connectionId = http.get(url.toEncoded());

     setStatus( url.toEncoded());

 }


 void MainWindow::readRemoteData(const QHttpResponseHeader &resp)
 {
     //qDebug() << "readRemoteData() << " << resp.statusCode();

     if (resp.statusCode() == 200 ) { //redirect
         QString response=http.readAll();
     } else {
         http.abort();
     }
 }
