#include "history.h"
using namespace std;


History::History() { 
  _pos = 0;
}

void History::addToHistory(const mzSlice slice) {

	if ( size() == 0 ) { 
		history.push_back(slice);
		return;
	}

    if (_pos < history.size() && _pos >= 0 ) {
        if ( history[_pos].mzmin == slice.mzmin && history[_pos].mzmax == slice.mzmax &&
                history[_pos].rtmin == slice.rtmin && history[_pos].rtmax == slice.rtmax ) {
            return;
        }
    }

	//cerr << "addToHistory() " << slice.mzmin << " " << slice.mzmax << endl;
	history.push_back(slice);
	if (history.size() > 10 ) history.pop_front();	//keep history to some reasonable number
	_pos = history.size()-1;

}

mzSlice History::next() {
	if (size() == 0) return mzSlice(0,0,0,0);
	if (_pos + 1 < history.size() ) _pos++; 
	return history[_pos];
}

mzSlice History::last() {
	if (size() == 0) return mzSlice(0,0,0,0);
	if (_pos-1 > 0 )  _pos--; 
	return history[_pos];
}
