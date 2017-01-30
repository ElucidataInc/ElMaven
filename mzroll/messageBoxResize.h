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
                setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
                if (QWidget *textEdit = findChild<QTextEdit *>()) {
                textEdit->setMaximumHeight(QWIDGETSIZE_MAX);
                }
            }
            return res;
        }
};