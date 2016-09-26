#include<QtGui>
#include<QStyle>

class QHistogramSlider : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double maxBound READ maxBoundValue WRITE setMaxBound);
    Q_PROPERTY(double minBound READ minBoundValue WRITE setMinBound);
    Q_PROPERTY(double min READ minValue WRITE setMinValue);
    Q_PROPERTY(double max READ maxValue WRITE setMaxValue);
    Q_PROPERTY(int font READ font WRITE setFontDim);
    Q_PROPERTY(int numPrec READ numPrec WRITE setPrecision);
    Q_PROPERTY(QColor color READ color WRITE setBarColor);


public:
    double minBoundValue() const { return minBound; }
    double maxBoundValue() const { return maxBound; }

    double minValue() const { return minVal; }
    double maxValue() const { return maxVal; }

    int font() const { return fontDim; }
    int numPrec() const { return precision; }
    QColor color() const { return colBar; }


    QHistogramSlider(QWidget *parent = 0);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

Q_SIGNALS:
    void valueChanged(double);

public Q_SLOTS:
	void addDataPoint(QPointF p) { data << p; }
	void clearData() { data.clear(); }
	void recalculatePlotBounds();
    void setMaxValue(double);
    void setMinValue(double);
	void setMinBound(double);
	void setMaxBound(double);
    void setFontDim(int);
    void setPrecision(int);
    void setBarColor(QColor);

Q_SIGNALS:
	void minBoundChanged(double);
	void maxBoundChanged(double);
	void minValueChanged(double);
	void maxValueChanged(double);

protected:

    void paintEvent(QPaintEvent *);
    void initValue();
    void paintBorder();
    void paintBar(Qt::AlignmentFlag, float boundVal);
    void paintLine();
    void paintValue(Qt::AlignmentFlag, float value);
	void mouseMoveEvent ( QMouseEvent * );
	void mousePressEvent ( QMouseEvent * );
	void mouseReleaseEvent ( QMouseEvent * );
	void resizeEvent(QResizeEvent * );

private:

	QVector<QPointF>data;

    int fontDim;
    int precision;
    double lengthBar; 

    double minVal;
    double maxVal;

	double maxY;
	double minY;

    double maxBound;
    double minBound;
    QColor colBar;

};
