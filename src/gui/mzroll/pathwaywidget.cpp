#include "Compound.h"
#include "Matrix.h"
#include "animationcontrol.h"
#include "background_peaks_update.h"
#include "edge.h"
#include "eicwidget.h"
#include "enzyme_node.h"
#include "mainwindow.h"
#include "mavenparameters.h"
#include "metabolite_node.h"
#include "mzSample.h"
#include "node.h"
#include "pathwaywidget.h"
#include "PeakDetector.h"
#include "projectdockwidget.h"
#include "tinyplot.h"
#include "widgets/qprog.h"

PathwayWidget::PathwayWidget(MainWindow* parent) {
	mw = parent;
	GraphWidget();
	setObjectName("PathwayWidget");

	animationControl = NULL;
	_focusedCompound = NULL;

	_showCofactorsFlag = false;
	_showEnzymesFlag = false;
	_encodeVideo = false;
	_forceUpdate = false;
	_showAtomCoordinatesFlag = false;
	//setAcceptDrops(true);

	setNodeSizeNormalization(GraphWidget::FixedSize);
	/*
	 cofactors <<"CARBON-DIOXIDE" <<"NADH" <<"NAD" <<"ATP" <<"ADP" <<"NADPH" << "NAD+"
	 <<"NADP" <<"OXYGEN-MOLECULE" <<"PROTON" <<"WATER" <<"AMMONIA" <<"HS"
	 <<"SULFATE" <<"SULFITE" <<"PPI" <<"NH3" <<"HYDROGEN-MOLECULE"
	 <<"M_H2O_M" <<"M_CO2_C" <<"M_NA1_E" <<"M_O2_C"
	 <<"M_AMP_C" <<"M_NA1_C" <<"HYDROGEN-PEROXIDE" <<"M_H2O_P" <<"AMP" <<"NADH-P-OR-NOP"
	 <<"M_NADPH_C" <<"NAD-P-OR-NOP" <<"M_H2O_L" <<"M_NADP_C" <<"M_NADH_C" <<"M_COA_C"
	 <<"M_NAD_C" <<"|ACCEPTOR|" <<"|DONOR-H2|" <<"M_H_M" <<"M_PPI_C" <<"M_H_G"
	 <<"AMMONIA" <<"M_H_P" <<"M_ADP_C" <<"ADP" <<"M_PI_C" <<"CO-A" <<"PPI"
	 <<"|PI|" <<"CARBON-DIOXIDE" <<"NADH" <<"NAD" <<"M_ATP_C" <<"ATP"
	 <<"NADPH" <<"NADP" <<"M_H2O_C" <<"OXYGEN-MOLECULE" <<"M_H_C"
	 <<"WATER" <<"PROTON" << "CO2";
	 */

	cofactors << "C00001" //H2O
			<< "C00002" //ATP
			<< "C00003" //NAD+
			<< "C00004" //NADH
			<< "C00005" //NADPH
			<< "C00006" //NADP+
			<< "C00007" //Oxygen
			<< "C00008" //ADP
			<< "C00009" //Orthophosphate
			<< "C00010" //CoA
			<< "C00011" //CO2
			<< "C00013" //Diphosphate
			<< "C00014" //NH3
			<< "C00015" //UDP
			<< "C00020" //AMP

			<< "C00024" //Acetyl-CoA
			<< "C00027" //H2O2
			<< "C00028" //Acceptor
			<< "C00030" //Reduced acceptor
			<< "C00080"; //H+;

	//animations
	_timerId = 0;
	setTimerStep(0);
	setTimerMaxSteps(0);
	setTimerSpeed(50);

	workerThread = new BackgroundPeakUpdate(mw);
	workerThread->setMainWindow(mw);

	connect(workerThread, SIGNAL(finished()), this,
			SLOT(updateCompoundConcentrations()));
	connect(workerThread, SIGNAL(updateProgressBar(QString,int,int)), parent,
			SLOT(setProgressBar(QString, int,int)));

	// animationProgress = new QProg(0, scene());
	// animationProgress->setMinValue(0);
	// animationProgress->setMaxValue(1.0);
	// animationProgress->setValue(0.0);
	// animationProgress->setFlag(QGraphicsItem::ItemIsMovable);
	// animationProgress->setPos(0, 0);
	// animationProgress->scale(.5, .5);
	// animationProgress->hide();

	QSettings* settings = mw->getSettings();
	if (settings->contains("pathwayId")) {
		QString pathwayId = mw->getSettings()->value("pathwayId").toString();
		if (!pathwayId.isEmpty())
			setPathway(pathwayId);
	}

	tinyPlot = new TinyPlot(0, scene());
	tinyPlot->setZValue(1000);
	tinyPlot->setVisible(true);
	tinyPlot->setPos(scene()->width() * 0.10, scene()->height() * 0.10);
	tinyPlot->setFlag(QGraphicsItem::ItemIsMovable);
	tinyPlot->setFlag(QGraphicsItem::ItemIsSelectable);
	tinyPlot->setFlag(QGraphicsItem::ItemIsFocusable);
	tinyPlot->setWidth(300);
	tinyPlot->setHeight(300);
}

void PathwayWidget::showTinyPlot(Node* n) {
	return;
	/*
	 if(n && tinyPlot) {
	 QVector<float>yvaluesC= n->getConcentrations();
	 QVector<float>yvaluesL= n->getLabaledConcentrations();
	 if (yvaluesC.size() == 0 || yvaluesL.size() == 0) { tinyPlot->hide(); return; }

	 if (!tinyPlot->scene()) scene()->addItem(tinyPlot);

	 tinyPlot->clearData();
	 tinyPlot->setTitle(n->getNote());
	 tinyPlot->addData(yvaluesC);
	 tinyPlot->addData(yvaluesL);
	 tinyPlot->update();
	 tinyPlot->setVisible(true);
	 }
	 */
}

void PathwayWidget::hideEmpty() {

	Q_FOREACH (Node* n, nodelist ){
	if (!n->isEnzyme() || !n->isVisible()) continue;
	EnzymeNode* enz = (EnzymeNode*) n;
	Q_FOREACH(Edge* e, enz->edges() ) {
		Node* o = e->sourceNode(); if (o == enz) e->destNode();
		if (o->isMetabolite() ) {
			Compound* c = ((MetaboliteNode*) o)->getCompound();
                        if (c ) {o->hide(); e->hide();}
		}
	}
}
}

void PathwayWidget::clear() {

	if (nodelist.size() == 0)
		return;
	stopSimulation();
	setTimerStep(0);
	setTimerMaxSteps(0);
	ConcMatrix.resize(0, 0);
	LabelingMatrix.resize(0, 0);
	setBackgroundImage(QString());	//empty background image
	if (!scene())
		return;

	// if (animationProgress && animationProgress->scene() == scene()) {
	// 	scene()->removeItem(animationProgress);
	// }
	if (tinyPlot && tinyPlot->scene() == scene()) {
		scene()->removeItem(tinyPlot);
	}
	// if (animationProgress)
	// 	animationProgress->hide();
	GraphWidget::clear();
}

void PathwayWidget::updateConcentrations() {
	checkCompoundExistance();
	updateCompoundConcentrations();
}

void PathwayWidget::recalculateConcentrations() {
	_forceUpdate = true;
	checkCompoundExistance();
	_forceUpdate = false;
}

void PathwayWidget::checkCompoundExistance() {
	//cerr << "PathwayWidget::checkCompoundExistance() force=" << _forceUpdate << endl;

	if (workerThread->isRunning()) {
		workerThread->stop();
		workerThread->wait(10);
	}
	if (workerThread->isRunning())
		return;

	samples = mw->getVisibleSamples();  	//get list of visibleSamples
	sort(samples.begin(), samples.end(), mzSample::compSampleOrder);
	if (samples.size() == 0)
		return;

	if (samples.size() > 1 && animationControl) {
		animationControl->show();
	} else {
		animationControl->hide();
	}

	vector<Compound*> checkList;
	Q_FOREACH(Node* n, nodelist){
	if (! n->isMetabolite() || ! n->isVisible() ) continue;
	Compound* c = ((MetaboliteNode*) n)->getCompound();

        if ( c == NULL) {
		continue;
	} else if (_forceUpdate == true ) {
		checkList.push_back(c);
        } else {
		checkList.push_back(c);
	}
}

//cerr << "PathwayWidget::checkCompoundExistance() compounds=" <<  checkList.size() << endl;

	MavenParameters* mavenParameters = mw->mavenParameters;

	if (checkList.size() > 0) {
		mavenParameters->setSamples(samples);
		mavenParameters->setCompounds(checkList);
		mavenParameters->compoundMassCutoffWindow = mw->getUserMassCutoff();
		workerThread->setRunFunction("computePeaks");

		mavenParameters->minGoodGroupCount = 1;
		mavenParameters->minSignalBlankRatio = 2;
		mavenParameters->minSignalBaseLineRatio = 2;
		mavenParameters->minNoNoiseObs = 2;
		mavenParameters->minGroupIntensity = 0;
		mavenParameters->writeCSVFlag = false;
		mavenParameters->matchRtFlag = true;
		mavenParameters->showProgressFlag = true;
		mavenParameters->pullIsotopesFlag = true;
		mavenParameters->keepFoundGroups = true;

		workerThread->setMavenParameters(mavenParameters);
		workerThread->setPeakDetector(new PeakDetector(mavenParameters));

		workerThread->start();
	}
}

PathwayWidget::~PathwayWidget() {
	delete (workerThread);
}

void PathwayWidget::showConnectedNodes(Node* n, int depth) {
	if (depth > 10)
		return;

	QList<Edge*> edges = n->edges();
	QSet<Node*> cnodes;
	n->setVisible(true);

	for (int i = 0; i < edges.size(); i++) {
		Node* sn = edges[i]->sourceNode();
		Node* dn = edges[i]->destNode();
		if (sn->getId() == dn->getId())
			continue;

		if (sn->molClass() == Node::Enzyme) {
			Q_FOREACH (Edge *se, sn->edges() ){
			if ( se->sourceNode() && !se->sourceNode()->isVisible() ) {
				cnodes.insert(se->sourceNode());
			}
			if ( se->destNode() && !se->destNode()->isVisible() ) {
				cnodes.insert(se->destNode());
			}
		}
	}

		if (dn->molClass() == Node::Enzyme) {
			Q_FOREACH (Edge *de, dn->edges() ){
			if ( de->sourceNode() && !de->sourceNode()->isVisible() ) {
				cnodes.insert(de->sourceNode());
			}
			if ( de->destNode() && !de->destNode()->isVisible() ) {
				cnodes.insert(de->destNode());
			}
		}
	}

		if (sn && !sn->isVisible()) {
			cnodes.insert(sn);
		}
		if (dn && !dn->isVisible()) {
			cnodes.insert(dn);
		}

		edges[i]->setVisible(true);
	}

	Q_FOREACH (Node* n, cnodes){
	Compound* c=NULL;
	if ( n->molClass() == Node::Metabolite ) {
		Compound* c = ((MetaboliteNode*) n)->getCompound();
	}

        if (c!= NULL && cofactorCheck(c->id().c_str())) continue;
	if (n->edges().size() > 20 ) continue;
	n->setVisible(true);
	showConnectedNodes(n,depth+1); 	// recurse
}
	n->setVisible(true);
}

bool PathwayWidget::cofactorCheck(QString id) {
	if (cofactors.contains(id.toUpper()))
		return true;
	return false;
}

void PathwayWidget::setCompound(Compound* c) {
	if (c == NULL)
		return;
	if (c == _focusedCompound)
		return;

        QString compoundName = QString(c->name().c_str());
	QString pathwayId = "CUSTOM_" + compoundName;
	if (setPathway(pathwayId) == true) {
		_pathwayId = pathwayId;
		return;
	}

	//compound already in the pathway view.. ignore..
	Q_FOREACH(Node* n, nodelist){
	if (n->isMetabolite() && n->isVisible() ) {
		MetaboliteNode* m = (MetaboliteNode*) n;
		//cerr << m->getCompound() << " " << c << endl;
		if (m->getCompound() == c) return;
	}
}

        if (c->db() == "KNOWNS") {
                QString compoundId = QString(c->id().c_str());
		QStringList list = compoundId.split("|");
		if (list.size() > 1)
			compoundId = list[0];
		string id = compoundId.toStdString();
		for (int i = 0; i < DB.compoundsDB.size(); i++) {
                        if (DB.compoundsDB[i]->db() == "KEGG"
                                        && DB.compoundsDB[i]->id() == id) {
				c = DB.compoundsDB[i];
				break;
			}
		}
	}

	if (c == _focusedCompound)
		return;

	_focusedCompound = c;
	_pathwayId = pathwayId;

	//cerr << "PathwayWidget():setCompound():" << c->name << " " << c->id <<  " " << c->reactions.size() << " " << c->db << endl;
	clear();

	//set title
        QString title = QString(c->name().c_str()) + " "
                        + QString(c->formula().c_str());
	setTitle(title);

	//get reactions
	vector<string> rids = getCompoundReactions(c);

	//show reactions
	setLayoutAlgorithm(Circular);
	setReactions(rids);
	showLabels(true);
}

void PathwayWidget::itemMoved() {
}

void PathwayWidget::expandOnCompound(Compound* c) {
	if (!c)
		return;
	vector<string> rids = getCompoundReactions(c);
	addReactions(rids);

        Node* n = locateNode(c->id().c_str());
	if (n)
		centerOn(n);
	checkCompoundExistance();

	recursiveDepth(n, 0);
	showLabels(true);
	updateLayout();
	resetZoom();

	/*
	 ensureVisible(n->pos().x(), n->pos().y(), scene()->width(), scene()->height());
	 fitInView(n->pos().x(), n->pos().y(), width(), scene()->height(),Qt::KeepAspectRatio);
	 */
}

vector<string> PathwayWidget::getCompoundReactions(Compound* c0, int depth) {
	vector<string> rids;
	return rids;
}

void PathwayWidget::setTitle(Reaction* r) {
	if (r == NULL)
		return;

	QStringList reactants;
	QStringList products;

	for (unsigned int j = 0; j < r->reactants.size(); j++) {
		if (r->reactants[j] != NULL)
                        reactants << QString(r->reactants[j]->name().c_str());
	}

	for (unsigned int j = 0; j < r->products.size(); j++) {
		if (r->products[j] != NULL)
                        products << QString(r->products[j]->name().c_str());
	}
	QString direction(" => ");
	if (r->reversable)
		direction = " &lt;=&gt; ";
	QString reaction = reactants.join(" + ") + direction + products.join(" + ");
	QString title = tr("<B>%1</B>: %2").arg(title, reaction);
	GraphWidget::setTitle(title);
}

void PathwayWidget::setCompoundFocus(Compound* c) {
	if (c == NULL)
		return;
	if (c == _focusedCompound)
		return;
	_focusedCompound = c;

        QString title = QString(c->name().c_str()) + ": "
                        + QString(c->formula().c_str());
	setTitle(title);

	c = _focusedCompound;

}

void PathwayWidget::setCompoundSelected(Compound* c) {
        Node* n = locateNode(c->id().c_str());
	if (n)
		centerOn(n);
}

void PathwayWidget::showSample(int pos) {
	if (getTimerMaxSteps() == 0)
		return;
	if (samples.size() == 0)
		return;
	if (pos < 0)
		pos = 0;
	if (pos >= getTimerMaxSteps())
		pos = getTimerMaxSteps() - 1;

	QString title(samples[pos]->sampleName.c_str());
	setTitle(title);

	showAnimationStep(((float) pos) / (getTimerMaxSteps() - 1));
}

void PathwayWidget::updateCompoundConcentrations() {

	//cerr << "PathwayWidget::updateCompoundConcentrations() " << endl;
	if (mw->sampleCount() == 0)
		return;
	samples = mw->getVisibleSamples();  	//get list of visibleSamples
	sort(samples.begin(), samples.end(), mzSample::compSampleOrder);

	Q_FOREACH(Node* n, nodelist){
	if (!n->isMetabolite()) continue;
	MetaboliteNode* m = (MetaboliteNode*) n;
        if (m->getCompound() == NULL) continue;
}

	setupAnimationMatrix();
	normalizeNodeSizes(getNodeSizeNormalization());
	showAnimationStep(0);
	scene()->update();
}

void PathwayWidget::setReactions(vector<string>& reactions) {
	clear();
	addReactions(reactions);

	if (!_pathwayId.isEmpty()) {
		if (loadLayout(_pathwayId) == false)
			newLayout();
	} else {
		newLayout();
	}

	checkCompoundExistance();
	updateCompoundConcentrations();

	normalizeNodeSizes(getNodeSizeNormalization());
	layoutCofactors();
}

MetaboliteNode* PathwayWidget::addMetabolite(QString id, Compound* c) {
	Node* n = locateNode(id);
	if (!n || cofactorCheck(id)) {
		//	qDebug() << "addMetabolite() " << id;
		MetaboliteNode* n = new MetaboliteNode(0, scene());
		n->setId(id);
		n->setNote(id);
		if (c) {
			n->setCompound(c);
                        n->setNote(c->name().c_str());
		}
		if (cofactorCheck(id))
			n->setCofactor(true);
		n->setGraphWidget(this);
		nodelist[id] = n;

		if (_showAtomCoordinatesFlag) {
			Molecule2D* mol = DB.getMolecularCoordinates(id);
			if (mol) {
				n->showCoordinates(true);
				n->setMolecularCoordinates(mol);
			}
		} else {
			n->showCoordinates(false);
		}

		if (mw)
			connect(n, SIGNAL(compoundFocused(Compound*)), mw,
					SLOT(setUrl(Compound*)));
		if (c)
			connect(n, SIGNAL(atomSelected(Compound*,int)), this,
					SLOT(startAtomTransformationAnimation(Compound*,int)));

		if (c)
			connect(n, SIGNAL(compoundFocused(Compound*)), this,
					SLOT(setCompoundFocus(Compound*)));
		if (c)
			connect(n, SIGNAL(expandOnCompound(Compound*)), this,
					SLOT(expandOnCompound(Compound*)));
		if (c)
			connect(n, SIGNAL(nodeMoved(Node*)), this,
					SLOT(adjustEnzymePositions()));
		if (c)
			connect(n, SIGNAL(nodePressed(Node*)), this,
					SLOT(showTinyPlot(Node*)));
		return n;
	}
	return (MetaboliteNode*) n;
}

EnzymeNode* PathwayWidget::addEnzyme(QString id, Reaction* r) {
	Node* n = locateNode(id);
	if (!n) {
		EnzymeNode* n = new EnzymeNode(0, scene());
		if (r)
			n->setReaction(r);
		n->setId(id);
		n->setNote(id);
		n->setGraphWidget(this);
		nodelist[id] = n;
		connect(n, SIGNAL(enzymeFocused(Reaction*)), SLOT(setTitle(Reaction*)));
		if (mw)
			connect(n, SIGNAL(enzymeFocused(Reaction*)), mw,
					SLOT(setUrl(Reaction*)));
		return n;
	}
	return (EnzymeNode*) n;
}

EnzymeNode* PathwayWidget::addReaction(Reaction* r) {
	if (r == NULL)
		return NULL;
	if (r->reactants.size() == 0 || r->products.size() == 0)
		return NULL;

	//already exists
	EnzymeNode* enzyme = (EnzymeNode*) locateNode(r->id.c_str());
	if (enzyme)
		return enzyme;

	//qDebug() << "addReaction()" << r->id.c_str();
	enzyme = addEnzyme(r->id.c_str(), r);

	for (unsigned int i = 0; i < r->reactants.size(); i++) {
		Compound* c = r->reactants[i];
		if (!c)
			continue;
                MetaboliteNode* n = addMetabolite(c->id().c_str(), c);
		if (n) {
			Edge* e = addEdge(n, enzyme, r->id, r);
			e->setReversable(r->reversable);
		}
	}

	for (unsigned int i = 0; i < r->products.size(); i++) {
		Compound* c = r->products[i];
		if (!c)
			continue;
                MetaboliteNode* n = addMetabolite(c->id().c_str(), c);
		if (n) {
			Edge* e = addEdge(enzyme, n, r->id, r);
			e->setReversable(r->reversable);
		}
	}
	return enzyme;
}

bool PathwayWidget::setPathway(QString pid) {
	if (pid.isEmpty())
		return false;
	if (pid == _pathwayId)
		return true;

	//qDebug() << "PathwayWidget::setPathway() " << pid << endl;
	clear();
	vector<string> reactionsIds = DB.getPathwayReactions(pid.toStdString());
	if (reactionsIds.size() == 0)
		return false;

	for (int i = 0; i < reactionsIds.size(); i++) {
		if (reactionsIds[i].find("RXN") == string::npos) {
			vector<string> subreactions = DB.getPathwayReactions(
					reactionsIds[i]);
			for (int j = 0; j < subreactions.size(); j++)
				reactionsIds.push_back(subreactions[j]);
		}
	}
	_pathwayId = pid;
	_focusedCompound = NULL;
	mw->getSettings()->setValue("pathwayId", pid);
	setReactions(reactionsIds);
	showLabels(true);
}

bool PathwayWidget::saveLayout() {
	if (_pathwayId.isEmpty())
		return false;

	//QSqlDatabase* db = mw->getLocalDB();

	QSqlDatabase& db = DB.getLigandDB();
	QSqlQuery query(db);
	db.transaction();

	query.prepare("delete from pathways_layout where pathway_id = ?");
	query.addBindValue(_pathwayId);
	query.exec();

	query.prepare(
			"insert into pathways_layout(pathway_id,species_id,x,y) values(?,?,?,?)");
	Q_FOREACH (Node* n, nodelist ){
	QString id = n->getNote();

	if ( n->isMetabolite() ) {
		Compound* c = ((MetaboliteNode*) n)->getCompound();
                if (c) id = QString(c->id().c_str());
	}

	query.addBindValue(_pathwayId);
	query.addBindValue(id);
	query.addBindValue(n->pos().x() );
	query.addBindValue(n->pos().y() );
	if (!query.exec()) qDebug() << query.lastError();
}
	query.clear();

	if (_pathwayId.contains("CUSTOM_")) {
		query.prepare("delete from pathways where pathway_id = ?");
		query.addBindValue(_pathwayId);
		if (!query.exec())
			qDebug() << query.lastError();
		query.clear();

		query.prepare(
				"insert into pathways(pathway_id,pathway_name,reaction_id,database) values(?,?,?,?)");
		Q_FOREACH (Node* n, nodelist ){
		if (n->isEnzyme() ) {
			Reaction* r = ((EnzymeNode*) n)->getReaction();
			query.addBindValue(_pathwayId);
			query.addBindValue("Layout based on "+_pathwayId);
			query.addBindValue(r->id.c_str());
			query.addBindValue("CUSTOM");
			if (!query.exec()) qDebug() << query.lastError();
		}
	}
		query.clear();
	}

	db.commit();
}

bool PathwayWidget::loadLayout(QString pid) {
	//qDebug() << "PathwayWidget::loadLayout() " << pid << endl;

	if (pid.isEmpty())
		return false;
	QSqlDatabase& db = DB.getLigandDB();
	QSqlQuery query(db);
	query.prepare("select * from pathways_layout where pathway_id = ?");
	query.addBindValue(pid);
	query.exec();

	bool loadLayout = false;
	while (query.next()) {
		QString id = query.value(1).toString().simplified();
		float x = query.value(2).toDouble();
		float y = query.value(3).toDouble();
		Node* n = locateNode(id);
		if (n) {
			n->setPos(x, y);
			n->setDepth(1);
			//	qDebug() << "loadLayout() " << id << " " << x << " " << y;
		} else {
			cerr << "Can't find node=" << id.toStdString() << "'" << endl;
		}

		loadLayout = true;
	}
	query.clear();
	if (loadLayout == false)
		return false;

	//cerr << "adjust Enzyme Positions" << endl;
	Q_FOREACH (Node* n, nodelist ){
	int xpos=0; int ypos=0; int count=0;
	if ( n->isEnzyme() && n->pos().x()==0 && n->pos().y()==0 ) {
		Q_FOREACH (Edge* e, n->edges() ) {
			Node* other = e->sourceNode();
			if (e->sourceNode() == n) other=e->destNode();
			if (!other->isCofactor() && other->pos().x()!=0 && other->pos().y()!=0) {
				xpos += other->pos().x();
				ypos += other->pos().y();
				count++;
			}
		}
	}
	if (count) {xpos/=count; ypos/=count; n->setPos(xpos+1,ypos+1);}
	if ( n->pos().x()==0 && n->pos().y() == 0) n->hide();
}

	resetZoom();

	return loadLayout;
}

void PathwayWidget::addReactions(vector<string>& reactionsIds) {
	for (int i = 0; i < reactionsIds.size(); i++) {
		string rxnId = reactionsIds[i];

		//if reaction has not been loaded.. load reaction
		if (!locateNode(rxnId.c_str())) {
			if (DB.reactionIdMap.count(rxnId) > 0) {
				Reaction* r = DB.reactionIdMap[rxnId];
				addReaction(r);
			}
		}

		if (locateNode(rxnId.c_str())) {
			EnzymeNode* enzyme = (EnzymeNode*) nodelist[rxnId.c_str()];

			Q_FOREACH( Edge* e, enzyme->edges() ){
			e->sourceNode()->setVisible(true);
			e->destNode()->setVisible(true);
			e->setVisible(true);
		}
			enzyme->setVisible(true);
		}
	}
	showCofactors(_showCofactorsFlag);
}

void PathwayWidget::layoutCofactors() {
	if (!_showCofactorsFlag)
		return;

	Q_FOREACH (Node* n, nodelist ){
	if (n->isCofactor() ) n->setVisible(_showCofactorsFlag);
}

	Q_FOREACH (Node* n, nodelist ){
	if ( n->isEnzyme() ) {((EnzymeNode*) n)->layoutCofactors();}
}
}

void PathwayWidget::contextMenuEvent(QContextMenuEvent * event) {
	QGraphicsView::contextMenuEvent(event);
	QMenu menu;
	QMenu nodes("Node Sizes");
	QMenu options("Options");
	QMenu layout("Layout");
	QMenu animation("Animation");

	menu.addMenu(&nodes);
	menu.addMenu(&layout);
	menu.addMenu(&options);
	menu.addMenu(&animation);

	//layout
	QAction* b1 = layout.addAction("Save Layout");
	connect(b1, SIGNAL(triggered()), SLOT(saveLayout()));

	//save
	QAction* b3 = layout.addAction("Load Model from File");
	connect(b3, SIGNAL(triggered()), SLOT(loadModelFile()));
	QAction* b4 = layout.addAction("Save Model to File");
	connect(b4, SIGNAL(triggered()), SLOT(saveModelFile()));

	//background image
	QAction* b5 = layout.addAction("Load Background Image");
	connect(b5, SIGNAL(triggered()), SLOT(loadBackgroundImage()));

	//options
	QAction* e = options.addAction("Show Cofactors");
	e->setCheckable(true);
	e->setChecked(_showCofactorsFlag);
	connect(e, SIGNAL(toggled(bool)), SLOT(showCofactors(bool)));

	QAction* e4 = options.addAction("Show Enzymes");
	e4->setCheckable(true);
	e4->setChecked(_showEnzymesFlag);
	connect(e4, SIGNAL(toggled(bool)), SLOT(showEnzymes(bool)));

        MavenParameters* mavenParameters =
            workerThread->peakDetector->getMavenParameters();

	QAction* e2 = options.addAction("Show Isotopes");
	e2->setCheckable(true);
	e2->setChecked(mavenParameters->pullIsotopesFlag);
	connect(e2, SIGNAL(toggled(bool)), SLOT(calculateIsotopes(bool)));

	QAction* e3 = options.addAction("Show Atoms");
	e3->setCheckable(true);
	e3->setChecked(_showAtomCoordinatesFlag);
	connect(e3, SIGNAL(toggled(bool)), SLOT(showAtomCoordinates(bool)));

	QAction* e7 = options.addAction("Show Nodes");
	e7->setCheckable(true);
	e7->setChecked(true);
	connect(e7, SIGNAL(toggled(bool)), SLOT(showNodes(bool)));

	QAction* e5 = options.addAction("Show Edges");
	e5->setCheckable(true);
	e5->setChecked(true);
	connect(e5, SIGNAL(toggled(bool)), SLOT(showEdges(bool)));

	QAction* e6 = options.addAction("Show Labels");
	e6->setCheckable(true);
	e6->setChecked(true);
	connect(e6, SIGNAL(toggled(bool)), SLOT(showLabels(bool)));

	//animation control
	QAction* a1 = animation.addAction("Animation Start");
	connect(a1, SIGNAL(triggered()), SLOT(startSimulation()));
	QAction* a2 = animation.addAction("Animation Stop");
	connect(a2, SIGNAL(triggered()), SLOT(stopSimulation()));

	//node sizes
	QAction* n1 = nodes.addAction("Fixed Node Size");
	QAction* n2 = nodes.addAction("Absolute Intensity  Size");
	QAction* n3 = nodes.addAction("Relative Intensity  Size");
	QAction* n4 = nodes.addAction("Pairwise Difference  Size");

	QSignalMapper signalMapper(this);
	signalMapper.setMapping(n1, FixedSize);
	signalMapper.setMapping(n2, AbsoluteSize);
	signalMapper.setMapping(n3, RelativeSize);
	signalMapper.setMapping(n4, PairwiseSize);

	connect(n1, SIGNAL(triggered()), &signalMapper, SLOT(map()));
	connect(n2, SIGNAL(triggered()), &signalMapper, SLOT(map()));
	connect(n3, SIGNAL(triggered()), &signalMapper, SLOT(map()));
	connect(n4, SIGNAL(triggered()), &signalMapper, SLOT(map()));
	connect(&signalMapper, SIGNAL(mapped(int)), this,
			SLOT(normalizeNodeSizes(int)));

	/*
	 //layout algorithms
	 QAction* l1 = layout.addAction("Spring Layout");
	 QAction* l2 = layout.addAction("Circular Layout");
	 QAction* l3 = layout.addAction("Baloon Layout");
	 QSignalMapper signalMapperL(this);
	 signalMapperL.setMapping(l1, FMMM);
	 signalMapperL.setMapping(l2, Circular);
	 signalMapperL.setMapping(l3, Balloon);
	 connect(l1, SIGNAL(triggered()), &signalMapperL,SLOT(map()));
	 connect(l2, SIGNAL(triggered()), &signalMapperL,SLOT(map()));
	 connect(l3, SIGNAL(triggered()), &signalMapperL,SLOT(map()));
	 connect(&signalMapperL, SIGNAL(mapped(int)), this, SLOT(changeLayoutAlgorithm(int)));
	 */

	QAction* z = menu.addAction("Update");
	connect(z, SIGNAL(triggered()), SLOT(checkCompoundExistance()));
	connect(z, SIGNAL(triggered()), SLOT(updateCompoundConcentrations()));

	QAction* zz = menu.addAction("Recalculate");
	connect(zz, SIGNAL(triggered()), SLOT(recalculateConcentrations()));
	connect(zz, SIGNAL(triggered()), SLOT(updateCompoundConcentrations()));

	QAction* d = menu.addAction("Remove Selected Nodes");
	connect(d, SIGNAL(triggered()), SLOT(removeSelectedNodes()));

	QAction* pdf = menu.addAction("Save To PDF File");
	connect(pdf, SIGNAL(triggered()), SLOT(exportPDF()));

	//show menu
	QAction *selectedAction = menu.exec(event->globalPos());
}

void PathwayWidget::loadModelFile(QString filename) {

	QFile data(filename);
	if (!data.open(QFile::ReadOnly)) {
		QErrorMessage errDialog(this);
		errDialog.showMessage("File open " + filename + " failed");
		return;
	}
	clear();

	QXmlStreamReader xml(&data);
	QString currentTag;
	QString currentId;
	Reaction* r = NULL;
	EnzymeNode* e = NULL;

	QMap<QString, Compound*> cmap;
	QMap<QString, Reaction*> rmap;
	QMap<QString, QString> concentrations;
	QMap<QString, QString> concentrationsLabeled;
	QMap<QString, QString> fluxes;

	bool _doNewLayout = true;

	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement()) {
			QString id = xml.attributes().value("id").toString().toLower();
			currentTag = xml.name().toString();
			if (xml.name() == "species") {
				QString name = xml.attributes().value("name").toString();
				QString formula = xml.attributes().value("formula").toString();
				double charge =
						xml.attributes().value("charge").toString().toDouble();
				double expectedRt =
						xml.attributes().value("expectedRt").toString().toDouble();
				QString type =
						xml.attributes().value("type").toString().toLower();
				double xpos =
						xml.attributes().value("xcoord").toString().toDouble();
				double ypos =
						xml.attributes().value("ycoord").toString().toDouble();
				int visible =
						xml.attributes().value("visible").toString().toInt();
				if (name.isEmpty())
					name = id;
                //Updated when Merging with Maven776 - Kiran
                                Compound* c = DB.findSpeciesByIdAndName(id.toStdString(),
                                                                        name.toStdString(),
                                                                        DB.ANYDATABASE);
				if (!c) {
					c = new Compound(id.toStdString(), name.toStdString(),
							formula.toStdString(), charge);
				}
                                c->setCharge(charge);
                                c->setExpectedRt (expectedRt);
				cmap[id] = c;
				currentId = id;
				MetaboliteNode* n = addMetabolite(id, c);
				n->setVisible(true);
				if (xpos != 0 || ypos != 0) {
					n->setPos(xpos, ypos);
					_doNewLayout = false;
				}
				if (visible == 0) {
					n->setVisible(false);
				}
			} else if (xml.name() == "reaction") {
				QString name = xml.attributes().value("name").toString();
				if (name.isEmpty())
					name = id;
				int reversable =
						xml.attributes().value("reversable").toString().toInt();
				r = new Reaction("_blank_", id.toStdString(),
						name.toStdString());
				r->setReversable(reversable);
				double xpos =
						xml.attributes().value("xcoord").toString().toDouble();
				double ypos =
						xml.attributes().value("ycoord").toString().toDouble();
				int visible =
						xml.attributes().value("visible").toString().toInt();
				e = addEnzyme(id, r);
				e->setVisible(true);
				if (xpos != 0 || ypos != 0) {
					e->setPos(xpos, ypos);
					_doNewLayout = false;
				}
				if (visible == 0)
					e->setVisible(false);
				rmap[id] = r;
				currentId = id;
			} else if (xml.name() == "reactant") {
				int stoch = xml.attributes().value("stoch").toString().toInt();
				if (stoch == 0)
					stoch == 1;
				if (cmap.count(id) && r != NULL)
					r->addReactant(cmap[id], stoch);
				Node* n = locateNode(id);
				if (cofactorCheck(id))
					n = addMetabolite(id, cmap[id]); //each cofactor is a seperate node
				if (n and e) {
					Edge* edge = addEdge(n, e, "", e->getReaction());
					if (e->isVisible())
						edge->setVisible(true);
				}

			} else if (xml.name() == "product") {
				int stoch = xml.attributes().value("stoch").toString().toInt();
				if (stoch == 0)
					stoch == 1;
				if (cmap.count(id) && r != NULL)
					r->addProduct(cmap[id], stoch);

				Node* n = locateNode(id);
				if (cofactorCheck(id))
					n = addMetabolite(id, cmap[id]);
				if (n and e) {
					Edge* edge = addEdge(e, n, "", e->getReaction());
					if (e->isVisible())
						edge->setVisible(true);
				}
			} else if (xml.name() == "backgroundImage") {
				QString filename =
						xml.attributes().value("filename").toString();
				setBackgroundImage(filename);
			}
		} else if (xml.isEndElement()) {
			//if (xml.name() == "reaction") addReaction(r);
		} else if (xml.isCharacters() && !xml.isWhitespace()) {
			if (currentTag == "concentration" && !currentId.isEmpty()) {
				concentrations[currentId] += xml.text().toString();
			} else if (currentTag == "concentrationsLabeled"
					&& !currentId.isEmpty()) {
				concentrationsLabeled[currentId] += xml.text().toString();
			} else if (currentTag == "fluxes" && !currentId.isEmpty()) {
				fluxes[currentId] += xml.text().toString();
			}
		}
	}
	if (xml.error())
		qWarning() << "XML ERROR:" << xml.lineNumber() << ": "
				<< xml.errorString();

	Q_FOREACH (QString id, concentrations.keys() ){
	QVector<float>v;
	string cid = id.toStdString();
	Q_FOREACH ( QString f, concentrations[id].split(",") ) v << f.toDouble();
	if ( v.size() > 0 ) {
		Node* n = locateNode(id); if (n) n->setConcentrations(v);
	}
}

	Q_FOREACH (QString id, concentrationsLabeled.keys() ){
	QVector<float>v;
	string cid = id.toStdString();
	Q_FOREACH ( QString f, concentrationsLabeled[id].split(",") ) v << f.toDouble();
	if ( v.size() > 0 ) {
		Node* n = locateNode(id); if (n) n->setLabeledConcentrations(v);
	}
}

	Q_FOREACH (QString id, fluxes.keys() ){
	QList<float>v;
	string cid = id.toStdString();
	Q_FOREACH ( QString f, fluxes[id].split(",") ) v << f.toDouble();
	if ( v.size() > 0 ) {
		EnzymeNode* n = (EnzymeNode*) locateNode(id); if (n) n->setFluxes(v);
	}
}

	scene()->setSceneRect(scene()->itemsBoundingRect());
	setupAnimationMatrix();
	resetZoom();
	if (_doNewLayout)
		newLayout();
	showLabels();
	scene()->update();
}

void PathwayWidget::saveModelFile(QString filename) {

	if (filename.isEmpty())
		return;

	QString outputStringXML;
	QXmlStreamWriter stream(&outputStringXML);
	stream.setAutoFormatting(true);
	stream.writeStartElement("model");

	QList<Node*> metabolites;
	QList<Node*> enzymes;

	Q_FOREACH( Node* node, nodelist ){
	if (node->isEnzyme() ) enzymes << node; else metabolites << node;
}

	Q_FOREACH( Node* node, metabolites ){
	if ( node->isMetabolite() ) {
		Compound* c = ((MetaboliteNode*) node)->getCompound();
		stream.writeStartElement("species");
		stream.writeAttribute("id", node->getId() );
		stream.writeAttribute("name", node->getNote() );
		stream.writeAttribute("type", "metabolite");
		stream.writeAttribute("visible",QString::number(node->isVisible()));

		if ( node->pos().x() != 0 || node->pos().y() != 0 ) {
			stream.writeAttribute("xcoord",QString::number(node->pos().x()));
			stream.writeAttribute("ycoord",QString::number(node->pos().y()));
		}

		if (c) {
                stream.writeAttribute("formula",c->formula().c_str());
                stream.writeAttribute("expectedRt",QString::number(c->expectedRt()));
			//stream.writeAttribute("name",c->name.c_str());
		}

		QVector<float> concentrations = node->getConcentrations();
		if (concentrations.size() > 0 ) {
			QStringList concentrationsList;
			Q_FOREACH(float v,concentrations) concentrationsList << QString::number(v,'f',5);
			stream.writeStartElement("concentration");
			stream.writeCharacters(concentrationsList.join(","));
			stream.writeEndElement();
		}

		QVector<float> concentrationsLabeled = node->getLabaledConcentrations();
		if (concentrationsLabeled.size() > 0 ) {
			QStringList concentrationsList;
			Q_FOREACH(float v,concentrationsLabeled) concentrationsList << QString::number(v,'f',5);
			stream.writeStartElement("concentrationsLabeled");
			stream.writeCharacters(concentrationsList.join(","));
			stream.writeEndElement();
		}
		stream.writeEndElement();
	}
}

	Q_FOREACH( Node* node, enzymes ){
	if ( node->isEnzyme() ) {
		Reaction* r = ((EnzymeNode*) node)->getReaction();
		stream.writeStartElement("reaction");
		stream.writeAttribute("id", node->getId() );
		stream.writeAttribute("name", node->getNote() );
		stream.writeAttribute("visible",QString::number(node->isVisible()));

		if ( node->pos().x() != 0 || node->pos().y() != 0 ) {
			stream.writeAttribute("xcoord",QString::number(node->pos().x()));
			stream.writeAttribute("ycoord",QString::number(node->pos().y()));
		}

		if ( r ) {
			for( unsigned int j=0; j < r->reactants.size(); j++ ) {
				if(r->reactants[j] != NULL) {
					stream.writeStartElement("reactant");
                                        stream.writeAttribute("id",r->reactants[j]->id().c_str());
					stream.writeEndElement();
				}
			}
			for( unsigned int j=0; j < r->products.size(); j++ ) {
				if(r->products[j] != NULL) {
					stream.writeStartElement("product");
                                        stream.writeAttribute("id",r->products[j]->id().c_str());
					stream.writeEndElement();
				}
			}

		}
		stream.writeEndElement();
	}
}

	stream.writeStartElement("backgroundImage");
	stream.writeAttribute("filename", getBackgroundImageFilename());
	stream.writeEndElement();

	stream.writeEndElement();
	QFile file(filename);
	if (!file.open(QFile::WriteOnly)) {
		QErrorMessage errDialog(this);
		errDialog.showMessage("File open: " + filename + " failed");
		return;
	}
	QTextStream out(&file);
	out << outputStringXML;
	file.close();
}

void PathwayWidget::loadBackgroundImage() {

	QStringList filelist = QFileDialog::getOpenFileNames(this,
			"Select Pathway File To Load", ".",
			"Image Files(*.png *.gif *.jpg *.jpeg *.svg)");
	if (filelist.size() == 0 || filelist[0].isEmpty())
		return;
	setBackgroundImage(filelist[0]);

}

void PathwayWidget::loadModelFile() {

	QStringList filelist = QFileDialog::getOpenFileNames(this,
			"Select Pathway File To Load", ".", "All Files(*.xml)");
	if (filelist.size() == 0 || filelist[0].isEmpty())
		return;
	loadModelFile(filelist[0]);
}

void PathwayWidget::saveModelFile() {
	QString filename = QFileDialog::getSaveFileName(this,
			tr("Save Pathway to a File"));
	if (filename.isEmpty())
		return;
	saveModelFile(filename);

}

int PathwayWidget::loadSpreadsheet(QString filename) {
	QFile data(filename);
	if (data.open(QFile::ReadOnly)) {
		QTextStream stream(&data);
		QString line;
		do {
			line = stream.readLine();
			QStringList fields = line.split(",");

		} while (!line.isNull());
	}
}

void PathwayWidget::showEnzymes(bool flag) {
	_showEnzymesFlag = flag;

	Q_FOREACH (QGraphicsItem *item, scene()->items()){
	if (Node *n = qgraphicsitem_cast<Node *>(item)) {
		if (n->isEnzyme()) {n->showLabel(flag);}
	}
}
	scene()->update();
}

void PathwayWidget::showCofactors(bool flag) {
	_showCofactorsFlag = flag;

	Q_FOREACH (QGraphicsItem *item, scene()->items()){
	if (Node *n = qgraphicsitem_cast<Node *>(item)) {
		if (n->isCofactor()) {
			//               qDebug() << "showCofactors=" << n->getId() << " " << n->getNote();
			n->setVisible(_showCofactorsFlag);
		}
	}
}

	scene()->update();
}

void PathwayWidget::calculateIsotopes(bool flag) {
	if (workerThread) {
            MavenParameters* mavenParameters =
                workerThread->peakDetector->getMavenParameters();
		mavenParameters->pullIsotopesFlag = flag;
	}

}

void PathwayWidget::normalizeNodeSizes(int x) {
	GraphWidget::sizeNormalization xt = (GraphWidget::sizeNormalization) x;
	setNodeSizeNormalization(xt);
	setupAnimationMatrix();
	scene()->update();
}

void PathwayWidget::changeLayoutAlgorithm(int x) {
	GraphWidget::LayoutAlgorithm xt = (GraphWidget::LayoutAlgorithm) x;
	setLayoutAlgorithm(xt);
	newLayout();
	scene()->update();
}

////simulation code

void PathwayWidget::resetSimulation() {

	setupAnimationMatrix();
	if (ConcMatrix.rows() == 0 || ConcMatrix.cols() == 0)
		return;

	for (int i = 0; i < vnodes.size(); i++) {
		vnodes[i]->setConcentration(ConcMatrix(i, 0));
		vnodes[i]->setLabeledConcentration(LabelingMatrix(i, 0));
	}
}

void PathwayWidget::stopSimulation() {
	if (_timerId != 0)
		killTimer(_timerId);
	_timerId = 0;
	// if (animationProgress)
	// 	animationProgress->hide();
	if (_encodeVideo)
		encodeVideo();
	setTimerStep(0);
}

void PathwayWidget::startSimulation() {
	if (_timerId != 0) {
		stopSimulation();
		return;
	}

	cleanTempVideoDir();
	resetSimulation();

	// if (animationProgress)
	// 	animationProgress->show();
	_timerId = startTimer(_timerSpeed); // in ms
}

void PathwayWidget::keyPressEvent(QKeyEvent *event) {

	switch (event->key()) {
	case Qt::Key_H:
		hideEmpty();
		break;
	case Qt::Key_C:
		clear();
		break;
	}

	GraphWidget::keyPressEvent(event);
	scene()->update();
}

void PathwayWidget::timerEvent(QTimerEvent* event) {
	Q_UNUSED(event);

	if (_showAtomCoordinatesFlag && animationQ.size() > 0) {
		showAtomTrasformations();
	}

	if (getTimerMaxSteps() == 0)
		return;
	//qDebug() << "timerEvent() " << getTimerMaxSteps() << " " << getTimerStep();

	if (getTimerStep() > 1) {
		stopSimulation();
		return;
	}
	double increment = 0.1;
	if (getTimerMaxSteps() > 0)
		increment = (1.0 / getTimerMaxSteps()) / 25;
	setTimerStep(getTimerStep() + increment);
	showAnimationStep(getTimerStep());

}

void PathwayWidget::setupAnimationMatrix() {
	//maximum number of steps for simulation
	QList<Node*> visiableNodes; 			//visible nodes
	int animationSteps = 0;

	QList<Node*> metabolites = getNodes(Node::Metabolite);
	Q_FOREACH (Node* node, metabolites ){
	if (node->isVisible() ) {
		visiableNodes << node;
		int steps = node->getMaxConcentrationSteps();
		if ( steps > animationSteps ) animationSteps=steps;
	}
}
	setTimerMaxSteps(animationSteps);
	ConcMatrix.resize(visiableNodes.size(), animationSteps);
	LabelingMatrix.resize(visiableNodes.size(), animationSteps);

	//qDebug() << "setupAnimationMatrix() " << visiableNodes.size() << " " << animationSteps;
	for (int r = 0; r < visiableNodes.size(); r++) {
		float tconc0 = visiableNodes[r]->getConcentration(0);
		float lconc0 = visiableNodes[r]->getLabeledConcentration(0);
		//if ( tconc0 > 0 && lconc0/tconc0 > 0.3 ) continue;
		for (int c = 0; c < animationSteps; c++) {
			float tconc = visiableNodes[r]->getConcentration(c);
			float lconc = visiableNodes[r]->getLabeledConcentration(c);
			ConcMatrix(r, c) = tconc;
			LabelingMatrix(r, c) = lconc;
		}
	}

	//qDebug() << "setupAnimationMatrix() " << ConcMatrix.nCols() << endl;

	if (getNodeSizeNormalization() == GraphWidget::PairwiseSize)
		setupPairWiseComparisonMatrix();

	if (animationControl) {
		if (animationSteps <= 1) {
			animationControl->hide();
		} else {
			animationControl->slider->setMinimum(0);
			animationControl->slider->setMaximum(ConcMatrix.cols() - 1);
		}
	}

	vnodes = visiableNodes;
	//qDebug() << "setupAnimationMatrix() maxConc=" << ConcMatrix.maxValue();
}

void PathwayWidget::setupPairWiseComparisonMatrix() {
	if (ConcMatrix.cols() < 2)
		return;
	if (LabelingMatrix.cols() < 2)
		return;

	int nRows = ConcMatrix.rows();
	int nCols = ConcMatrix.cols();
	int newCols = nCols;

	MatrixXf M(vnodes.size(), newCols);
	MatrixXf L(vnodes.size(), newCols);

	for (int r = 0; r < nRows; r++) {
		for (int c = 0; c < nCols; c++) {
			float conc0 = ConcMatrix(r, 0);
			float conc1 = ConcMatrix(r, 1);
			float conc2 = ConcMatrix(r, c);
			float lab2 = LabelingMatrix(r, c);

			float concRatio = 0;
			if (conc0 > 0 && conc1 > 0)
				concRatio = (conc1 / conc0) * 2;

			M(r, c) = concRatio;
			L(r, c) = (lab2 / conc2) * concRatio;
		}
	}

	ConcMatrix = M;
	LabelingMatrix = L;
	setTimerMaxSteps(newCols);
}

void PathwayWidget::showAnimationStep(float fraction) {

	if (fraction >= 1)
		fraction = 1;
	if (fraction <= 0)
		fraction = 0;

	if (tinyPlot)
		tinyPlot->setCurrentXCoord(fraction);
	// if (animationProgress)
	// 	animationProgress->setValue(fraction);

	if (getTimerMaxSteps() == 0)
		return;

	int step = (int) (fraction * getTimerMaxSteps());	//lower bound
	if (step < 0)
		step = 0;
	if (step > getTimerMaxSteps())
		return;

	float stepFraction = (fraction * getTimerMaxSteps()) - step;

	if (step >= ConcMatrix.cols())
		return;
	if (step >= LabelingMatrix.cols())
		return;

	if (samples.size() > 0 && step < samples.size()) {
		QString title(samples[step]->sampleName.c_str());
		setTitle(title);
	}

	for (int i = 0; i < vnodes.size(); i++) {
		Node* m = vnodes[i];
		if (m == NULL)
			continue;

		float lconc = ConcMatrix(i, step);
		float lfrac = LabelingMatrix(i, step);
		float nconc = lconc;
		float nfrac = lfrac;

		if (step + 1 < ConcMatrix.cols()) {
			nconc = ConcMatrix(i, step + 1);
			nfrac = LabelingMatrix(i, step + 1);
		}

		//if ( m->isMetabolite() ) qDebug() << m->getId() << " " << lconc << " " << lfrac << endl;

		float conc = lconc + (nconc - lconc) * stepFraction;
		float frac = lfrac + (nfrac - lfrac) * stepFraction;

		m->setConcentration(conc);
		m->setLabeledConcentration(frac);

		//m->update();
		//Q_FOREACH (Edge* e, m->edges()) e->update();
	}

	scene()->update();

	if (_encodeVideo) {
		QDir(QDir::tempPath()).mkpath("video/");
		QRectF source = scene()->sceneRect();
		source.adjust(-50, -50, +50, +50);
		QRectF dist = source;	//save size as scene
		QPixmap pixmap(dist.width(), dist.height());
		pixmap.fill(Qt::white);
		QPainter painter(&pixmap);
		painter.setRenderHint(QPainter::Antialiasing);
		scene()->render(&painter);
		painter.end();
		QString index = QString::number(fraction / 1000.00, 'f', 6).right(6);
		pixmap.save(
				QDir::tempPath() + QString("/video/video_%1.png").arg(index),
				"PNG");
	}
}

void PathwayWidget::wheelEvent(QWheelEvent *event) {

	if (scene()->selectedItems().size() > 0) {
		QGraphicsView::wheelEvent(event);
		return;
	} else {
		scaleView(pow((double) 2, -event->delta() / 240.0));
		Q_FOREACH (QGraphicsItem *item, scene()->selectedItems()){
		centerOn(item);
		translate(item->pos().x(), item->pos().y());
		break;
	}
}
}

void PathwayWidget::encodeVideo() {
	QStringList videoFilesFilter;
	videoFilesFilter << "video*.png";
	QFileInfoList filelist = QDir(QDir::tempPath() + "/video/").entryInfoList(
			videoFilesFilter, QDir::Files, QDir::NoSort);
	if (filelist.size() == 0)
		return;

	QString outputFile = QFileDialog::getSaveFileName(this,
			tr("Save Movie to a File"), QDir::tempPath() + "/video/video.wmv",
			tr("Movies (*.wmv *.avi *.mp4 *.mpeg *.mov)"));

	if (outputFile.isEmpty())
		outputFile = QDir::tempPath() + "/video/video.wmv";

	QRectF source = scene()->sceneRect();
	source.adjust(-50, -50, +50, +50);
	//QRectF  source = QRectF(0,0,1200,900);
	QString fps = QString::number(10);
	QString width = QString::number((int) source.width());
	QString height = QString::number((int) source.height());
	//QString commandFFmpeg = QString("ffmpeg -r %4 -y -i \"%1video*.bmp\" -vcodec %3 \"%2\"").

	QString commandFFmpeg =
			QString(
					"mencoder mf://%1*.png -mf fps=%3 -lavcopts vcodec=msmpeg4v2:vbitrate=800 -ovc lavc -o \"%2\"").arg(
					QDir::tempPath() + "/video/").arg(outputFile).arg(fps);

	qDebug() << commandFFmpeg;

	// exec mencoder
	QProcess *processFFmpeg = new QProcess();
	processFFmpeg->setStandardOutputFile(
			QDir::tempPath() + "/video/output.txt");
	processFFmpeg->setStandardErrorFile(QDir::tempPath() + "/video/error.txt");
	//connect(processFFmpeg, SIGNAL(finished(int)), this, SLOT(doVideoCreated(int)));

	cout << commandFFmpeg.toStdString() << endl;
	processFFmpeg->start(commandFFmpeg);

	if (!processFFmpeg->waitForStarted()) {
		processFFmpeg->kill();
		return;
	}
	while (!processFFmpeg->waitForFinished()) {
	};

	cleanTempVideoDir();
	_encodeVideo = false;
}

void PathwayWidget::cleanTempVideoDir() {
	if (_encodeVideo == false)
		return;
	QStringList videoFilesFilter;
	videoFilesFilter << "video*.png";
	QFileInfoList filelist = QDir(QDir::tempPath() + "/video/").entryInfoList(
			videoFilesFilter, QDir::Files, QDir::NoSort);
	if (filelist.size() == 0)
		return;
	Q_FOREACH (QFileInfo file, filelist ){ QFile::remove(file.absoluteFilePath());}
}

void PathwayWidget::exportPDF() {
	const QString fileName = QFileDialog::getSaveFileName(this,
			"Export File Name", QString(), "PDF Documents (*.pdf)");

	if (fileName.isEmpty())
		return;

	QPrinter printer;
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOutputFileName(fileName);
	printer.setFontEmbeddingEnabled(false);
	printer.setPageSize(QPrinter::Letter);
	printer.setOrientation(QPrinter::Landscape);
	printer.setResolution(600);
	QPainter painter(&printer);
	painter.setRenderHint(QPainter::Antialiasing);
	this->render(&painter);

	/*
	 QSvgGenerator generator;
	 generator.setFileName(fileName);
	 generator.setSize(QSize(scene()->width(), scene()->height()));
	 generator.setViewBox(QRect(0, 0, scene()->width(), scene()->height()));
	 generator.setTitle(tr("Pathway"));

	 QPainter painter;
	 painter.begin(&generator);
	 this->render(&painter);
	 painter.end();
	 */

}

void PathwayWidget::getReactionPairs() {
	qDebug() << "PathwayWidget::getReactionParis()";

	//clear list of reaction pairs
	if (rpairs.size()) {
		delete_all(rpairs);
	}

	//construct list of reaction in current view
	QStringList reactionsIds;
	Q_FOREACH (Node* n, nodelist ){
	if(!n->isEnzyme())continue;
	EnzymeNode* enzyme = (EnzymeNode*) n;
	if ( enzyme->getReaction() ) {
		string rid = '"'+enzyme->getReaction()->id +'"';
		reactionsIds << QString(rid.c_str());
	}
}
//query database to get reaction pairs

	QString sql =
			tr("select * from reactions_rpairs where reaction_id in(%1)").arg(
					reactionsIds.join(","));
	//qDebug() << sql;
	QSqlDatabase& db = DB.getLigandDB();
	QSqlQuery query(db);
	query.prepare(sql);
	query.exec();

	while (query.next()) {
		Rpair* x = new Rpair;
		x->reaction_id = query.value(0).toString();
		x->rpair_id = query.value(1).toString();
		x->species1 = query.value(2).toString();
		x->species2 = query.value(3).toString();
		rpairs.push_back(x);
	}

	qDebug() << "getReactionPairs() " << rpairs.size();

}

void PathwayWidget::clearSelectedAtoms() {
	Q_FOREACH( Node* n, nodelist){
	if ((n->isMetabolite() || n->isCofactor()) ) {((MetaboliteNode*) n)->setSelectedAtom(-1);}
}
getReactionPairs();
}

void PathwayWidget::startAtomTransformationAnimation(Compound* c,
		int atomNumber) {
	stopSimulation();
	clearSelectedAtoms();
	animationQ.clear();
	if (c) {
		animationQ[c] = atomNumber;
	}
	_timerId = startTimer(_timerSpeed); // in ms
}

void PathwayWidget::showAtomTrasformations() {
	Q_FOREACH(Compound* c2, animationQ.keys()){
	showAtomTrasformations(c2, animationQ[c2]);
	animationQ.remove(c2);
}
}

void PathwayWidget::showAtomTrasformations(Compound* c, int atomNumber) {
        qDebug() << "showAtomTrasformations() " << c->id().c_str() << " "
			<< atomNumber << " RPAIRS=" << rpairs.size();

	if (!c)
		return;
        QString compoundId(c->id().c_str());
	if (!nodelist.contains(compoundId))
		return;
	Node* sourceNode = nodelist[compoundId];

	for (int i = 0; i < rpairs.size(); i++) {
		Rpair* rpair = rpairs[i];
		QString enzymeId = rpair->reaction_id;

		if (!nodelist.contains(enzymeId))
			continue;
		EnzymeNode* enzyme = (EnzymeNode*) nodelist[enzymeId];
		Reaction* reaction = enzyme->getReaction();

		if (rpair->species1 == compoundId || rpair->species2 == compoundId) {
			QString compoundTwoId =
					(rpair->species2 == compoundId) ?
							rpair->species1 : rpair->species2;
			if (!nodelist.contains(compoundTwoId))
				continue;
			if (!nodelist[compoundTwoId]->isMetabolite())
				continue;

			MetaboliteNode* destNode = (MetaboliteNode*) nodelist[compoundTwoId];
			Compound* destMetabolite = destNode->getCompound();

			QVector<QPoint> atompairs = getEqualentAtoms(rpair->rpair_id);
			qDebug() << rpair->rpair_id << " " << rpair->species1 << " "
					<< rpair->species2;

			bool swapped = false;
			if (rpair->species2 == compoundId) {
				swapped = true;
			}

			Q_FOREACH(QPoint pair, atompairs){
			if (swapped == false && pair.x() == atomNumber) {
				//qDebug() << pair.x() << " => " << pair.y();
				if (destNode->getSelectedAtom() != pair.y()) {
					animationQ[destMetabolite] = pair.y();
					destNode->setSelectedAtom(pair.y());
					destNode->update();
				}
			} else if ( swapped == true && pair.y() == atomNumber) {
				if (destNode->getSelectedAtom() != pair.x()) {
					//qDebug() << pair.x() << " <= " << pair.y();
					destNode->setSelectedAtom(pair.x());
					animationQ[destMetabolite] = pair.x();
					destNode->update();
				}
			}
		}
	}
}

//scene()->update();
}

void PathwayWidget::dropEvent(QDropEvent * event) {
	QGraphicsView::dropEvent(event);

	// qDebug() << "dropEvent:" << event->mimeData()->text();
	// qDebug() << "source: " << event->source()->windowTitle();
	// qDebug() << "type" << event->type();
	// qDebug() << "format" << event->format();
	// qDebug() << "encodedData" << event->encodedData(event->format());

	QByteArray encoded = event->mimeData()->data("BUG");
	QDataStream stream(&encoded, QIODevice::ReadOnly);
	QGraphicsView::dropEvent(event);

	while (!stream.atEnd()) {
		int row, col;
		QMap<int, QVariant> roleDataMap;
		stream >> row >> col >> roleDataMap;
		QList<QVariant> values = roleDataMap.values();

		if (values.size() == 2) {
			QString id = values.at(0).toString();
			QString db = values.at(1).toString();
			vector<Compound*> compounds = DB.findSpeciesByName(id.toStdString(),
					db.toStdString());

			if (compounds.size() > 0) {
				Compound* c = compounds[0];
				MetaboliteNode* n = addMetabolite(id, c);
				n->setVisible(true);
				n->showLabel(true);
				n->update();
				n->setPos(this->mapToScene(event->pos()));
				this->showLabels();
				break;
			}
			event->acceptProposedAction();
		} else {
			qDebug() << "Can't find compound: " << values;
		}
	}
}

void PathwayWidget::dragEnterEvent(QDragEnterEvent *event) {
	QGraphicsView::dragEnterEvent(event);

	if (event->mimeData()->hasFormat(
			"application/x-qabstractitemmodeldatalist")) {
		event->setDropAction(Qt::LinkAction);
		event->acceptProposedAction();
		event->accept();
	} else {
		return;
	}

}

void PathwayWidget::dragLeaveEvent(QDragLeaveEvent *event) {
	QGraphicsView::dragLeaveEvent(event);
	event->accept();

}

void PathwayWidget::dragMoveEvent(QDragMoveEvent *event) {
	QGraphicsView::dragMoveEvent(event);
	event->accept();
}

QVector<QPoint> PathwayWidget::getEqualentAtoms(QString rpairId) {
	QSqlDatabase& db = DB.getLigandDB();
	QSqlQuery query(db);
	QString sql =
			"select atom1n, atom2n, atom1, atom2 from rpairs where rpair_id = ?";
	query.prepare(sql);
	query.addBindValue(rpairId);
	query.exec();

	QVector<QPoint> atompairs;

	while (query.next()) {
		//qDebug() << query.value(0) << " " << query.value(1) << " | " << query.value(2) << " " << query.value(3);
		QPoint pair(query.value(0).toInt() - 1, query.value(1).toInt() - 1);
		atompairs << pair;
	}

	return atompairs;
}
