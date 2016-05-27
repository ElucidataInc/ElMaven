#include "note.h"
#include <QDebug>

Note::Note(const QString &text, QGraphicsItem *parent, QGraphicsScene *scene): QGraphicsItem(parent, scene)
{
    setupGraphicOptions();
    setPlainText(text);
    setBackgroundBrush(QBrush(QColor(255, 250, 176, 255)));
}


Note::Note(UserNote* usernote, QGraphicsItem *parent): QGraphicsItem(parent)
{
    setupGraphicOptions();

    if (usernote) {

        QString text = tr("<b>%2</b><br>%3<br> rt:%4").arg(
                usernote->title,
                usernote->description,
                QString::number(usernote->rt,'f',2));

        _label->setFlag(QGraphicsItem::ItemIsSelectable);
        _label->setFlag(QGraphicsItem::ItemIsFocusable);
        _label->setAcceptedMouseButtons(Qt::LeftButton);

        setHtml(text);
        setData(0,QVariant(usernote->noteid));
        //connect(_label,SIGNAL(linkActivated(QString)),SLOT(linkClicked()));
    }
}


void Note::setupGraphicOptions() {
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
   // setFlag(QGraphicsItem::ItemIsMovable);

    setAcceptsHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setHandlesChildEvents(true);


    _style = Note::NumberOnly;

    hovered=false;
    editable=false;
    expanded=false;

    _label = new QGraphicsTextItem(this,scene());
    _labelBox = new QGraphicsPathItem(this,scene());
    _labelBox->stackBefore(_label);

    _timerId=0;
    _elepsedTime=0;
    _timeOutTime=0;
}


QRectF Note::boundingRect() const {
	return _shape.boundingRect();
}

void Note::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	update();
}

QPainterPath Note::shape() const
{
	return _shape;
}

void Note::setPos(float sceneX, float sceneY) { 
	QGraphicsItem::setPos(sceneX,sceneY);
}

void Note::showBoundBox() {
/*
        if (_timeOutTime) {
            if (_timerId) killTimer(_timerId);
            _elepsedTime=0;
            _timerId = startTimer(10);
        }
*/
	_shape = QPainterPath();
	if (!_label) return;

	if(!_labelBox) _labelBox = new QGraphicsPathItem(this);

	if ( expanded || hovered ) {
		_label->show();
		_shape.addRect(_label->boundingRect());
        } else if (_style == showNoteIcon) {
            _label->hide();
            QGraphicsPixmapItem* icon = new QGraphicsPixmapItem(this,this->scene());
            icon->setPixmap(QIcon(":/images/note.png").pixmap(24,24));
            _shape.addRect(icon->boundingRect());
             icon->setPos(-5,-5);
	} else {
            _label->hide();
            _shape.addRect(QRect(-3,-3,6,6));
	}

	if (_labelBox) {
            _labelBox->show();
            _labelBox->setPath(_shape);
            if(_labelBox->scene() != scene()) scene()->addItem(_labelBox);
            _labelBox->setZValue(_label->zValue()-1);
	}
}

void Note::setPlainText(const QString & text ) {
	if ( _label ) { _label->setPlainText(text); }
	showBoundBox();
}

void Note::setHtml(const QString & text ) {
	if ( _label ) { _label->setHtml(text); }
	showBoundBox();
}

void Note::setZValue(int z) {
	QGraphicsItem::setZValue(z);
	if(_labelBox) _labelBox->setZValue(z-1);
}

void Note::setBackgroundBrush(QBrush brush) {
    if(_labelBox) {
        _labelBox->setBrush(brush);
        _labelBox->update();
    }
}

void Note::hoverEnterEvent (QGraphicsSceneHoverEvent*) {
    hovered=true;
    showBoundBox();
    update();
}

void Note::hoverLeaveEvent ( QGraphicsSceneHoverEvent*) { 
    hovered=false;
    showBoundBox();
    update(); 
}

void Note::mouseMoveEvent (QGraphicsSceneMouseEvent* event) {
	QGraphicsItem::mouseMoveEvent(event);
}

//format remote note URL
void Note::setRemoteNoteLink(UserNote* note, QString link) {
    if (note && !link.isEmpty() ) {
        QUrl url(link);
        url.addQueryItem("noteid", QString::number(note->noteid));
        url.addQueryItem("action","editnote");
        _link = url.toString();
    }
}

void Note::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(!_link.isEmpty()) QDesktopServices::openUrl(_link);
};

void Note::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    qDebug() << "Note::mouseReleaseEvent";
};

void Note::mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) {

    if (editable ) {
        bool ok;
        QString text = QInputDialog::getText(0, tr("Edit Note"), tr("Note:"), QLineEdit::Normal, toPlainText(), &ok);

        if ( ok && !text.isEmpty()) {
            _label->setHtml(text);
            emit(itemChanged(this));
        }
        update();
    }
}

void Note::timerEvent(QTimerEvent * event) {
    if ( _timerId && _elepsedTime < _timeOutTime) {
        expanded=true;
        _elepsedTime++;
    } else {
        killTimer(_timerId);
        _elepsedTime=0;
        _timerId=0;
        expanded=false;
    }
}

void Note::linkClicked() {
    qDebug() << "Note::linkClicked";
    if(!_link.isEmpty()) QDesktopServices::openUrl(_link);
   //emit(linkActivated(_link));
}
