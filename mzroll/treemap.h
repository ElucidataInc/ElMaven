#ifndef TREEMAPWIDGET_H
#define TREEMAPWIDGET_H

#include "stable.h"
#include "mzSample.h"
#include "mainwindow.h"
#include "tabledockwidget.h"
#include "statistics.h"
#include "tabledockwidget.h"


class TreemapItem
{
		public:
                                TreemapItem(int size, QColor color) { m_size = size; m_color = color; m_group=NULL;}

                                TreemapItem(PeakGroup* g) {
						m_group = g;
						m_color = Qt::gray;
						m_size = 0;
						if (g) 
								for(int j=0; j<g->peaks.size();j++) 
										m_size += g->peaks[j].peakAreaCorrected;

				}

                                TreemapItem(const QList<TreemapItem *>& children) {
						m_size = 0;
						m_group= NULL;
						for(int i = 0; i < children.size(); i++) {
								m_children.push_back(children[i]);
								m_size += (int)children[i]->TmiGetSize();
						}
						qSort(m_children.begin(), m_children.end());
				}

                                ~TreemapItem()
				{
					for(int i = 0; i < m_children.size(); i++) delete m_children[i];
				}

				bool TmiIsLeaf()                const   { return m_children.size() == 0; }
				QRectF TmiGetRectangle()          const   { return m_rect; }
				void TmiSetRectangle(const QRectF& rc)   { m_rect = rc; }
				QColor TmiGetGraphColor()         const   { return m_color; }
				int TmiGetChildrenCount()      const   { return (int)m_children.size(); }
                                TreemapItem *TmiGetChild(int c)         const   { return m_children[c]; }
				double TmiGetSize()               const  { return m_size; }
                                bool operator< (const TreemapItem* b) { return this->TmiGetSize() < b->TmiGetSize(); }
				void setColor(QColor c) { m_color = c; }
				void setGroup(PeakGroup* g) { m_group = g; }
				PeakGroup* getGroup() { return m_group; }

		private:
                                QList<TreemapItem *> m_children;    // Our children
				double m_size;                 // Our size (in fantasy units)
				QColor m_color;           // Our color
				QRectF m_rect;               // Our Rectangle in the treemap
				PeakGroup* m_group;
};


class TreeMap : public QGraphicsView
{
    Q_OBJECT

		public:
				TreeMap(MainWindow* mw);
                ~TreeMap();

		public slots: 
			void setTable(TableDockWidget* t);
            void replot();

		private:
				QColor getColor(float cellValue, float minValue, float maxValue);
				MainWindow* mainwindow;
				TableDockWidget* _table;
                                TreemapItem* _root;

		protected:
				void drawMap();
				void resizeEvent ( QResizeEvent *event );
                void mousePressEvent(QMouseEvent *event);
                                void renderLeaf(TreemapItem *item, const double *surface);
                                void recurseDrawGraph( TreemapItem *item, const QRectF& rc, bool asroot, const double *psurface, double h);
                                void drawChildren(TreemapItem *parent, const double *surface, double h);

};

#endif

