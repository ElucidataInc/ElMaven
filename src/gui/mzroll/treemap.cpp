#include "Compound.h"
#include "globals.h"
#include "mainwindow.h"
#include "mzSample.h"
#include "Scan.h"
#include "statistics.h"
#include "tabledockwidget.h"
#include "treemap.h"

TreeMap::TreeMap(MainWindow* mw) { 
	this->mainwindow = mw;
	_table=NULL;
	_root=NULL;
    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::BspTreeIndex);
	setObjectName("Treemap");
}


TreeMap::~TreeMap() {
  if (scene()!=NULL) delete(scene());
}

void TreeMap::setTable(TableDockWidget* t) { 
		_table = t; 
}

void TreeMap::replot() {
    drawMap();
}


void TreeMap::drawMap() { 

    scene()->clear();
	if (_root != NULL) delete(_root);
	if (_table == NULL) return;

    float boxW = 30;
    float boxH = 30;
    float margin=100;


	QList<PeakGroup*> glist = _table->getGroups();
	if (glist.size()==0) return;

	QList<TreemapItem*> treeItems; 
    for (int i=0; i < glist.size(); i++ ) {
        PeakGroup* g = glist[i];
        if ( g ) {
            double area=0;
            for(int j=0; j<g->peaks.size();j++) area += g->peaks[j].peakAreaCorrected;
			if ( area > 0 ) {
					TreemapItem* treeItem = new TreemapItem(log2(area),Qt::gray);
					treeItem->setGroup(g);
					if ( g->hasCompoundLink() ) { Compound* c = g->getCompound(); }
					treeItems << treeItem;
			}
        }
    }

	if(treeItems.size() == 0 ) return;
	_root = new TreemapItem(treeItems);

	int X=20;
	int Y=20;
	int W=width()-20;
	int H=height()-20;
    scene()->setSceneRect(X,Y,W,H);

	QRectF rc(X,Y,W,H);
	double surface[4];
	recurseDrawGraph(_root, rc, true, surface, scene()->height());

	/** TEST TREEMAP
	Item* item1 = new Item(100,Qt::gray);
	Item* item2 = new Item(200,Qt::gray);
	Item* item3=  new Item(150,Qt::gray);
	Item* item4=  new  Item(250,Qt::gray);
	Item* item5 = new Item(100,Qt::red);
	Item* item6 = new Item(50,Qt::blue);
	Item* item7=  new Item(30,Qt::yellow);
	Item* item8=  new  Item(20,Qt::green);
	Item* item9=  new  Item(2,Qt::cyan);
	Item* item10=  new  Item(2,Qt::cyan);
	Item* item11=  new  Item(1,Qt::cyan);
	Item* item12=  new  Item(1,Qt::cyan);

	QList<Item*> itemsY; 
	itemsY << item1 << item2 << item3 << item4;
	Item* tree1 = new Item(itemsY);

	QList<Item*> itemsX; 
	itemsX << item5 << item6 << item7 << item8 << item9 << item10 << item11 << item12;
	Item* tree2 = new Item(itemsX);
	
	QList<Item*> itemsXandY; 
	itemsXandY << tree1 << tree2;

	Item* root =  new Item(itemsXandY);

	QRectF rc(X,Y,W,H);
	double surface[4];
	recurseDrawGraph(root, rc, true, surface, scene()->height());
	*/
}

// The classical squarification method.
//
void TreeMap::drawChildren(TreemapItem *parent, const double *surface, double h)
{
		//qDebug() << "drawChildren() ";

        // Rest rectangle to fill
        QRectF remaining = parent->TmiGetRectangle();

        assert(remaining.width() > 0);
        assert(remaining.height() > 0);

        // Size of rest rectangle
        double remainingSize = parent->TmiGetSize();
        assert(remainingSize > 0);

        // Scale factor
        const double sizePerSquarePixel = (double)parent->TmiGetSize() / remaining.width() / remaining.height();

        // First child for next row
        int head = 0;

        // At least one child left
        while(head < parent->TmiGetChildrenCount())
        {

                assert(remaining.width() > 0);
                assert(remaining.height() > 0);

                // How we devide the remaining rectangle
                bool horizontal = (remaining.width() >= remaining.height());

                // height() of the new row
                const float height = horizontal ? remaining.height() : remaining.width();

                // Square of height in size scale for ratio formula
                const double hh = (height * height) * sizePerSquarePixel;
                assert(hh > 0);

                // Row will be made up of child(rowBegin)...child(rowEnd - 1)
                int rowBegin = head;
                int rowEnd = head;

                // Worst ratio so far
                double worst = DBL_MAX;

                // Maxmium size of children in row
                double rmax = parent->TmiGetChild(rowBegin)->TmiGetSize();

                // Sum of sizes of children in row
                double sum = 0;

                // This condition will hold at least once.
                while(rowEnd < parent->TmiGetChildrenCount())
                {
                        // We check a virtual row made up of child(rowBegin)...child(rowEnd) here.

                        // Minimum size of child in virtual row
                        double rmin = parent->TmiGetChild(rowEnd)->TmiGetSize();

                        // If sizes of the rest of the children is zero, we add all of them
                        if(rmin == 0)
                        {
                                rowEnd = parent->TmiGetChildrenCount();
                                break;
                        }

                        // Calculate the worst ratio in virtual row.
                        // Formula taken from the "Squarified Treemaps" paper.
                        // (http://http://www.win.tue.nl/~vanwijk/)

                        const double ss = ((double)sum + rmin) * ((double)sum + rmin);
                        const double ratio1 = hh * rmax / ss;
                        const double ratio2 = ss / hh / rmin;

                        const double nextWorst = std::max(ratio1, ratio2);

                        // Will the ratio get worse?
                        if(nextWorst > worst)
                        {
                                // Yes. Don't take the virtual row, but the
                                // real row (child(rowBegin)..child(rowEnd - 1))
                                // made so far.
                                break;
                        }

                        // Here we have decided to add child(rowEnd) to the row.
                        sum += rmin;
                        rowEnd++;

                        worst = nextWorst;
                }

                // Row will be made up of child(rowBegin)...child(rowEnd - 1).
                // sum is the size of the row.

                // As the size of parent is greater than zero, the size of
                // the first child must have been greater than zero, too.
                assert(sum > 0);

                // width() of row
                float width = (horizontal ? remaining.width() : remaining.height());
                assert(width > 0);

                if(sum < remainingSize) width = (float)((double)sum / remainingSize * width);
                // else: use up the whole width
                // width may be 0 here.


                // Build the rectangles of children.
                QRectF rc;
                double fBegin;
                if(horizontal)
                {
                        rc.setLeft(remaining.left());
                        rc.setRight(remaining.left() + width);
                        fBegin = remaining.top();
						//qDebug() << "horizontal " << rc << " remaining " << remaining;
                }
                else
                {
                        rc.setTop(remaining.top());
                        rc.setBottom(remaining.top() + width);
                        fBegin = remaining.left();
						//qDebug() << "virtical " << rc << " remaining " << remaining;
                }

                // Now put the children into their places
                for(int i = rowBegin; i < rowEnd; i++)
                {
                        double begin = fBegin;
                        double fraction = (double)(parent->TmiGetChild(i)->TmiGetSize()) / sum;
                        double fEnd = fBegin + fraction * height;
                        double end = fEnd;


                        bool lastChild = (i == rowEnd - 1 || parent->TmiGetChild(i+1)->TmiGetSize() == 0);

                        if(lastChild)
                        {
                                // Use up the whole height
                                end = (horizontal ? remaining.top() + height : remaining.left() + height);
                        }

                        if(horizontal)
                        {
                                rc.setTop(begin);
                                rc.setBottom(end);
								//qDebug() << "\t child horizontal " << rc << " remaining " << remaining;
                        }
                        else
                        {
                                rc.setLeft(begin);
                                rc.setRight(end);
								//qDebug() << "\t child virtical " << rc << " remaining " << remaining;
                        }

                        assert(rc.left() < rc.right());
                        assert(rc.top() < rc.bottom());

						/*
						cerr << "Left=" << rc.left() << " " << remaining.left() << endl;
						cerr << "Right=" << rc.right() << " " << remaining.right() << endl;
                        assert(rc.left() > remaining.left());
                        assert(rc.right() < remaining.right());
                        assert(rc.top() > remaining.top());
                        assert(rc.bottom() < remaining.bottom());
						*/

                        recurseDrawGraph(parent->TmiGetChild(i), rc, false, surface, h);

                        if(lastChild)
                                break;

                        fBegin = fEnd;
                }

                // Put the next row into the rest of the rectangle
                if(horizontal)
                {
                        remaining.setLeft( remaining.left() + width);
                }
                else
                {
                        remaining.setTop( remaining.top() + width );
                }

                remainingSize -= sum;

				/*
                assert(remaining.left() <= remaining.right());
                assert(remaining.top() <= remaining.bottom());
				*/
                assert(remainingSize >= 0);

                head += (rowEnd - rowBegin);

                if(remaining.width() <= 0 || remaining.height() <= 0)
                {
                        if(head < parent->TmiGetChildrenCount())
                        {
                                parent->TmiGetChild(head)->TmiSetRectangle(QRectF(-1, -1, -1, -1));
                        }

                        break;
                }
        }
        //assert(remainingSize == 0);
        //assert(remaining.left() == remaining.right() || remaining.top() == remaining.bottom());
}

void TreeMap::recurseDrawGraph( TreemapItem *item, const QRectF& rc, bool asroot, const double *psurface, double h)
{
        assert(rc.width() >= 0);
        assert(rc.height() >= 0);
        assert(item->TmiGetSize() > 0);

		//qDebug() << "recurseDrawGraph() " << rc;

        item->TmiSetRectangle(rc);

        double surface[4];

        if(item->TmiIsLeaf()) {
                renderLeaf(item, surface);
        } else {
                assert(item->TmiGetChildrenCount() > 0);
                assert(item->TmiGetSize() > 0);
                drawChildren(item, surface, h);
        }
}

void TreeMap::renderLeaf(TreemapItem *item, const double *surface)
{
        QRectF rc = item->TmiGetRectangle();
		QColor color = item->TmiGetGraphColor();
		QPen pen(Qt::black);
		QBrush brush(color);
	    QGraphicsRectItem* rect = scene()->addRect(rc,pen,brush);
		PeakGroup* group = item->getGroup();
		if (group) rect->setData(0, QVariant::fromValue(group));

		qDebug() << "addRect " << rc << " " <<  rect->pos();

		QString tooltip;
		if ( group && group->getCompound() != NULL ) {
                        tooltip = QString(group->getCompound()->name().c_str());
		}
		tooltip += "<br>" + QString::number(item->TmiGetSize());
		rect->setToolTip(tooltip);
		/*
		 QGraphicsTextItem* text = scene()->addText(QString::number(item->TmiGetSize()));
		 int textWidth = text->boundingRect().width();
		 float ratio = rect->boundingRect().width()/(float) textWidth;
		 text->setPos(rc.left()+rc.width()/2-textWidth/2,rc.top()+rc.height()/2 );
		 text->setZValue(5);
	 	 if ( ratio < 1 ) text->scale(ratio,ratio);
		 */
}


QColor TreeMap::getColor(float cellValue, float minValue, float maxValue) { 
	QColor color = Qt::black;

	if (cellValue < 0)  { 
			float intensity=abs(cellValue/minValue);
			color.setHsvF(0.1,intensity,intensity);
	}

	if (cellValue > 0 )  { 
			float intensity=abs(cellValue/maxValue);
			color.setHsvF(0.6,intensity,intensity);
	}
	return color;
}


void TreeMap::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->pos());
        cerr << "Item=" << item << endl;
        if ( item != NULL )  {
			QVariant v = item->data(0);
   			PeakGroup*  group =  v.value<PeakGroup*>();
            if (group != NULL && mainwindow != NULL) {
                mainwindow->setPeakGroup(group);
            }
        }
	}
}

void TreeMap::resizeEvent ( QResizeEvent * event ) {
      QSize newsize = event->size();
	  cerr << "TreeMap:resizeEvent() "  << endl;
	  replot();
	  update();
}
