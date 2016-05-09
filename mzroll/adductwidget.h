/***************************************************************************
 *   Copyright (C) 2008 by melamud,,,   *
 *   emelamud@princeton.edu   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef ADDUCTGUI_H
#define ADDUCTGUI_H

#include "stable.h"
#include "mainwindow.h"
#include "graphwidget.h"

using namespace std;

class AdductWidget: public QDockWidget {
      Q_OBJECT

public:
      AdductWidget(MainWindow*);
      void setPeak(Peak*);

private slots: 
		void showGraph();
		void showLink(Node* n);
		void expandNode(Node* n);
		void addLinks(float centerMz, int recursionLevel);
		void addLink(mzLink* l);
	  	mzLink* checkConnection(float centerMz,float mz,std::string);

private:
      void addToolBar();

      MainWindow* _mw;
	  vector<mzLink*> links;
      QVector<float> processedMzs;
	  GraphWidget* _graph;
	  Scan* _scan;

	  bool linkExists(float mz1, float mz2, float ppm);
      float getIntensity(float mz, float ppm);
      
};

#endif
