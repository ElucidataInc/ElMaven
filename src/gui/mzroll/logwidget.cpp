#include "logwidget.h"

LogWidget::LogWidget(QMainWindow *parent, std::ostream &stream): QDockWidget("Log", parent,Qt::Widget), m_stream(stream) {
    _mainwindow = (MainWindow*) parent;

    setFloating(false);
    setWindowTitle("Log");
    setObjectName("Log");

    QFont font;
    font.setFamily("Helvetica");
    font.setPointSize(8);

    _editor = new QTextEdit(this);
    _editor->setFont(font);
    _editor->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::MinimumExpanding);
    _editor->verticalScrollBar()->triggerAction(QScrollBar::SliderToMaximum);

     //limit log size
    _editor->document()->setMaximumBlockCount(1000);
     m_old_buf = stream.rdbuf();
         stream.rdbuf(this);

    setWidget(_editor);
}

LogWidget::~LogWidget()
 {
  // output anything that is left
  if (!m_string.empty())
   	_editor->append(m_string.c_str());
    m_stream.rdbuf(m_old_buf);
 }

void LogWidget::append(QString text) {
    _editor->append(text);
}



void LogWidget::append(const char* text) {
    _editor->append(text);
}

void LogWidget::clear() {
    _editor->clear();
}

int LogWidget::overflow(int_type v)
 {
  if (v == '\n')
  {
   _editor->append(m_string.c_str());
    m_string.erase(m_string.begin(), m_string.end());
  }
  else
   m_string += v;

  return v;
 }

std::streamsize LogWidget::xsputn(const char *p, std::streamsize n)
 {
  m_string.append(p, p + n);

  int pos = 0;
  while (pos != std::string::npos)
  {
   pos = m_string.find('\n');
   if (pos != std::string::npos)
   {
    std::string tmp(m_string.begin(), m_string.begin() + pos);
    _editor->append(tmp.c_str());
    m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
   }
  }

  return n;
 }
