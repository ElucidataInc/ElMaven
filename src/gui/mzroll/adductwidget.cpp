#include "adductwidget.h"

AdductWidget::AdductWidget(MainWindow* mw) { 
  _mw = mw;
  _graph = new GraphWidget();
  setObjectName("AdductWidget");
  _graph->setLayoutAlgorithm(GraphWidget::Circular);
  _graph->setNodeSizeScale(0.2);
  _graph->setLabelSizeScale(1.6);
  setWidget(_graph);
  addToolBar();
}

void AdductWidget::addToolBar() {
		QToolBar *toolBar = new QToolBar(this);
		toolBar->setFloatable(false);
		toolBar->setMovable(false);

		QToolButton *btnZoomIn = new QToolButton(toolBar);
		btnZoomIn->setIcon(QIcon(rsrcPath + "/zoomin.png"));
		connect(btnZoomIn, SIGNAL(clicked()), _graph, SLOT(zoomIn()));

		QToolButton *btnZoomOut = new QToolButton(toolBar);
		btnZoomOut->setIcon(QIcon(rsrcPath + "/zoomout.png"));
		connect(btnZoomOut, SIGNAL(clicked()), _graph, SLOT(zoomOut()));

		QToolButton *btnTextZoomIn = new QToolButton(toolBar);
		btnTextZoomIn->setIcon(QIcon(rsrcPath + "/zoomInText.png"));
		connect(btnTextZoomIn, SIGNAL(clicked()), _graph, SLOT(increaseLabelSize()));

		QToolButton *btnTextZoomOut = new QToolButton(toolBar);
		btnTextZoomOut->setIcon(QIcon(rsrcPath + "/zoomOutText.png"));
		connect(btnTextZoomOut, SIGNAL(clicked()), _graph, SLOT(decreaseLabelSize()));

		QToolButton *btnNodeZoomIn = new QToolButton(toolBar);
		btnNodeZoomIn->setIcon(QIcon(rsrcPath + "/zoomInNode.png"));
		connect(btnNodeZoomIn, SIGNAL(clicked()), _graph, SLOT(increaseNodeSize()));

		QToolButton *btnNodeZoomOut = new QToolButton(toolBar);
		btnNodeZoomOut->setIcon(QIcon(rsrcPath + "/zoomOutNode.png"));
		connect(btnNodeZoomOut, SIGNAL(clicked()), _graph, SLOT(decreaseNodeSize()));

		QToolButton *btnEdgeZoomIn = new QToolButton(toolBar);
		btnEdgeZoomIn->setIcon(QIcon(rsrcPath + "/zoomInEdge.png"));
		connect(btnEdgeZoomIn, SIGNAL(clicked()), _graph, SLOT(increaseEdgeSize()));

		QToolButton *btnEdgeZoomOut = new QToolButton(toolBar);
		btnEdgeZoomOut->setIcon(QIcon(rsrcPath + "/zoomOutEdge.png"));
		connect(btnEdgeZoomOut, SIGNAL(clicked()), _graph, SLOT(decreaseEdgeSize()));

		toolBar->addWidget(btnZoomIn);
		toolBar->addWidget(btnZoomOut);
		toolBar->addWidget(btnNodeZoomIn);
		toolBar->addWidget(btnNodeZoomOut);
		toolBar->addWidget(btnEdgeZoomIn);
		toolBar->addWidget(btnEdgeZoomOut);
		toolBar->addWidget(btnTextZoomIn);
		toolBar->addWidget(btnTextZoomOut);
		setTitleBarWidget(toolBar);
}


void AdductWidget::addLinks(float centerMz,int recursionLevel) {
	if (recursionLevel > 2) return;
	if (!_scan) return;
	Scan* scan = _scan;
	float ppm = _mw->getUserPPM();
	mzSample* sample = scan->getSample();
 
	int ionizationMode = scan->getPolarity();
	ionizationMode = _mw->mavenParameters->ionizationMode; //user specified ionization mode

    QVector<float>newMzs;
    float intensity1 = getIntensity(centerMz,ppm);

	//figure out charge
	int chargeCounts[4] = { 0, 0, 0, 0 };
	vector< vector<float> > charges(4);

	for(int c=1; c<4; c++ ) {
		vector<float>matches;
		float pMz=centerMz;

		for(int i=1; i<100; i++ ) { 
			float mz=centerMz-(i*1.0034)/c;
			bool found =_scan->hasMz(mz,ppm);
			//cerr << "Parent Checking : " << mz << " " << c << " " << found << endl;
			if (found) { pMz = mz; } else { break; }
		}
		
		//cerr << "pMz=" << pMz << endl;
		for(int i=1; i<10; i++ ) {
			if ( i != c || i == 1) {
				float mz=pMz+(i*1.0034)/c;
				bool found =_scan->hasMz(mz,ppm);
				//cerr << "Charge Checking : " << mz << " " << c << " " << found << endl;
				if (!found) break;
				chargeCounts[c]++; 
				matches.push_back(mz);
			}
		}
		charges[c]=matches;
	}

	bool maxLikeCharge=1;
	for(int c=2; c<4; c++ ) { if (chargeCounts[c] > chargeCounts[maxLikeCharge] ) { maxLikeCharge=c; } }
	cerr << "addLinks: charge Count" << chargeCounts[0] << " " << chargeCounts[1] << " " << chargeCounts[2] << endl;
	cerr << "Best charge=" << maxLikeCharge << endl;
	printF(charges[maxLikeCharge]);

	//check for possible C13s
	for(int i=-20; i<20; i++ ) {
		if(i==0) continue;
		float pMz=centerMz;
		float mz=pMz+(i*1.0034);
		float mzz=pMz+(i*1.0034)/2;
		float mzzz=pMz+(i*1.0034)/3;

		if (pMz > mz ) swap(mz,pMz);
		QString noteText = tr("C13-[%1]").arg(i);
		mzLink* l = checkConnection(pMz,mz,noteText.toStdString());
		if(l) newMzs << mz;

		if (pMz > mzz ) swap(mzz,pMz);
		if( i % 2 !=0 && mzz!=mz ) {
			QString noteText = tr("C13-[%1] z=2").arg(i);
			mzLink* l = checkConnection(pMz,mzz,noteText.toStdString());
			if(l) newMzs << mzz;
		}

		if (pMz > mzzz ) swap(mzzz,pMz);
		if( i % 3 !=0 && mzzz !=mz ) {
			QString noteText = tr("C13-[%1] z=3").arg(i);
			mzLink* l = checkConnection(pMz,mzzz,noteText.toStdString());
			if(l) newMzs << mzzz;
		}
	}

    for(int i=0; i < DB.fragmentsDB.size(); i++ ) {
		Adduct* frag  = DB.fragmentsDB[i];
    	//if(frag->charge != 0 && SIGN(frag->charge) != SIGN(ionizationMode) ) continue;
        float mzMinus=centerMz-frag->mass;
        float mzPlus =centerMz+frag->mass;
		{
				QString noteText = tr("%1 Fragment").arg(QString(DB.fragmentsDB[i]->name.c_str()));
				mzLink* l = checkConnection(centerMz,mzPlus,noteText.toStdString());
				if(l) newMzs << mzPlus;
		}
 
		{
				QString noteText = tr("%1 Fragment").arg(QString(DB.fragmentsDB[i]->name.c_str()));
				mzLink* l = checkConnection(mzMinus,centerMz,noteText.toStdString());
				if(l) newMzs << mzMinus;
		}

    }

	//parent check
    for(int i=0; i < DB.adductsDB.size(); i++ ) {
    	if ( SIGN(DB.adductsDB[i]->charge) != SIGN(ionizationMode) ) continue;
        float parentMass=DB.adductsDB[i]->computeParentMass(centerMz);
		parentMass += ionizationMode*HMASS;   //adjusted mass
		cerr << DB.adductsDB[i]->name << " " << DB.adductsDB[i]->charge << " " << parentMass << endl;
        if( abs(parentMass-centerMz)>0.1 && scan->hasMz(parentMass,ppm)) {
            QString noteText = tr("Possible Parent %1").arg(QString(DB.adductsDB[i]->name.c_str()));
			float correlation  = sample->correlation(centerMz, parentMass, 5, scan->rt-1, scan->rt+1,
													_mw->mavenParameters->eicType, _mw->mavenParameters->filterline);
            float parentIntensity = getIntensity(parentMass,ppm);

			if ( correlation > 0.3 && !linkExists(centerMz,parentMass,5) && parentIntensity > intensity1) {
					mzLink* l = new mzLink(parentMass,centerMz,noteText.toStdString());
					l->correlation = correlation;
					links.push_back(l);
					addLink(l);
                    newMzs << parentMass;
			}
        }
    }

	//adduct check
    for(int i=0; i < DB.adductsDB.size(); i++ ) {
    	if ( SIGN(DB.adductsDB[i]->charge) != SIGN(ionizationMode) ) continue;
		float parentMass = centerMz-ionizationMode*HMASS;   //adjusted mass
        float adductMass=DB.adductsDB[i]->computeAdductMass(parentMass);

        if( abs(adductMass-centerMz)>0.1 && scan->hasMz(adductMass,ppm)) {
            QString noteText = tr("Adduct %1").arg(QString(DB.adductsDB[i]->name.c_str()));
			float correlation  = sample->correlation(centerMz, adductMass, 5, scan->rt-1, scan->rt+1,
												_mw->mavenParameters->eicType, _mw->mavenParameters->filterline);
            float childIntensity = getIntensity(adductMass,ppm);

			if (correlation > 0.5 && ! linkExists(adductMass, centerMz,5) && childIntensity < intensity1) {
				mzLink* l = new mzLink(centerMz,adductMass,noteText.toStdString());
				l->correlation = correlation;
				links.push_back(l);
				addLink(l);
                newMzs << adductMass;
			}
        }
    }

	/*
    //recursive walk
    processedMzs << centerMz;
    Q_FOREACH (float mz1, newMzs ) {
        Q_FOREACH (float mz2, processedMzs ) if (ppmDist(mz1,mz2) < ppm) continue;
        addLinks(mz1,recursionLevel+1); 
    }
	*/
   //_graph->updateLayout(); 
}


bool AdductWidget::linkExists(float mz1, float mz2, float ppm) {
	Q_FOREACH( mzLink* link, links ) {
		if ( mzUtils::ppmDist(link->mz1,mz1) < ppm  && mzUtils::ppmDist(link->mz2,mz2) < ppm) {
				return true;
		}
		if ( mzUtils::ppmDist(link->mz2,mz1) < ppm && mzUtils::ppmDist(link->mz1,mz2) < ppm) {
				return true;
		}
	} 
	return false;
}


void AdductWidget::setPeak(Peak* peak) { 
     if ( peak == NULL) return;
     Scan* scan = peak->getScan();
     if (scan == NULL) return;
	 _scan = scan;

	links.clear();
	_graph->clear();

	int recursionLevel=0;
	addLinks(peak->peakMz,recursionLevel);
	showGraph();
}

void AdductWidget::expandNode(Node* n) { 
	QString mzString = n->getNote();

	float mz = mzString.toFloat();
	if (mz > 0 ) addLinks(mz,1);
	_graph->updateLayout();
}

void AdductWidget::showLink(Node* n) { 
	if (!n) return;
	QVariant v= n->data(0);
    float mz = v.toFloat();
	if (mz > 0 ) {
        _mw->setMzValue(mz);
        _mw->spectraWidget->setScan(_scan,mz-2,mz+2);
    }
}

float AdductWidget::getIntensity(float mz, float ppm) {
    if (!_scan) return 0;

    float x=0;
    float mzmin = mz - mz/1e6*ppm;
    float mzmax = mz + mz/1e6*ppm;

   vector<int>matches = _scan->findMatchingMzs(mzmin, mzmax);
   for(int i=0; i < matches.size(); i++ ) {
       if ( _scan->intensity[ matches[i] ] > x ) 
           x = _scan->intensity[matches[i]];
   }
   return x;
}

mzLink* AdductWidget::checkConnection(float mz1, float mz2, string note) {

	//cerr << "check: " << mz1 << " " << mz2 << " " << note << endl;
	if(!_scan) return NULL;
	mzSample* sample = _scan->getSample();

	if(!sample) return NULL;
	float ppm = _mw->getUserPPM();

	if (linkExists(mz1,mz2,ppm)) return NULL;

    float intensity1 = getIntensity(mz1,ppm);
    float intensity2 = getIntensity(mz2,ppm);
	if (intensity1 < intensity2) swap(mz1,mz2);

	if (links.size() > 1 ) {
		for(int i=0; i < links.size(); i++ ) {
			if ( sample->correlation(links[i]->mz1, mz2, 5, _scan->rt-1, _scan->rt+1,
				_mw->mavenParameters->eicType, _mw->mavenParameters->filterline) < 0.0 ) return NULL;
			if ( sample->correlation(links[i]->mz1, mz1, 5, _scan->rt-1, _scan->rt+1,
				_mw->mavenParameters->eicType, _mw->mavenParameters->filterline) < 0.0 ) return NULL;
		}
	}

	if( _scan->hasMz(mz2,ppm) ) {
		float correlation  = sample->correlation(mz1, mz2, 5, _scan->rt-1, _scan->rt+1,
												_mw->mavenParameters->eicType, _mw->mavenParameters->filterline);

		if ( correlation > 0.3) {
				mzLink* l = new mzLink(mz1,mz2,note);
				l->correlation = correlation;
				links.push_back(l);
				addLink(l);
				return l;
		}
	}
	return NULL;
}

void AdductWidget::addLink(mzLink* l) {
	if (!l) return;
	if (l->correlation < 0.5) return;

	QString name1=QString::number(l->mz1,'f',3); 
	QString name2=tr("%1 %2").arg(QString::number(l->mz2,'f',3), l->note.c_str());

	Node* n1 = NULL;
	Node* n2 = NULL;

	Q_FOREACH (Node* item, _graph->getNodes(Node::Unassigned)) {
	        QVariant v= item->data(0); float mz = v.toFloat();
            if (n1 && n2) break;
            if ( ppmDist(mz,l->mz1) < 10 ) n1=item;
            if ( ppmDist(mz,l->mz2) < 10 ) n2=item;
    }

	float ppm = _mw->getUserPPM();
	if (!n1) {
		n1 = _graph->addNode(name1.toStdString(),0);
		n1->setVisible(true); 
        n1->setBrush(QColor::fromRgbF(0.2,0.2,0.8,1.0));
        QVariant v(l->mz1);
        n1->setData(0,v);
        n1->setConcentration( getIntensity(l->mz1,ppm)); 
		connect(n1,SIGNAL(nodePressed(Node*)),SLOT(showLink(Node*)));
		connect(n1,SIGNAL(nodeDoubleClicked(Node*)),SLOT(expandNode(Node*)));

	}

	if (!n2) {
		n2 = _graph->addNode(name2.toStdString(),0);
		n2->setVisible(true); 
        n2->setBrush(QColor::fromRgbF(0.2,0.2,0.8,1.0));
        QVariant v(l->mz2);
        n2->setData(0,v);
        n2->setConcentration( getIntensity(l->mz2,ppm)); 
		connect(n2,SIGNAL(nodePressed(Node*)),SLOT(showLink(Node*)));
		connect(n2,SIGNAL(nodeDoubleClicked(Node*)),SLOT(expandNode(Node*)));

	}

	if (n1 && n2 && ! _graph->findEdge(n1,n2) ) {
        QString note = tr("%1 %2").arg(QString(l->note.c_str()), QString::number(l->mz1-l->mz2,'f',3) );
		Edge* e = _graph->addEdge(n1,n2,note.toStdString(),0);
		e->setVisible(true);
		e->setShowNote(false);
		e->setFlux( l->correlation );
	}
}

void AdductWidget::showGraph() {
	_graph->clear();
     for(int i=0; i <links.size(); i++ ) addLink(links[i]);
	_graph->newLayout();
	_graph->resetZoom();
	_graph->update();
	_graph->showLabels();

}
