class QMessageBoxResize: public QMessageBox
{
    Q_OBJECT
    public:
        QMessageBoxResize(QWidget *parent) : QMessageBox(parent) {
            setMouseTracking(true);
            setSizeGripEnabled(true);
        }
        QMessageBoxResize() {
            setMouseTracking(true);
            setSizeGripEnabled(true);
        }

    private:
        virtual bool event(QEvent *e) {
            bool res = QMessageBox::event(e);
            switch (e->type()) {
            case QEvent::MouseMove:
            case QEvent::MouseButtonPress:
                setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
                setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
                if (QTextEdit *textEdit = findChild<QTextEdit *>()) {
                    textEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
                    textEdit->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
                    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                }
            }
            return res;
        }
};