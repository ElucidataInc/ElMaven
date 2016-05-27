#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include "stable.h"
#include "mainwindow.h"
#include <iostream>
#include <streambuf>
#include <string>


class LogWidget : public QDockWidget, public std::basic_streambuf<char>
{
    Q_OBJECT
public:
    LogWidget(QMainWindow *parent, std::ostream &stream);
    ~LogWidget();

public slots:
    void clear();
    void append(QString text);
    void append(const char*);

protected:

	virtual int overflow(int_type v);
 	virtual std::streamsize xsputn(const char *p, std::streamsize n);

private:
    QTextEdit* _editor;
    MainWindow* _mainwindow;

	std::ostream &m_stream;
	std::streambuf *m_old_buf;
	std::string m_string;
};

#endif // LOGWIDGET_H
