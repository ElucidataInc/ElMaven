#include "PeakDetectorCLI.h"

PeakDetectorCLI::PeakDetectorCLI() {
	parseOptions = new ParseOptions();
}

void PeakDetectorCLI::processOptions(int argc, char* argv[]) {

	//command line options
	const char * optv[] = {
							"a?alignSamples <int>",
							"b?minGoodGroupCount <int>",
							"c?matchRtFlag <int>",
							"d?db <string>",
							"e?processAllSlices <int>",
							"f?pullIsotopes <int>",				//C13(1st bit), S34i(2nd bit), N15i(3rd bit), D2(4th bit)
							"g?grouping_maxRtWindow <float>",
							"h?help",
							"i?minGroupIntensity <float>",
							"I?quantileIntensity <float>",
                            "j?saveEicJson <int>",
							"k?charge <int>",
							"m?model <string>",
							"n?eicMaxGroups <int>",
							"o?outputdir <string>",
							"p?ppmMerge <float>",
							"q?minQuality <float>",
							"Q?quantileQuality <float>",
							"r?rtStepSize <float>",
                            "s?savemzroll <int>",
                            "v?ionizationMode <int>",
							"w?minPeakWidth <int>",
							"x?xml <string>",
							"X|defaultXml",
							"y?eicSmoothingWindow <int>",
							"z?minSignalBaseLineRatio <float>",
							NULL 
	};

	//parse input options
	Options opts(*argv, optv);
	OptArgvIter iter(--argc, ++argv);
	const char * optarg;

	while (const char optchar = opts(iter, optarg)) {
		switch (optchar) {

		case 'a':
			mavenParameters->alignSamplesFlag = true;
			if (atoi(optarg) == 0) mavenParameters->alignSamplesFlag = false;
			break;

		case 'b':
			mavenParameters->minGoodGroupCount = atoi(optarg);
			break;

		case 'c':
        	mavenParameters->compoundRTWindow=atof(optarg);
            mavenParameters->matchRtFlag=true;
            if(mavenParameters->compoundRTWindow==0) mavenParameters->matchRtFlag=false;
            break;

		case 'd':
			mavenParameters->ligandDbFilename = optarg;
			break;

		case 'e':
			mavenParameters->processAllSlices = true;
			if (atoi(optarg) == 0) mavenParameters->processAllSlices = false;
			break;

        case 'f' :
			{
				mavenParameters->pullIsotopesFlag = 0;
				int label = 0;
				label = atoi(optarg);
				if (label > 0) {
					mavenParameters->pullIsotopesFlag = 1;
					mavenParameters->isotopeAtom["ShowIsotopes"] = true;
					if (label & 1) mavenParameters->C13Labeled_BPE = true; mavenParameters->isotopeAtom["C13Labeled_BPE"] = true; 
					if (label & 2) mavenParameters->S34Labeled_BPE = true; mavenParameters->isotopeAtom["S34Labeled_BPE"] = true;
					if (label & 4) mavenParameters->N15Labeled_BPE = true; mavenParameters->isotopeAtom["N15Labeled_BPE"] = true;
					if (label & 8) mavenParameters->D2Labeled_BPE  = true; mavenParameters->isotopeAtom["D2Labeled_BPE"] = true;
				}
			}
			break;

		case 'g':
			mavenParameters->grouping_maxRtWindow = atof(optarg);
			break;

		case 'h':
			opts.usage(cerr, "files ...");
			exit(0);
			break;

		case 'i':
			mavenParameters->minGroupIntensity = atof(optarg);
			break;

		case 'I':
			mavenParameters->quantileIntensity = atof(optarg);
			break;

        case 'j':
        	saveJsonEIC = true;
			if (atoi(optarg) == 0) saveJsonEIC = false;
			break;

		case 'k':
			mavenParameters->charge = atoi(optarg);
			break;

		case 'm':
			clsfModelFilename = optarg;
			break;

        case 'n' :
        	mavenParameters->eicMaxGroups = atoi(optarg);
			break;

		case 'o':
			mavenParameters->outputdir = optarg + string(DIR_SEPARATOR_STR);
			break;
	
		case 'p':
			mavenParameters->ppmMerge = atof(optarg);
			break;

		case 'q':
			mavenParameters->minQuality = atof(optarg);
			break;

		case 'Q':
			mavenParameters->quantileQuality = atof(optarg);
			break;

		case 'r':
			mavenParameters->rtStepSize = atoi(optarg);
			break;

        case 's':
        	saveMzrollFile = true;
			if (atoi(optarg) == 0) saveMzrollFile = false;
			break;

        case 'v' : 
			mavenParameters->ionizationMode = atoi(optarg);
			break;

		case 'w':
			mavenParameters->minNoNoiseObs = atoi(optarg);
			break;

		case 'x':
			if (!optarg) {
				processXML("config.xml");
			} else {
				processXML((char*)optarg);
			}
			break;

		case 'X':
			createXMLFile("config.xml");
			break;

		case 'y':
			mavenParameters->eic_smoothingWindow = atoi(optarg);
			break;

		case 'z':
			mavenParameters->minSignalBaseLineRatio = atof(optarg);
			break;

		default:
			break;
		}
	}

	cerr << "\n\nCommand:  ";

	for (int i = 0; i < argc; i++)
		cerr << argv[i] << " ";

	cerr << "\n\n\n"; 

	if (iter.index() < argc) {
		for (int i = iter.index(); i < argc; i++)
			filenames.push_back(argv[i]);
	}
}

void PeakDetectorCLI::processXML(const char* fileName){
	
	ifstream xmlFile(fileName);

	string file = string(fileName);

	if(file.substr(file.find_last_of(".") + 1) != "xml") {
		
		status = false;
		textStatus += "Not a xml file.\n";
		return;
	}

	if (xmlFile)
	{

		cerr << endl << "Found " << fileName << endl;
		cerr << endl << "Processing..." << endl;

		xml_document doc;
		doc.load_file(fileName, pugi::parse_minimal);
		xml_node argsNode = doc.child("Arguments");

		xml_node optionsArgs = argsNode.child("OptionsDialogArguments");
		xml_node peaksArgs = argsNode.child("PeaksDialogArguments");
		xml_node generalArgs = argsNode.child("GeneralArguments");

		processOptionsArgsXML(optionsArgs);
		processPeaksArgsXML(peaksArgs);
		processGeneralArgsXML(generalArgs);

	} else {

		status = false;
		string errorMsg = "Error Loading file " + (string)fileName + ". File not found. Can't process further.\n";
		errorMsg = errorMsg + "To create a default file pass argument --defaultXml.\n";
		errorMsg = errorMsg + "This will create a default file config.xml into the root folder.\n";

		textStatus += errorMsg;

	}

}

void PeakDetectorCLI::createXMLFile(const char* fileName) {

	Arguments arguments;

    arguments.populateArgs();
    QStringList optionsDialog = arguments.optionsDialogArgs;
    QStringList peakDialog = arguments.peakDialogArgs;
    QStringList general = arguments.generalArgs;


    xml_document doc;
	xml_node args = doc.append_child("Arguments");

    parseOptions->addChildren(args, "OptionsDialogArguments", optionsDialog);
    parseOptions->addChildren(args, "PeaksDialogArguments", peakDialog);
    parseOptions->addChildren(args, "GeneralArguments", general);

    doc.save_file(fileName);

	status = false;

	textStatus = textStatus + "Default file \"" + fileName + "\" created.\n";

}

void PeakDetectorCLI::processOptionsArgsXML(xml_node& optionsArgs) {

	for (xml_node node = optionsArgs.first_child(); node; node = node.next_sibling()) {
		if (strcmp(node.name(),"ionizationMode") == 0) {
			mavenParameters->ionizationMode = atoi(node.attribute("value").value());
		}
		else if (strcmp(node.name(),"charge") == 0) {
			mavenParameters->charge = atoi(node.attribute("value").value());
		}
		else {
			cerr << endl << "Unknown node : " << node.name() << endl;
		}

	}

}

void PeakDetectorCLI::processPeaksArgsXML(xml_node& peaksArgs) {

	for (xml_node node = peaksArgs.first_child(); node; node = node.next_sibling()) {
  
		if (strcmp(node.name(),"minGoodGroupCount") == 0) {

			mavenParameters->minGoodGroupCount = atoi(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"matchRtFlag") == 0) {

        	mavenParameters->compoundRTWindow=atof(node.attribute("value").value());
            mavenParameters->matchRtFlag=true;
            if(mavenParameters->compoundRTWindow==0) mavenParameters->matchRtFlag=false;

		}
		else if (strcmp(node.name(),"Db") == 0) {

			mavenParameters->ligandDbFilename = node.attribute("value").value();

		}
		else if (strcmp(node.name(),"processAllSlices") == 0) {

			mavenParameters->processAllSlices = true;
			if (atoi(node.attribute("value").value()) == 0) mavenParameters->processAllSlices = false;

		}
		else if (strcmp(node.name(),"pullIsotopes") == 0) {

				mavenParameters->pullIsotopesFlag = 0;
				int label = 0;
				label = atoi(node.attribute("value").value());
				if (label > 0) {
					mavenParameters->pullIsotopesFlag = 1;
					mavenParameters->isotopeAtom["ShowIsotopes"] = true;
					if (label & 1) mavenParameters->C13Labeled_BPE = true; mavenParameters->isotopeAtom["C13Labeled_BPE"] = true; 
					if (label & 2) mavenParameters->S34Labeled_BPE = true; mavenParameters->isotopeAtom["S34Labeled_BPE"] = true;
					if (label & 4) mavenParameters->N15Labeled_BPE = true; mavenParameters->isotopeAtom["N15Labeled_BPE"] = true;
					if (label & 8) mavenParameters->D2Labeled_BPE  = true; mavenParameters->isotopeAtom["D2Labeled_BPE"] = true;
				}

		}
		else if (strcmp(node.name(),"grouping_maxRtWindow") == 0) {

			mavenParameters->grouping_maxRtWindow = atof(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"minGroupIntensity") == 0) {

			mavenParameters->minGroupIntensity = atof(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"quantileIntensity") == 0) {

			mavenParameters->quantileIntensity = atof(node.attribute("value").value());
			
		}
		else if (strcmp(node.name(),"model") == 0) {

			clsfModelFilename = node.attribute("value").value();

		}
		else if (strcmp(node.name(),"eicMaxGroups") == 0) {

			mavenParameters->eicMaxGroups = atoi(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"ppmMerge") == 0) {

			mavenParameters->ppmMerge = atof(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"minQuality") == 0) {

			mavenParameters->minQuality = atof(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"quantileQuality") == 0) {

			mavenParameters->quantileQuality = atof(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"rtStepSize") == 0) {

			mavenParameters->rtStepSize = atoi(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"minPeakWidth") == 0) {

			mavenParameters->minNoNoiseObs = atoi(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"eicSmoothingWindow") == 0) {

			mavenParameters->eic_smoothingWindow = atoi(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"minSignalBaseLineRatio") == 0) {

			mavenParameters->minSignalBaseLineRatio = atof(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"quantitationType") == 0) {

			mavenParameters->peakQuantitation = (PeakGroup::QType)atoi(node.attribute("value").value()); //AreaTop=0, Area=1, Height=2, AreaNotCorrected=3
			quantitationType = (PeakGroup::QType)atoi(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"minScanMz") == 0) {

			mavenParameters->minMz = atof(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"maxScanMz") == 0) {

			mavenParameters->maxMz = atof(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"minScanRt") == 0) {

			mavenParameters->minRt = atof(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"maxScanRt") == 0) {

			mavenParameters->maxRt = atof(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"minScanIntensity") == 0) {

			mavenParameters->minIntensity = atof(node.attribute("value").value());

		}
		else if (strcmp(node.name(),"maxScanIntensity") == 0) {

			mavenParameters->maxIntensity = atof(node.attribute("value").value());

		}
		
		else {
			cerr << endl << "Unknown node : " << node.name() << endl;
		}

	}

}

void PeakDetectorCLI::processGeneralArgsXML(xml_node& generalArgs) {

	for (xml_node node = generalArgs.first_child(); node; node = node.next_sibling()) {

		if (strcmp(node.name(),"alignSamples") == 0) {

			mavenParameters->alignSamplesFlag = true;
			if (atoi(node.attribute("value").value()) == 0) mavenParameters->alignSamplesFlag = false;

		}
		else if (strcmp(node.name(),"saveEicJson") == 0) {

        	saveJsonEIC = true;
			if (atoi(node.attribute("value").value()) == 0) saveJsonEIC = false;

		}
		else if (strcmp(node.name(),"outputdir") == 0) {

			mavenParameters->outputdir = node.attribute("value").value() + string(DIR_SEPARATOR_STR);

		}
		else if (strcmp(node.name(),"savemzroll") == 0) {

        	saveMzrollFile = true;
			if (atoi(node.attribute("value").value()) == 0) saveMzrollFile = false;

		}
		else if (strcmp(node.name(),"samples") == 0) {

			string sampleStr = node.attribute("value").value();
			filenames.push_back(sampleStr);
		}
		else {
			cerr << endl << "Unknown node : " << node.name() << endl;
		}

	}

}

void PeakDetectorCLI::loadClassificationModel(string clsfModelFilename) {

	cerr << "Loading classifiation model" << endl;
	cerr << "clsfModelFilename " << clsfModelFilename << endl;
	mavenParameters->clsf = new ClassifierNeuralNet();
	mavenParameters->clsf->loadModel(clsfModelFilename);
}

void PeakDetectorCLI::loadCompoundsFile() {

	//load compound list
	if (!mavenParameters->ligandDbFilename.empty()) {
		mavenParameters->processAllSlices = false;
		cerr << "\nLoading ligand database" << endl;
		int loadCount = DB.loadCompoundCSVFile(mavenParameters->ligandDbFilename);
		mavenParameters->compounds = DB.compoundsDB;
		cerr << "Total Compounds Loaded : " << loadCount << endl;
	}

}

void PeakDetectorCLI::loadSamples(vector<string>&filenames) {

	double startLoadingTime = getTime();
	cerr << "\nLoading samples" << endl;

	for (unsigned int i = 0; i < filenames.size(); i++) {
		mzSample* sample = new mzSample();
		sample->loadSample(filenames[i].c_str());
		sample->sampleName = cleanSampleName(filenames[i]);

		if (sample->scans.size() >= 1) {
			mavenParameters->samples.push_back(sample);
			cerr << endl << "Loaded Sample : " << sample->getSampleName() << endl;
		} else {
			if (sample != NULL) {
				delete sample;
				sample = NULL;
			}
		}
	}


	if (mavenParameters->samples.size() == 0) {
		cerr << "Exiting .. nothing to process " << endl;
		exit(1);
	}

	sort(mavenParameters->samples.begin(), mavenParameters->samples.end(),mzSample::compSampleSort);

	cerr << "LoadSamples done: loaded " << mavenParameters->samples.size() << " samples";
	cerr << "\nExecution time (Sample loading) : " << getTime() - startLoadingTime << " seconds \n";

}

void PeakDetectorCLI::saveEICsJson(string filename) {
	ofstream myfile(filename.c_str());
	if (!myfile.is_open()) return;
    myfile << "[\n";
	
	for (int i = 0; i < mavenParameters->allgroups.size(); i++) {
		PeakGroup& grp = mavenParameters->allgroups[i];
		float rtmin = grp.minRt - 3;
		float rtmax = grp.maxRt + 3;

		myfile << "{\n";
		myfile << "\"groupId\": " << i << "," << endl;
		myfile << "\"rtmin\": " << rtmin << "," << endl;
		myfile << "\"rtmax\": " << rtmax << "," << endl;
		myfile << "\"eics\": [ " << endl;
		vector<EIC*> eics = getEICs(rtmin, rtmax, grp); //get EICs

		for(int j=0; j < (eics.size() / grp.peaks.size()); j++ ) {
				myfile << "{\n";
				saveEICJson(myfile, eics[j] ); //save EICs
				myfile << "}\n";
				if ( j < ((eics.size() / grp.peaks.size())-1)) myfile << ",\n";
		}
		myfile << "]" << endl;
		myfile << "}" << endl;
        
        if (i != (mavenParameters->allgroups.size()-1)){
            myfile << ",";        	
        }

		delete_all(eics); //cleanup
	}
	myfile << "]";

	myfile.close();
}

void PeakDetectorCLI::saveEICJson(ofstream& out, EIC* eic) {
	int N = eic->rt.size();
	int count = 0;

		out << "\"label\":" << "\"" << eic->getSample()->sampleName << "\"," << endl;
		out << "\"data\": [";
		out << setprecision(4);
		for(int i=0; i<N; i++) { 
				if (eic->intensity[i]>0) {
					  if(count && i<N) out << ",";
					  out << "[" << eic->rt[i] << "," <<  eic->intensity[i] << "]"; 
					  count++;
				};
		}
		out << "]\n"; 
}

vector<EIC*> PeakDetectorCLI::getEICs(float rtmin, float rtmax, PeakGroup& grp) {
	vector<EIC*> eics;
	for (int i = 0; i < grp.peaks.size(); i++) {
		float mzmin = grp.meanMz - 0.2;
		float mzmax = grp.meanMz + 0.2;
		//cerr <<setprecision(5) << "getEICs: mz:" << mzmin << "-" << mzmax << " rt:" << rtmin << "-" << rtmax << endl;

		for (unsigned int j = 0; j < samples.size(); j++) {
			if (!grp.srmId.empty()) {
				EIC* eic = mavenParameters->samples[j]->getEIC(grp.srmId, mavenParameters->eicType);
				eics.push_back(eic);
			} else {
				EIC* eic = samples[j]->getEIC(mzmin, mzmax, rtmin, rtmax, 1, mavenParameters->eicType, mavenParameters->filterline);
				eics.push_back(eic);
			}
		}
	}
	return (eics);
}

string PeakDetectorCLI::cleanSampleName(string sampleName) {
        QString out(sampleName.c_str());
        out.replace(QRegExp(".*/"),"");
        out.replace(QRegExp(".*\\"),"");

        QStringList fileExtensions = QStringList()
                <<"\\.mzCSV$"<<"\\.mzdata$"<<"\\.mzXML$"<<"\\.mzML$"<<"\\.mz5$"<<"\\.pepXML$"<<"\\.xml$"<<"\\.cdf$"<<"\\.raw$";
        //copied from maven_stable/mzfileio.cpp

        Q_FOREACH (const QString& s, fileExtensions) {
            out.replace(QRegExp(s,Qt::CaseInsensitive),"");
        }
        return out.toStdString();
}

void PeakDetectorCLI::writeReport(string setName) {


	//create an output folder
	mzUtils::createDir(mavenParameters->outputdir.c_str());

	cerr << "\nwriteReport " << mavenParameters->allgroups.size() << " groups ";

	//reduce groups
	groupReduction();

	//save Eic Json
	saveJson(setName);

	//save Mzroll File
	saveMzRoll(setName);

	//save output CSV
	saveCSV(setName);

}

void PeakDetectorCLI::groupReduction() {

	if (reduceGroupsFlag) {
		double startGroupReduction = getTime();
		reduceGroups();
		cerr << "\tExecution time (Group reduction) : " << getTime() - startGroupReduction << " seconds \n";
	}
}

void PeakDetectorCLI::saveJson(string setName) {
	if (saveJsonEIC) {

		double startSavingJson = getTime();
		saveEICsJson(mavenParameters->outputdir + setName + ".eics.json");
		cerr << "\tExecution time (Saving Eic Json) : " << getTime() - startSavingJson << " seconds \n";
	}
}

void PeakDetectorCLI::saveMzRoll(string setName) {

	if (saveMzrollFile == true)
	{
		double startSavingMzroll = getTime();
		writePeakTableXML(mavenParameters->outputdir + setName + ".mzroll");
		cerr << "\tExecution time (Saving mzroll)   : " << getTime() - startSavingMzroll << " seconds \n";
	
    }
}

void PeakDetectorCLI::saveCSV(string setName) {

    double startSavingCSV = getTime();

	string fileName = mavenParameters->outputdir + setName + ".csv";

    CSVReports* csvreports = new CSVReports(mavenParameters->samples);
    csvreports->setMavenParameters(mavenParameters);

    if (mavenParameters->allgroups.size() == 0 ) {
		cerr << "Writing to CSV Failed: No Groups found" << endl;
        return;
    }

    if(fileName.empty()) return;

	csvreports->flag = 0;
    
    if (mavenParameters->samples.size() == 0) return;

    csvreports->setUserQuantType(quantitationType);

    //Added to pass into csvreports file when merged with Maven776 - Kiran
    bool includeSetNamesLines=true;
	csvreports->openGroupReport(fileName, includeSetNamesLines);

    for(int i=0; i<mavenParameters->allgroups.size(); i++ ) {
		PeakGroup& group = mavenParameters->allgroups[i];
		csvreports->addGroup(&group);
    }
    csvreports->closeFiles();

    if (csvreports->getErrorReport() != "") {
        cerr << endl << "Writing to CSV Failed : " << csvreports->getErrorReport().toStdString() << endl;
    }

    cerr << "\tExecution time (Saving CSV)      : " << getTime() - startSavingCSV << " seconds \n";
}

void PeakDetectorCLI::reduceGroups() {
	sort(mavenParameters->allgroups.begin(), mavenParameters->allgroups.end(), PeakGroup::compMz);
	cerr << "\nreduceGroups(): " << mavenParameters->allgroups.size();
	//init deleteFlag 
	for(unsigned int i=0; i<mavenParameters->allgroups.size(); i++) {
			mavenParameters->allgroups[i].deletedFlag=false;
	}

	for(unsigned int i=0; i<mavenParameters->allgroups.size(); i++) {
		PeakGroup& grup1 = mavenParameters->allgroups[i];
        if(grup1.deletedFlag) continue;
		for(unsigned int j=i+1; j<mavenParameters->allgroups.size(); j++) {
			PeakGroup& grup2 = mavenParameters->allgroups[j];
            if( grup2.deletedFlag) continue;

			float rtoverlap = mzUtils::checkOverlap(grup1.minRt, grup1.maxRt, grup2.minRt, grup2.maxRt );
			float ppmdist = ppmDist(grup2.meanMz, grup1.meanMz);
		    if ( ppmdist > mavenParameters->ppmMerge ) break;

			if (rtoverlap > 0.8 && ppmdist < mavenParameters->ppmMerge) {
				if (grup1.maxIntensity <= grup2.maxIntensity) {
                     grup1.deletedFlag = true;
					 //allgroups.erase(allgroups.begin()+i);
					 //i--;
					 break;
				} else if ( grup1.maxIntensity > grup2.maxIntensity) {
                     grup2.deletedFlag = true;
					 //allgroups.erase(allgroups.begin()+j);
					 //i--;
					// break;
				}
			}
		}
	}
    int reducedGroupCount = 0;
    vector<PeakGroup> allgroups_;
    for(int i=0; i <mavenParameters->allgroups.size(); i++)
    {
        PeakGroup& grup1 = mavenParameters->allgroups[i];
        if(grup1.deletedFlag == false)
        {
            allgroups_.push_back(grup1);
            reducedGroupCount++;
        }
    }
    cerr << "\nReduced count of groups : " << reducedGroupCount << " \n";
    mavenParameters->allgroups = allgroups_;
	cerr << "Done final group count(): " << mavenParameters->allgroups.size() << endl;
}

void PeakDetectorCLI::writeSampleListXML(xml_node& parent) {
	xml_node samplesset = parent.append_child();
	samplesset.set_name("samples");

	for (int i = 0; i < samples.size(); i++) {
		xml_node _sample = samplesset.append_child();
		_sample.set_name("sample");
		_sample.append_attribute("name") = samples[i]->sampleName.c_str();
		_sample.append_attribute("filename") = samples[i]->fileName.c_str();
		_sample.append_attribute("sampleOrder") = i;
		_sample.append_attribute("setName") = "";
		_sample.append_attribute("sampleName") = samples[i]->sampleName.c_str();
	}
}

void PeakDetectorCLI::writePeakTableXML(std::string filename) {

	xml_document doc;
	doc.append_child().set_name("project");
	xml_node peak = doc.child("project");
	peak.append_attribute("Program") = "peakdetector";
	peak.append_attribute("Version") = "Mar112012";

	writeSampleListXML(peak);
	writeParametersXML(peak);

	if (mavenParameters->allgroups.size()) {
		peak.append_child().set_name("PeakGroups");
		xml_node peakgroups = peak.child("PeakGroups");
		for (int i = 0; i < mavenParameters->allgroups.size(); i++) {
			writeGroupXML(peakgroups, &mavenParameters->allgroups[i]);
		}
	}

	doc.save_file(filename.c_str());
}

void PeakDetectorCLI::writeParametersXML(xml_node& parent) {

	xml_node p = parent.append_child();
	p.set_name("PeakDetectionParameters");
	p.append_attribute("alignSamples") = mavenParameters->alignSamplesFlag;
	p.append_attribute("matchRt") = mavenParameters->matchRtFlag;
	p.append_attribute("ligandDbFilename") =
			mavenParameters->ligandDbFilename.c_str();
	p.append_attribute("clsfModelFilename") = clsfModelFilename.c_str();
	p.append_attribute("rtStepSize") = mavenParameters->rtStepSize;
	p.append_attribute("ppmMerge") = mavenParameters->ppmMerge;
	p.append_attribute("eic_smoothingWindow") =
			mavenParameters->eic_smoothingWindow;
	p.append_attribute("grouping_maxRtWindow") =
			mavenParameters->grouping_maxRtWindow;
	p.append_attribute("minGoodGroupCount") = mavenParameters->minGoodGroupCount;
	p.append_attribute("minSignalBlankRatio") =
			mavenParameters->minSignalBlankRatio;
	p.append_attribute("minPeakWidth") = mavenParameters->minNoNoiseObs;
	p.append_attribute("minSignalBaseLineRatio") =
			mavenParameters->minSignalBaseLineRatio;
	p.append_attribute("minGroupIntensity") = mavenParameters->minGroupIntensity;
	p.append_attribute("minQuality") = mavenParameters->minQuality;
}

void PeakDetectorCLI::writeGroupXML(xml_node& parent, PeakGroup* g) {
	if (!g)
		return;

	xml_node group = parent.append_child();
	group.set_name("PeakGroup");

	group.append_attribute("groupId") = g->groupId;
	group.append_attribute("tagString") = g->tagString.c_str();
	group.append_attribute("metaGroupId") = g->metaGroupId;
	group.append_attribute("clusterId") = g->clusterId;
	group.append_attribute("expectedRtDiff") = g->expectedRtDiff;
	group.append_attribute("groupRank") = (float) g->groupRank;
	group.append_attribute("label") = g->label;
	group.append_attribute("type") = (int) g->type();
	group.append_attribute("changeFoldRatio") = (int) g->changeFoldRatio;
	group.append_attribute("changePValue") = (int) g->changePValue;
	if (g->srmId.length())
		group.append_attribute("srmId") = g->srmId.c_str();

	if (g->hasCompoundLink()) {
		Compound* c = g->compound;
		group.append_attribute("compoundId") = c->id.c_str();
		group.append_attribute("compoundDB") = c->db.c_str();
	}

	for (int j = 0; j < g->peaks.size(); j++) {
		Peak& p = g->peaks[j];
		xml_node peak = group.append_child();
		peak.set_name("Peak");
		peak.append_attribute("pos") = ((int) p.pos);
		peak.append_attribute("minpos") = (int) p.minpos;
		peak.append_attribute("maxpos") = (int) p.maxpos;
		peak.append_attribute("rt") = p.rt;
		peak.append_attribute("rtmin") = p.rtmin;
		peak.append_attribute("rtmax") = p.rtmax;
		peak.append_attribute("mzmin") = p.mzmin;
		peak.append_attribute("mzmax") = p.mzmax;
		peak.append_attribute("scan") = (int) p.scan;
		peak.append_attribute("minscan") = (int) p.minscan;
		peak.append_attribute("maxscan") = (int) p.maxscan;
		peak.append_attribute("peakArea") = p.peakArea;
		peak.append_attribute("peakAreaCorrected") = p.peakAreaCorrected;
		peak.append_attribute("peakAreaTop") = p.peakAreaTop;
        peak.append_attribute("peakAreaTopCorrected") = p.peakAreaTopCorrected;
		peak.append_attribute("peakAreaFractional") = p.peakAreaFractional;
		peak.append_attribute("peakRank") = p.peakRank;
		peak.append_attribute("peakIntensity") = p.peakIntensity;
		;
		peak.append_attribute("peakBaseLineLevel") = p.peakBaseLineLevel;
		peak.append_attribute("peakMz") = p.peakMz;
		peak.append_attribute("medianMz") = p.medianMz;
		peak.append_attribute("baseMz") = p.baseMz;
		peak.append_attribute("quality") = p.quality;
		peak.append_attribute("width") = (int) p.width;
		peak.append_attribute("gaussFitSigma") = p.gaussFitSigma;
		peak.append_attribute("gaussFitR2") = p.gaussFitR2;
		peak.append_attribute("groupNum") = p.groupNum;
		peak.append_attribute("noNoiseObs") = (int) p.noNoiseObs;
		peak.append_attribute("noNoiseFraction") = p.noNoiseFraction;
		peak.append_attribute("symmetry") = p.symmetry;
		peak.append_attribute("signalBaselineRatio") = p.signalBaselineRatio;
		peak.append_attribute("groupOverlap") = p.groupOverlap;
		peak.append_attribute("groupOverlapFrac") = p.groupOverlapFrac;
		peak.append_attribute("localMaxFlag") = p.localMaxFlag;
		peak.append_attribute("fromBlankSample") = p.fromBlankSample;
		peak.append_attribute("label") = p.label;
		peak.append_attribute("sample") = p.getSample()->sampleName.c_str();
	}

	if (g->childCount()) {
		for (int i = 0; i < g->children.size(); i++) {
			PeakGroup* child = &(g->children[i]);
			writeGroupXML(group, child);
		}
	}
}

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
double get_cpu_time(){
    return (double)getTime() / CLOCKS_PER_SEC;
}
