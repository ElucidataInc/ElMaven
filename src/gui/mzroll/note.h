#ifndef NOTE_H
#define NOTE_H

#include "globals.h"
#include "stable.h"


class Note : public QObject, public QGraphicsItem {
    Q_OBJECT
#if QT_VERSION >= 0x040600
    Q_INTERFACES( QGraphicsItem )
#endif

public:
	
        Note(UserNote* note, QGraphicsItem *parent = 0);
	Note(const QString &text, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

	void setEditable(bool f) { editable=f; }
	void setZValue(int x);
	void setBackgroundBrush(QBrush b);
	void setPlainText(const QString & text );
	void setHtml(const QString & text);
	QString toPlainText() { if(_label) return _label->toPlainText(); return QString(); }
	void setPos(float sceneX, float sceneY);
	QRectF boundingRect() const;

	void setExpanded(bool f) { expanded=f; }
        void setUrl(QString url) { _link=url; }
        enum Style { NumberOnly=0, ShortText=1, FullText=2, showNoteIcon=3 };
        void setStyle(Style s) { _style = s; }
        void setTimeoutTime(int time) { _timeOutTime=time; }
        void setRemoteNoteLink(UserNote* note, QString url);


	int noteid;
	double mzmin;
	double mzmax;
	double rt;
	double intensity;

public Q_SLOTS:
	void linkClicked();

Q_SIGNALS:
	void itemChanged(Note*);
	void linkActivated(const QString& link);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
 	void hoverEnterEvent( QGraphicsSceneHoverEvent * event);
        void hoverLeaveEvent( QGraphicsSceneHoverEvent * event);
        void mousePressEvent(QGraphicsSceneMouseEvent *event);
        void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
        void mouseDoubleClickEvent (QGraphicsSceneMouseEvent * event);
	void mouseMoveEvent (QGraphicsSceneMouseEvent*);
        void timerEvent(QTimerEvent *event);
	QPainterPath shape() const;

private:
        void setupGraphicOptions();
        bool hovered;
	bool editable;
	bool expanded;
	
	void showBoundBox();

        QString _link;

	Style _style;
	QGraphicsTextItem* _label;
	QGraphicsPathItem* _labelBox;
	QPainterPath _shape;

        int _elepsedTime;
        int _timerId;
        int _timeOutTime;

};

#endif
