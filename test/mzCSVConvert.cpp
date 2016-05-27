#include "mzSample.h"

std::string myreplace(const std::string &sin,  std::string toReplace, std::string replaceWith) {
    size_t found = sin.find(toReplace);
    if(found>0) {
        std::string sout=sin;
        return(sout.replace(found, toReplace.length(), replaceWith));
    } 
    return sin;
}

int main(int argc, char *argv[]) {

    for(int i=1; i<argc;i++) {
	    string fileIn(argv[i]);
	    mzSample* sample = new mzSample();
	    sample->loadSample(fileIn.c_str());    //load file

       std::string fileOut = "hit.out";
       //std::string fileOut = myreplace(fileIn,"mzXML","mzCSV");

        if ( fileIn != fileOut ) {
            cerr << "Writing " << fileOut  << endl;
	        sample->writeMzCSV(fileOut.c_str());       //write out file
        }
    }

}
