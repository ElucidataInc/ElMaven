#include "Peptide.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include <math.h>
#include <stdlib.h>

/*

Library       : Peptide
Author        : Henry Lam <hlam@systemsbiology.org>                                                       
Date          : 03.06.06 


Copyright (C) 2006 Henry Lam

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA

Henry Lam
Insitute for Systems Biology
1441 North 34th St. 
Seattle, WA  98103  USA
hlam@systemsbiology.org

*/

/* Class: Peptide
 * 
 * A container class for a peptide. Takes a peptide string in either SEQUEST .out format
 * (something like K.AC*DEFGHIK.L), interact format (something like K.AC[339]DEFGHIK.L) or
 * msp format (something like K.ACDEFGM(O)IK.L/3 with associated mod string), parses them
 * properly with error checking, and exports methods to do copying, comparison, conversion, 
 * mass calculation, etc.
 */

// static members initialization. NOTE that the static method Peptide::defaultTable() 
// will be called the first one a Peptide object is constructed to set up these tables. If
// the user wishes to have non-default tables, he can supply these tables before calling any methods. 
// (See defaultTable() for an explanation) 
map<char, double>* Peptide::AAAverageMassTable = NULL;
map<string, double>* Peptide::modAverageMassTable = NULL;
map<char, double>* Peptide::AAMonoisotopicMassTable = NULL;
map<string, double>* Peptide::modMonoisotopicMassTable = NULL;
map<string, string>* Peptide::modTokenTable = NULL;
map<char, double*>* Peptide::AAMonoisotopicNeutralLossTable = NULL;
map<string, double*>* Peptide::modMonoisotopicNeutralLossTable = NULL;
map<string, double*>* Peptide::AAMonoisotopicImmoniumTable = NULL;

// constants to refer to N-term mod and C-term mod in an msp-style modification string.
const int Peptide::NTermPos = -1;
const int Peptide::CTermPos = -2;

// CONSTRUCTORS, DESTRUCTOR AND ASSIGNMENT OPERATOR

// Constructor (SEQUEST .out or interact format)
Peptide::Peptide(string pep, int ch) {

  if (!AAAverageMassTable || !modAverageMassTable || !AAMonoisotopicMassTable || !modMonoisotopicMassTable || !modTokenTable ||
      !AAMonoisotopicNeutralLossTable || !modMonoisotopicNeutralLossTable || !AAMonoisotopicImmoniumTable) {
    defaultTables();
  }

  charge = ch;
  prevAA = 'X';
  nextAA = 'X';
  nTermMod = "";
  cTermMod = "";
  hasUnknownMod = false;
  illegalPeptideStr = false;
  illegalMspModStr = false;
	
  considerIsobaricAASame = false;
		
  isModsSet = false;
  if (!stripPeptide(pep)) {
    illegalPeptideStr = true;
    //    cerr << "Illegal peptide string: " << pep << " !" << endl;
  }	
}	
	
// Constructor (.msp format) - NOTE: If the charge and mod string are supplied, they will
// override what's in the peptide string. (No error is reported if there is an inconsistency)
// If you want the program to parse out the charge and/or the modifications based on the peptide string, 
// supply ch = 0 and/or mspModStr = "".
Peptide::Peptide(string pep, int ch, string mspModStr) {

  if (!AAAverageMassTable || !modAverageMassTable || !AAMonoisotopicMassTable || !modMonoisotopicMassTable || !modTokenTable ||
      !AAMonoisotopicNeutralLossTable || !modMonoisotopicNeutralLossTable || !AAMonoisotopicImmoniumTable) {
    defaultTables();
  }
	
  charge = ch;	
  prevAA = 'X';
  nextAA = 'X';
  nTermMod = "";
  cTermMod = "";
  hasUnknownMod = false;
  illegalPeptideStr = false;
  illegalMspModStr = false;

  considerIsobaricAASame = false;
	
  isModsSet = false;
  if (!parseMspModStr(mspModStr)) {
    illegalMspModStr = true;
    //    cerr << "Illegal msp-style mod string: " << mspModStr << " !" << endl;
  }

  if (!stripPeptide(pep)) {
    illegalPeptideStr = true;
    //    cerr << "Illegal peptide string: " << pep << " !" << endl;
  }
		
}

// Copy constructor
Peptide::Peptide(Peptide& p) {
  (*this) = p;
}

// Destructor
Peptide::~Peptide() {
	
}

bool Peptide::isGood() {
  return (!hasUnknownMod && !illegalPeptideStr && !illegalMspModStr);
}

// Assignment operator
Peptide& Peptide::operator=(Peptide& p) {

  this->charge = p.charge;
  this->isModsSet = p.isModsSet;
  this->stripped = p.stripped;
  
  this->mods.clear();
  for (map<int, string>::iterator c = p.mods.begin(); c != p.mods.end(); c++) {
    this->mods.insert(*c);
  }
  
  this->nTermMod = p.nTermMod;
  this->cTermMod = p.cTermMod;
  this->prevAA = p.prevAA;
  this->nextAA = p.nextAA;	
  this->considerIsobaricAASame = p.considerIsobaricAASame;
  this->hasUnknownMod = p.hasUnknownMod;
  this->illegalPeptideStr = p.illegalPeptideStr;
  this->illegalMspModStr = p.illegalMspModStr;
  
  return (*this);
}

// =======================================================================
// METHODS TO DETERMINE EQUALITY AND HOMOLOGY OF TWO PEPTIDE IONS

// Equality operator - two peptides are considered the same if their stripped peptides are the same, AND
// their charges are the same, AND their modifications are the same. NOTE that the previous and next AA are
// NOT considered!
bool Peptide::operator==(Peptide& p) {

  if (this->stripped != p.stripped) {
    if (considerIsobaricAASame) {
      // if considerIsobaricAASame is set, this function will consider K and Q to be the same,
      // and I and L to be the same. The degenerate symbols B (for N and D) and Z (for Q and E)
      // will be considered the same as their nondegenerate ones.
      
      // check the length of the peptides
      if (this->stripped.length() != p.stripped.length()) {
        return (false);
      }
      
      // they are the same length, now go through each AA to compare.
      string::size_type i, j;

      for (i = 0, j = 0; i < this->stripped.length() && j < p.stripped.length(); i++, j++) {	
        if (!((this->stripped[i] == p.stripped[j]) ||
              (this->stripped[i] == 'K' && p.stripped[j] == 'Q') || 
              (this->stripped[i] == 'Q' && p.stripped[j] == 'K') || 
              (this->stripped[i] == 'I' && p.stripped[j] == 'L') || 
              (this->stripped[i] == 'L' && p.stripped[j] == 'I') ||
              (this->stripped[i] == 'B' && p.stripped[j] == 'N') ||
              (this->stripped[i] == 'B' && p.stripped[j] == 'D') ||
              (this->stripped[i] == 'N' && p.stripped[j] == 'B') ||
              (this->stripped[i] == 'D' && p.stripped[j] == 'B') ||
              (this->stripped[i] == 'Z' && p.stripped[j] == 'Q') ||
              (this->stripped[i] == 'Z' && p.stripped[j] == 'E') ||
              (this->stripped[i] == 'Q' && p.stripped[j] == 'Z') ||
              (this->stripped[i] == 'E' && p.stripped[j] == 'Z'))) {
          return (false);
        }
      }       
      
    } else {
      return (false);
    }
  }	
  if (this->charge != 0 && p.charge != 0 && this->charge != p.charge) {
    return (false);
  }
  if (this->hasUnknownMod || p.hasUnknownMod) {
    return (false);
  }
  if (this->isModsSet && p.isModsSet && this->mods != p.mods) {
    return (false);
  }
  if (this->isModsSet && p.isModsSet && 
      ((this->nTermMod != p.nTermMod) || (this->cTermMod != p.cTermMod))) {
    return (false);
  }
  
  return (true);
	
}
	
// strippedEquals - similar to the equality operator, but only consider the stripped peptide. Two peptides
// will be considered the same even if they have different charges and/or modifications.
bool Peptide::strippedEquals(Peptide& p) {
	
  if (this->stripped != p.stripped) {
    if (considerIsobaricAASame) {
      if (this->stripped.length() != p.stripped.length()) {
        return (false);
      }

      string::size_type i, j;
      
      for (i = 0, j = 0; i < this->stripped.length() && j < p.stripped.length(); i++, j++) {	
        if (!((this->stripped[i] == p.stripped[j]) ||
              (this->stripped[i] == 'K' && p.stripped[j] == 'Q') || 
              (this->stripped[i] == 'Q' && p.stripped[j] == 'K') || 
              (this->stripped[i] == 'I' && p.stripped[j] == 'L') || 
              (this->stripped[i] == 'L' && p.stripped[j] == 'I') ||
              (this->stripped[i] == 'B' && p.stripped[j] == 'N') ||
              (this->stripped[i] == 'B' && p.stripped[j] == 'D') ||
              (this->stripped[i] == 'N' && p.stripped[j] == 'B') ||
              (this->stripped[i] == 'D' && p.stripped[j] == 'B') ||
              (this->stripped[i] == 'Z' && p.stripped[j] == 'Q') ||
              (this->stripped[i] == 'Z' && p.stripped[j] == 'E') ||
              (this->stripped[i] == 'Q' && p.stripped[j] == 'Z') ||
              (this->stripped[i] == 'E' && p.stripped[j] == 'Z'))) {
	  
          return (false);
        }
      }       
    } else {
      return (false);
    }
  }	
  return (true);
  
	
}
	
// isSubsequence -- see if other is a sub-sequence of *this
bool Peptide::isSubsequence(Peptide& other, bool ignoreMods) {
	
  if (ignoreMods) {
    string::size_type found = stripped.find(other.stripped, 0);
    return (found != string::npos);
  } else {
    
    // not quite 100% correct... e.g. GGGGC will be considered a subsequence of AGGGGC[160]P, which it isn't.
    string str = interactStyle();
    string::size_type found = str.find(other.interactStyle(), 0);
    return (found != string::npos);
  }
}

// isHomolog - see if the two peptide sequences are homologous.
// use a dynamic programming algorithm to align the two sequences
// At the end, T[m][n] will contain a measure of sequence identity
// Basically, an aligned AA will get 1 point, and an aligned AA following another aligned AA will get 2 points.
// Examples:
//    DEFGHI-
// vs DEFGHIK  (1+2+2+2+2+2 = 11 points)
//    DEFGGHI-
// vs DEFG-HIK (1+2+2+2+1+2 = 10 points)
//    DEFFG-HI-
// vs YEF-GGHIK (1+2+1+1+2 = 7 points)
bool Peptide::isHomolog(Peptide& other, double threshold, int& identity) {
	
  unsigned int m = (unsigned int)(stripped.length());
  unsigned int n = (unsigned int)(other.stripped.length());
  
  vector<string> aa1(m + 1);
  vector<string> aa2(n + 1);
  
  string::size_type shorter = m;
  if (m > n) shorter = n;
  
  // if the shorter sequence is a perfect sub-sequence of the longer one, its score is (shorter * 2 - 1).
  // so we are calculating our threshold identity score by multiplying the specified threshold by this "perfect" score
  double minIdentity = threshold * (shorter * 2 - 1);
  
  int** alignedLength = new int*[m + 1];
  int** identical = new int*[m + 1];
    
  unsigned int i = 0;
  unsigned int j = 0;

  for (i = 0; i <= m; i++) {
    alignedLength[i] = new int[n + 1];
    identical[i] = new int[n + 1];
    
    alignedLength[i][0] = 0;
    if (i > 0) aa1[i] = interactStyleAA(i - 1);
  
    for (j = 0; j <= n; j++) {
      identical[i][j] = 0;
    }
  }  
  
  for (j = 0; j <= n; j++) {
    alignedLength[0][j] = 0;
    if (j > 0) aa2[j] = other.interactStyleAA(j - 1);
  }
  
  for (i = 1; i <= m; i++) {
    for (j = 1; j <= n; j++) {
      if (aa1[i] == aa2[j] ||
          (aa1[i] == "L" && aa2[j] == "I") || // we consider I and L the same, K and Q the same
          (aa1[i] == "I" && aa2[j] == "L") ||
          (aa1[i] == "K" && aa2[j] == "Q") ||
          (aa1[i] == "Q" && aa2[j] == "K")) {
        alignedLength[i][j] = alignedLength[i - 1][j - 1] + 1 + identical[i - 1][j - 1];
        identical[i][j] = 1;
      } else {
        if (alignedLength[i - 1][j] > alignedLength[i][j - 1]) {
          alignedLength[i][j] = alignedLength[i - 1][j];
        } else {
          alignedLength[i][j] = alignedLength[i][j - 1];
        }
       
      }
    }
  }
 
  identity = alignedLength[m][n];
  bool result = ((double)(alignedLength[m][n]) >= minIdentity);
 

  for (unsigned int r = 0; r <= m; r++) {
    delete[] alignedLength[r];
    delete[] identical[r];
  }
  delete[] alignedLength;
  delete[] identical;
  
  return (result);
  
}

	

// =======================================================================
// METHODS TO SET THE MODIFICATION HASH

// setModByToken - given a token (e.g., M[147], C@), set the modification by referring to
// the mod token table. the argument "terminus" is optional; if not given, 
// it's a normal, non-terminal mod.
bool Peptide::setModByToken(string token, unsigned int pos, char terminus) {

  // try to find the token in the token table
  map<string, string>::iterator found = (*modTokenTable).find(token);
  if (found == (*modTokenTable).end()) {
    if (token.length() > 1) {
      // it's not in the token table, and it has more than one char (can't be an
      // ordinary AA!)
      
      // check for Ms2-style, e.g. M[+16.0] 
      if (token.length() > 3 && token[1] == '[' && (token[2] == '+' || token[2] == '-') 
	  && token[token.length() - 1] == ']' 
	  && Peptide::AAMonoisotopicMassTable->find(token[0]) != Peptide::AAMonoisotopicMassTable->end()) {
        
	// try to construct SpectraST-style token by adding the delta to the amino acid mass
	double modMass = (*Peptide::AAMonoisotopicMassTable)[token[0]];
	string modMassStr("");
	modMassStr = token.substr(2, token.length() - 3);
	modMass += atof(modMassStr.c_str());      
        stringstream newTokenss;
        newTokenss << token[0] << '[';
        newTokenss << (int)(modMass + 0.5) << ']';
        string newToken = newTokenss.str();
      
	// now try to find this SpectraST-style token in the table
	found = (*modTokenTable).find(newToken);
	if (found != modTokenTable->end()) {
	  string modType = found->second;
	  return (setModByType(modType, pos, terminus));
	}
      
      
      }
      
      hasUnknownMod = true;
      return (false);
    }
  } else {
    // found the modType that corresponds to the mod token
    string modType = (*found).second;
    return (setModByType(modType, pos, terminus));
    
  }
  
  return (true);
  
  
}

// setModByType - given a mod type (e.g., Carbamidomethyl), set the modification.
// the argument "terminus" is optional; if not given, 
// it's a normal, non-terminal mod.
bool Peptide::setModByType(string modType, unsigned int pos, char terminus) {
  
  // find the mod type in the mod mass table.
   // map<string, double>::iterator foundA = (*modAverageMassTable).find(modType);
  map<string, double>::iterator foundM = (*modMonoisotopicMassTable).find(modType);
  
  if (/* foundA == (*modAverageMassTable).end() || */ foundM == (*modMonoisotopicMassTable).end()) {
    // not found!
    // cerr << "No such modification type: " << modType << endl;
    hasUnknownMod = true;
    return (false);
  } else {

    if (terminus == 'n') {
//      if (isNTermModType(modType)) {
        nTermMod = modType;
//     } else {
        // cerr << "No such N-terminal modification type: " << modType << endl;
//	hasUnknownMod = true;
//        return (false);
//      }
    } else if (terminus == 'c') {
//      if (isCTermModType(modType)) {
        cTermMod = modType;
//      } else {
        // cerr << "No such C-terminal modification type: " << modType << endl;
//	hasUnknownMod = true;
//        return (false);
//      }
          
    } else {
      
      mods[(int)pos] = modType;
    }
  }	
  return (true);
	
}

// =======================================================================
// METHODS TO PRINT OUT STRING REPRESENTATIONS OF THE PEPTIDE ION
	
// strippedWithCharge - returns the peptide in the format <stripped peptide>/<charge>
string Peptide::strippedWithCharge() {
	
  stringstream ss;
  ss << stripped << '/' << charge;
  return (ss.str());
	
}
	
// fullWithCharge - returns the peptide in the format <prevAA>.<stripped peptide>.<nextAA>/<charge>
string Peptide::fullWithCharge() {
	
  stringstream ss;
  ss << prevAA << '.' << stripped << '.' << nextAA << '/' << charge;
  return (ss.str());
	
}

// full - returns the peptide in the format <prevAA>.<stripped peptide>.<nextAA>
string Peptide::full() {

  stringstream ss;
  ss << prevAA << '.' << stripped << '.' << nextAA;
  return (ss.str());	
	
}

// mspMods - returns the modifications in .msp mod string format -- except that
// n-term mods will get a position of -1, and c-term mods will get a position of -2
string Peptide::mspMods() {

  if (!isModsSet) {
    return ("");
  }
  
  stringstream ss;
  ss << mods.size() + (nTermMod.empty() ? 0 : 1) + (cTermMod.empty() ? 0 : 1);
  
  if (!nTermMod.empty()) {	
    ss << '/' << Peptide::NTermPos /* + 1 */ << ',' << stripped[0] /* "n-term" */ << ',' << nTermMod; // Peptide::NTermPos = -1
  } 
  for (map<int, string>::iterator i = mods.begin(); i != mods.end(); i++) {
    ss << '/' << (*i).first /* + 1 */ << ',' << stripped[(*i).first] << ',' << (*i).second;
  }
  if (!cTermMod.empty()) {
    ss << '/' << Peptide::CTermPos << ',' << stripped[stripped.length() - 1] << ',' << cTermMod; // Peptide::CTermPos = -2
  }
  
  return (ss.str());
}

// interactStyle - returns the peptide in Interact format, i.e. modification masses as [ ] next to AA.
string Peptide::interactStyle() {

  if (!isModsSet || (mods.empty() && nTermMod.empty() && cTermMod.empty())) {
    return stripped;
  }

  stringstream ss;

  ss << interactStyleAA(0, 'n');

  for (unsigned int i = 0; i < (unsigned int)(stripped.length()); i++) {
    ss << interactStyleAA(i);
  }

  ss << interactStyleAA(0, 'c');
  
  return (ss.str());
}

// interactStyle - returns the peptide in Interact format, appended with /<charge>
string Peptide::interactStyleWithCharge() {
	
  stringstream ss;
  ss << interactStyle() << '/' << charge;
  return (ss.str());
	
}

// interactStyle - returns the peptide in the format <prevAA>.<interact-style peptide>.<nextAA>/<charge>
string Peptide::interactStyleFullWithCharge() {
	
  stringstream ss;
  ss << prevAA << '.' << interactStyle() << '.' << nextAA << '/' << charge;
  return (ss.str());
}

// htmlStyle - similar to interactStyle, but instead of square brackets, the mass
// is enclosed in <SUB> </SUB> html tags.
string Peptide::htmlStyle() {

  string is = interactStyle();
  stringstream ss;

  for (unsigned int i = 0; i < (unsigned int)(is.length()); i++) {
    if (is[i] == '[') {
      ss << "<SUB>";
    } else if (is[i] == ']') {
      ss << "</SUB>";
    } else {
      ss << is[i];
    }
  }

  return (ss.str());

}

// interactStyleAA - just displays a AA token (with a modToken if any).
// the argument "terminus" is optional. If not given, it's a normal non-terminal mod.
string Peptide::interactStyleAA(unsigned int pos, char terminus) {

  if (terminus == 'n' && isModsSet && !nTermMod.empty()) {
    return (getModToken('n', nTermMod));
  } 
  
  if (terminus == 'c' && isModsSet && !cTermMod.empty()) {
    return (getModToken('c', cTermMod));
  }

  if ( pos > (unsigned int)(stripped.length()) - 1 || terminus == 'n' || terminus == 'c') {
    return ("");
  }

  if (isModsSet) { 
    map<int, string>::iterator found = mods.find((int)pos);
    if (found != mods.end()) {
      return (getModToken(stripped[pos], found->second));
    }
  }
  
  string aa("");
  aa += stripped[pos];
  return (aa);
  
}

// getModToken - creates a mod token based on the amino acid and the modName. 
// Will first try if X[modName] is a known token. If not, will create one
// in the form X[average mass of aa + mod, rounded to nearest integer].
string Peptide::getModToken(char aa, string modName) {

  string token("");
  token += aa;
  token += "[" + modName + "]";

  if (modTokenTable->find(token) != modTokenTable->end()) {
    return (token);
  } else {
    stringstream ss;
    ss << aa << '[' << (int)(getAAPlusModMonoisotopicMass(aa, modName) + 0.5) << ']';
    return (ss.str());
  }

}

// getSafeName - replaces the symbols in the peptide string with underscores, so that
// the string can be used as a filename 
string Peptide::getSafeName() {

  stringstream ss;
  
  if (!isModsSet || (mods.empty() && nTermMod.empty() && cTermMod.empty())) {
    ss << stripped << '_' << charge;
    return (ss.str());
  }
  
  if (!nTermMod.empty()) {
    ss << 'n' << (int)(getAAPlusModMonoisotopicMass('n', nTermMod) + 0.5);
  }
  for (string::size_type i = 0; i < stripped.length(); i++) {
    ss << stripped[i];
    
    map<int, string>::iterator found = mods.find((int)i);		
    if (found != mods.end()) {
      // modified
      ss << (int)(getAAPlusModMonoisotopicMass(stripped[i], (*found).second) + 0.5);			
    }			
  }
  if (!cTermMod.empty()) {
    ss << 'c' << (int)(getAAPlusModMonoisotopicMass('c', cTermMod) + 0.5);
  }
  
  ss << '_' << charge;
  
  return (ss.str());
}	


// =======================================================================
// METHODS TO CALCULATE MASSES AND MASS-TO-CHARGE RATIOS
	
// averageNeutralM - calculates the average mass of the neutral peptide. 
double Peptide::averageNeutralM() {
	
  double sum = 0;
  for (string::size_type i = 0; i < stripped.length(); i++) {
    sum += (*AAAverageMassTable)[stripped[i]];
  }
  sum += (*AAAverageMassTable)['n']; // the H on the N-terminus
  sum += (*AAAverageMassTable)['c']; // the OH on the C-terminus
  
  if (isModsSet) {
    if (!nTermMod.empty()) {
      sum += getModAverageMass(nTermMod);	
    }
    if(!cTermMod.empty()) {
      sum += getModAverageMass(cTermMod);
    }
    for (map<int, string>::iterator j = mods.begin(); j != mods.end(); j++) {
      sum += getModAverageMass((*j).second); // add the modifications' masses
    }
  }
  return (sum);
}

// monoisotopicNeutralM - calculates the monoisotopic mass of the neutral peptide. 
double Peptide::monoisotopicNeutralM() {
	
  double sum = 0;
  for (string::size_type i = 0; i < stripped.length(); i++) {
    sum += (*AAMonoisotopicMassTable)[stripped[i]];
  }
  sum += (*AAMonoisotopicMassTable)['n']; // the H on the N-terminus
  sum += (*AAMonoisotopicMassTable)['c']; // the OH on the C-terminus
  
  if (isModsSet) {
    if (!nTermMod.empty()) {
      sum += getModMonoisotopicMass(nTermMod);	
    }
    if(!cTermMod.empty()) {
      sum += getModMonoisotopicMass(cTermMod);
    }
    for (map<int, string>::iterator j = mods.begin(); j != mods.end(); j++) {
      sum += getModMonoisotopicMass((*j).second); // add the modifications' masses
    }
  }
  return (sum);
}


// averageMH - calculates the average mass of the peptide ion. if the charge is not known
// (that is, set to zero), it will give the average mass of the neutral peptide.
// (Note: This is not the M+H used in SEQUEST, which is always the mass of the singly charged ion.)
double Peptide::averageMH() {
  
  return (averageNeutralM() + (charge * (*AAAverageMassTable)['+']));
}

// monoisotopicMH - calculates the monoisotopic mass of the peptide ion. if the charge is not known
// (that is, set to zero), it will give the monoisotopic mass of the neutral peptide.
// (Note: This is not the M+H used in SEQUEST, which is always the mass of the singly charged ion.)
double Peptide::monoisotopicMH() {

  return (monoisotopicNeutralM() + (charge * (*AAMonoisotopicMassTable)['+']));
}


// averageMZ - calculates the m/z value of the peptide ion. If charge is not specified, +1 is assumed.
double Peptide::averageMZ() {
  if (charge != 0) {
    return (averageMH() / (double)charge);	
  } else {
    cerr << "Attempting to calculate M/Z with charge = 0! Assume charge = 1." << endl;
    return (averageMH());
  }
}

// monoisotopicMZ - calculates the m/z value of the peptide ion. If charge is not specified, +1 is assumed.
double Peptide::monoisotopicMZ() {
  if (charge != 0) {
    return (monoisotopicMH() / (double)charge);	
  } else {
    cerr << "Attempting to calculate M/Z with charge = 0! Assume charge = 1." << endl;
    return (monoisotopicMH());
  }
}

// averageMZFragment - calculates the m/z value of a fragment. type is either y, b or a.
// for instance, to calculate the m/z of the y7++ fragment, type = 'y', numAA = 7, charge = 2.
double Peptide::averageMZFragment(char type, unsigned int numAA, unsigned int charge) {

  if (type != 'y' && type != 'b' && type != 'a' && type != 'z' && type != 'c') return (0.0);
  // will support x, c and z type ions later
  
  unsigned int len = NAA();
 
  if (numAA < 1 || numAA >= len) return (0.0);
  
  if (charge < 1) return (0.0);
  
  if (type == 'y' || type == 'z') {
    double sum = 0.0;
    // y ions are on the C-term side. Have to include the C-term mod, if any.
    if (isModsSet && !cTermMod.empty()) {
      sum += getModAverageMass(cTermMod);
    }
    for (unsigned int i = len - numAA; i < len; i++) {
      sum += (*AAAverageMassTable)[stripped[i]];
      
      if (isModsSet && !mods.empty()) {
        map<int, string>::iterator j = mods.find((int)i);
        if (j != mods.end()) {
          // modified at this position
          sum += getModAverageMass((*j).second);
        }
      }
    }
    // add a water for the y ion
    sum += (*AAAverageMassTable)['!'];
    // add a proton for each charge
    sum += ((double)charge * ((*AAAverageMassTable)['+']));
    
    // if it's a z ion, subtract an ammonia from the y ion
    if (type == 'z') {
      sum -= (*AAAverageMassTable)['a'];
    }

    return (sum / (double)charge);
    
  } 
  if (type == 'b' || type == 'a' || type == 'c') {
    double sum = 0.0;
    
    // b or a ions are on the N-term side. Add the N-term modification mass, if any
    if (isModsSet && !nTermMod.empty()) {
      sum += getModAverageMass(nTermMod);
    }
    
    for (unsigned int i = 0; i < numAA; i++) {
      sum += (*AAAverageMassTable)[stripped[i]];
      if (isModsSet && !mods.empty()) {
        map<int, string>::iterator j = mods.find((int)i);
        if (j != mods.end()) {
          // modified at this position
          sum += getModAverageMass((*j).second);
        }
      }
    }
    
    // add a proton for each charge
    sum += ((double)charge * ((*AAAverageMassTable)['+']));
    
    // if it's the a ion, subtract the carbonyl (C=O) from the b ion.
    if (type == 'a') {
      sum -= ((*AAAverageMassTable)['$'] + (*AAAverageMassTable)['o']);
    }

    // if it's the c ion, add an ammonia to the b ion
    if (type == 'c') {
      sum += (*AAAverageMassTable)['a'];
    }
    
    return (sum / (double)charge);
    
  }
  
  return (0.0);
}

// monoisotopicMZFragment - calculates the m/z value of a fragment. type is either y, b or a.
// for instance, to calculate the m/z of the y7++ fragment, type = 'y', numAA = 7, charge = 2.
double Peptide::monoisotopicMZFragment(char type, unsigned int numAA, unsigned int charge) {

  if (type != 'y' && type != 'b' && type != 'a' && type != 'z' && type != 'c') return (0.0);
  
  unsigned int len = NAA();
  
  if (numAA < 1 || numAA >= len) return (0.0);
  
  if (charge < 1) return (0.0);
  
  if (type == 'y' || type == 'z') {
    double sum = 0.0;
    // y ions are on the C-term side. Have to include the C-term mod, if any.
    if (isModsSet && !cTermMod.empty()) {
      sum += getModMonoisotopicMass(cTermMod);
    }
    for (unsigned int i = len - numAA; i < len; i++) {
      sum += (*AAMonoisotopicMassTable)[stripped[i]];
      if (isModsSet && !mods.empty()) {
        map<int, string>::iterator j = mods.find((int)i);
        if (j != mods.end()) {
          // modified at this position
          sum += getModMonoisotopicMass((*j).second);
        }
      }
    }
    // add a water for the y ion
    sum += (*AAMonoisotopicMassTable)['!'];
    // add a proton for each charge
    sum += ((double)charge * ((*AAMonoisotopicMassTable)['+']));
    
    // if it's a z ion, subtract an ammonia from the y ion
    if (type == 'z') {
      sum -= ((*AAMonoisotopicMassTable)['a'] - (*AAMonoisotopicMassTable)['+']);
    }
    
    return (sum / (double)charge);
    
  } 
  if (type == 'b' || type == 'a' || type == 'c') {
    double sum = 0.0;
    
    // b or a ions are on the N-term side. Add the N-term modification mass, if any
    if (isModsSet && !nTermMod.empty()) {
      sum += getModMonoisotopicMass(nTermMod);
    }
    
    for (unsigned int i = 0; i < numAA; i++) {
      sum += (*AAMonoisotopicMassTable)[stripped[i]];
      if (isModsSet && !mods.empty()) {
        map<int, string>::iterator j = mods.find((int)i);
        if (j != mods.end()) {
          // modified at this position
          sum += getModMonoisotopicMass((*j).second);
        }
      }
    }
    
    // add a proton for each charge
    sum += ((double)charge * ((*AAMonoisotopicMassTable)['+']));
  
    // if it's the a ion, subtract the carbonyl (C=O) from the b ion.
    if (type == 'a') {
      sum -= ((*AAMonoisotopicMassTable)['$'] + (*AAMonoisotopicMassTable)['o']);
    }
    
    // if it's the c ion, add an ammonia to the b ion
    if (type == 'c') {
      sum += (*AAMonoisotopicMassTable)['a'];
    }

    return (sum / (double)charge);
    
  }

  return (0.0);
}


// =======================================================================
// METHODS TO EVALUATE SOME BASIC PROPERTIES OF THE PEPTIDE ION
	
// NTT - counts the number of tryptic termini. NOTE: if prevAA or nextAA is not known, zero is returned,
// since there is no way to figure out the trypticness of the termini without that information. 
unsigned int Peptide::NTT() {

//  if (prevAA == 'X' || nextAA == 'X') {
//    return (0);
//  }
  
  unsigned int NTT = 0;	
  if (prevAA == '-' || prevAA == '[' || ((prevAA == 'K' || prevAA == 'R') && stripped[0] != 'P')) {
    NTT++;
  }
  if (nextAA == '-' || nextAA == ']' || ((stripped[stripped.length() - 1] == 'K' || stripped[stripped.length() - 1] == 'R') && nextAA != 'P')) {
    NTT++;
  }
  
  return (NTT);
  
	
}

// NMC - counts the number of missed tryptic cleavage
unsigned int Peptide::NMC() {

  unsigned int NMC = 0;
  for (string::size_type i = 0; i < stripped.length() - 1; i++) {
    if ((stripped[i] == 'K' || stripped[i] == 'R') && (stripped[i+1] != 'P')) {
      NMC++;
    }
  }
  return (NMC);
	
}

// isCleavableICAT - determines whether it is a cleavable ICAT peptide. 
bool Peptide::isCleavableICAT() {

  if (isModsSet) {
    for (map<int, string>::iterator m = mods.begin(); m != mods.end(); m++) {
      if (m->second == "ICAT-C" || m->second == "ICAT-C:13C(9)" ||
          m->second == "ICAT_light" || m->second == "ICAT_heavy") {
        return (true);
      }
    }
    return (false);
  } else {
    return (false);  
  }
}

// isUncleavableICAT - determines whether it is a uncleavable ICAT peptide. 
bool Peptide::isUncleavableICAT() {

  if (isModsSet) {
    for (map<int, string>::iterator m = mods.begin(); m != mods.end(); m++) {
      if (m->second == "ICAT-D" || m->second == "ICAT-D:2H(8)" ||
          m->second == "AB_old_ICATd0" || m->second == "AB_old_ICATd8") {

        return (true);
      }
    }
    return (false);
  } else {
    return (false);
  }  
	
}

// isCAMCysteine - determines whether the peptide contains a CAM-modified cysteine. 
bool Peptide::isCAMCysteine() {

  if (isModsSet) {
    for (map<int, string>::iterator m = mods.begin(); m != mods.end(); m++) {
      if (m->second == "Carbamidomethyl") {
        return (true);
      }
    }
    return (false);
  }
  
  return (false);	
}

// hasUnmodifiedCysteine - determines if the peptide contains an unmodified cysteine
bool Peptide::hasUnmodifiedCysteine() {
  
  if (!isModsSet) {
    return (stripped.find('C', 0) != string::npos);
  }
  
  string::size_type pos = 0;
  while (pos < stripped.length() && ((pos = stripped.find('C', pos)) != string::npos)) {
    if (mods.find((int)pos) == mods.end()) {
      return (true);
    }
    pos++;
  }
  return (false);
}

// NAA - counts the number of amino acids
unsigned int Peptide::NAA() {
  return ((unsigned int)(stripped.length()));	
}

// getAllPresentModTypes - lists all distinct (aa,modType) present on this peptide (and the number of times each occurs)
bool Peptide::getAllPresentModTypes(map<string, unsigned int>& presentModTypes) {

  if (!isModsSet || (mods.empty() && nTermMod.empty() && cTermMod.empty())) {
    return (false);
  }

  presentModTypes.clear();

  if (!nTermMod.empty()) {
    string tag("n,");
    tag += nTermMod;
    presentModTypes[tag] = 1;
  }

  if (!cTermMod.empty()) {
    string tag("c,");
    tag += cTermMod;
    presentModTypes[tag] = 1;
  }

  for (map<int, string>::iterator i = mods.begin(); i != mods.end(); i++) {
    stringstream tag;
    tag << stripped[i->first] << ',' << i->second;
    if (presentModTypes.find(tag.str()) != presentModTypes.end()) {
      presentModTypes[tag.str()]++;
    } else {
      presentModTypes[tag.str()] = 1;
    }
  }

  return (true);

}

// countAATokens - counts the number of occurrences of all AA tokens (e.g. C[160]) in the peptide
void Peptide::countAATokens(map<string, unsigned int>& tokenCounts) {
  
  string token("");
  
  for (unsigned int j = 0; j < NAA(); j++) {
    token = interactStyleAA(j);
    if (tokenCounts.find(token) == tokenCounts.end()) {
      tokenCounts[token] = 1;
    } else {
      tokenCounts[token]++;
    }
  
  }
  
  token = interactStyleAA(0, 'n');
  if (!(token.empty())) tokenCounts[token] = 1;
  
  token = interactStyleAA(0, 'c');
  if (!(token.empty())) tokenCounts[token] = 1;
  
}


// ====================================================================
// METHODS TO MANAGE AND ACCESS THE MASS TABLES
	
// defaultTable - set the default tables (this is an example of how the tables should look like
void Peptide::defaultTables() {
	
  // average mass
  
  Peptide::AAAverageMassTable = new map<char, double>();
  
  (*AAAverageMassTable)['A'] = 71.0788;
  (*AAAverageMassTable)['B'] = 114.5952; // average of Asp and Asn
  (*AAAverageMassTable)['C'] = 103.1388;
  (*AAAverageMassTable)['D'] = 115.0886;
  (*AAAverageMassTable)['E'] = 129.1155;
  (*AAAverageMassTable)['F'] = 147.1766;
  (*AAAverageMassTable)['G'] = 57.0519;
  (*AAAverageMassTable)['H'] = 137.1411;
  (*AAAverageMassTable)['I'] = 113.1594;
  (*AAAverageMassTable)['J'] = 0.0000; // fake AA for creative modifcations
  (*AAAverageMassTable)['K'] = 128.1741;
  (*AAAverageMassTable)['L'] = 113.1594;
  (*AAAverageMassTable)['M'] = 131.1926;
  (*AAAverageMassTable)['N'] = 114.1038;
  (*AAAverageMassTable)['O'] = 237.3018; // pyrrolysine
  (*AAAverageMassTable)['P'] = 97.1167;
  (*AAAverageMassTable)['Q'] = 128.1307;
  (*AAAverageMassTable)['R'] = 156.1875;
  (*AAAverageMassTable)['S'] = 87.0782;
  (*AAAverageMassTable)['T'] = 101.1051;
  (*AAAverageMassTable)['U'] = 150.0388; // selenocysteine
  (*AAAverageMassTable)['V'] = 99.1326;
  (*AAAverageMassTable)['W'] = 186.2132;
  (*AAAverageMassTable)['X'] = 113.1594; // set to Leu/Ile for no particular reason
  (*AAAverageMassTable)['Y'] = 163.1760;
  (*AAAverageMassTable)['Z'] = 128.6231; // average of Glu and Gln
  (*AAAverageMassTable)['!'] = 18.01524; // water - ! looks like a water droplet
  (*AAAverageMassTable)['+'] = 1.00739;  // proton
  (*AAAverageMassTable)['a'] = 17.0305; // ammonia - NH3
  (*AAAverageMassTable)['h'] = 1.00794;  // hydrogen
  (*AAAverageMassTable)['o'] = 15.9994;  // oxygen
  (*AAAverageMassTable)['$'] = 12.0110;  // carbon. think carbon = diamond = $
  (*AAAverageMassTable)['n'] = (*AAAverageMassTable)['h']; // N-terminus. Just the hydrogen
  (*AAAverageMassTable)['c'] = (*AAAverageMassTable)['h'] + (*AAAverageMassTable)['o']; // C-terminus. The hydroxyl group
  
  
  Peptide::modAverageMassTable = new map<string, double>();
  (*modAverageMassTable)["ICAT_light"] = 227.2603; 
  (*modAverageMassTable)["ICAT-C"] = 227.2603; // PSI new name

  (*modAverageMassTable)["ICAT_heavy"] = 236.1942;
  (*modAverageMassTable)["ICAT-C:13C(9)"] = 236.1942; // PSI new name

  (*modAverageMassTable)["AB_old_ICATd0"] = 442.5728;
  (*modAverageMassTable)["ICAT-D"] = 442.5728; // PSI new name

  (*modAverageMassTable)["AB_old_ICATd8"] = 450.6221;
  (*modAverageMassTable)["ICAT-D:2H(8)"] = 450.6221; // PSI new name

  (*modAverageMassTable)["Carbamidomethyl"] = 57.0513;
  
  (*modAverageMassTable)["Carboxymethyl"] = 58.0361;
  
  (*modAverageMassTable)["Propionamide"] = 71.0779; // alkylation of acrylamide to cysteines
  (*modAverageMassTable)["Propionamide:2H(3)"] = 74.0964; // alkylation of heavy acrylamide to cysteines
  (*modAverageMassTable)["Propionamide:13C(3)"] = 74.0558;
  
  (*modAverageMassTable)["Oxidation"] = (*AAAverageMassTable)['o'];
  
  (*modAverageMassTable)["Acetyl"] = 42.0106; // acetylation of N terminus
  
  (*modAverageMassTable)["Deamidation"] = 0.9848;
  (*modAverageMassTable)["Deamidated"] = 0.9848; // PSI new name

  (*modAverageMassTable)["Pyro-cmC"] = 40.0208; // cyclicization of N-terminal CAM-cysteine (FIXED value 01/27/07)
  (*modAverageMassTable)["Pyro-carbamidomethyl"] = 40.0208; // PSI new name

  (*modAverageMassTable)["Pyro-glu"] = -17.0305; // loss of NH3 from glutamine
  (*modAverageMassTable)["Gln->pyro-Glu"] = -17.0305; // PSI new name

  (*modAverageMassTable)["Pyro_glu"] = -(*AAAverageMassTable)['!']; // loss of H2O from glutamic acid
  (*modAverageMassTable)["Glu->pyro-Glu"] = -(*AAAverageMassTable)['!']; // PSI new name

  (*modAverageMassTable)["Amide"] = -0.9848; // amidation of C terminus
  (*modAverageMassTable)["Amidated"] = -0.9848; // PSI new name
  
  (*modAverageMassTable)["Phospho"] = 79.9799; // phosphorylation
  
  (*modAverageMassTable)["Sulfo"] = 80.0632; // O-sulfonation
  
  (*modAverageMassTable)["Methyl"] = 14.0266; // methylation
  
  (*modAverageMassTable)["Carbamyl"] = 43.0247; // carbamylation of N terminus or lysines
  
  (*modAverageMassTable)["iTRAQ4plex"] =  144.1544; // iTRAQ on N terminus or K
  
  (*modAverageMassTable)["PEO-Iodoacetyl-LC-Biotin"] =  414.52; // Hui Zhang's PEO alkylation agent on cysteines

  (*modAverageMassTable)["Label:2H(3)"] = 3.0185; // SILAC heavy leucine (+3)
  (*modAverageMassTable)["Label:13C(6)"] = 5.9559; // SILAC heavy lysine or arginine (+6)
  (*modAverageMassTable)["Label:13C(6)15N(1)"] = 6.9490;
  (*modAverageMassTable)["Label:13C(6)15N(2)"] = 7.9427; // SILAC heavy lysine (+8)
  (*modAverageMassTable)["Label:13C(6)15N(3)"] = 8.9358;  
  (*modAverageMassTable)["Label:13C(6)15N(4)"] = 9.9296; // SILAC heavy arginine (+10)
  
  (*modAverageMassTable)["Methylthio"] = 46.0916; // methylthiolated cysteine (cys blocking by MMTS)

  (*modAverageMassTable)["Leucyl"] = 113.1594; // leucine added to N-term or K
  (*modAverageMassTable)["Leucyl:13C(6)15N(1)"] = 120.1087; // heavy leucine added to N-term or K

 
  (*modAverageMassTable)["Nitro"] = 44.9976;
  (*modAverageMassTable)["Dimethyl"] = 28.0532;
  (*modAverageMassTable)["Trimethyl"] = 42.0797;

  (*modAverageMassTable)["Bromo"] = 78.8961;

  
  // Ubl chains
  (*modAverageMassTable)["SUMO_1"] = 2137.2567; // SUMO-1 Tryptic/LysC tail
  (*modAverageMassTable)["SUMO_2_3_Tryp"] = 3551.7075; // SUMO-2/3 Tryptic tail
  (*modAverageMassTable)["Smt3_R93A_Tryp"] = 3815.0935; // Smt3_R93A Tryptic tail
  (*modAverageMassTable)["Smt3_R93A_LysC"] = 4546.9153; // Smt3_R93A LysC tail
  (*modAverageMassTable)["NEDD8_LysC"] = 1556.9154; // NEDD8 LysC tail
  (*modAverageMassTable)["Rub1_LysC"] = 2455.9015; // Rub1 LysC tail
  (*modAverageMassTable)["Ub_LysC"] = 1432.6894; // Ubiquitin LysC tail
  (*modAverageMassTable)["GlyGly"] = 114.1026; // Ubiquitin/NEDD8 Tryptic tail (2 glycines)

	
  // monoisotopic mass
  Peptide::AAMonoisotopicMassTable = new map<char, double>();
  
  (*AAMonoisotopicMassTable)['A'] = 71.03711;
  (*AAMonoisotopicMassTable)['B'] = 114.53494; // average of Asp and Asn
  (*AAMonoisotopicMassTable)['C'] = 103.00919;
  (*AAMonoisotopicMassTable)['D'] = 115.02694;
  (*AAMonoisotopicMassTable)['E'] = 129.04259;
  (*AAMonoisotopicMassTable)['F'] = 147.06841;
  (*AAMonoisotopicMassTable)['G'] = 57.02146;
  (*AAMonoisotopicMassTable)['H'] = 137.05891;
  (*AAMonoisotopicMassTable)['I'] = 113.08406;
  (*AAMonoisotopicMassTable)['J'] = 0.0000; // fake AA for creative modifcations
  (*AAMonoisotopicMassTable)['K'] = 128.09496;
  (*AAMonoisotopicMassTable)['L'] = 113.08406;
  (*AAMonoisotopicMassTable)['M'] = 131.04049;
  (*AAMonoisotopicMassTable)['N'] = 114.04293;
  (*AAMonoisotopicMassTable)['O'] = 237.14773; // pyrrolysine
  (*AAMonoisotopicMassTable)['P'] = 97.05276;
  (*AAMonoisotopicMassTable)['Q'] = 128.05858;
  (*AAMonoisotopicMassTable)['R'] = 156.10111;
  (*AAMonoisotopicMassTable)['S'] = 87.03203;
  (*AAMonoisotopicMassTable)['T'] = 101.04768;
  (*AAMonoisotopicMassTable)['U'] = 150.95364; // selenocysteine
  (*AAMonoisotopicMassTable)['V'] = 99.06841;
  (*AAMonoisotopicMassTable)['W'] = 186.07931;
  (*AAMonoisotopicMassTable)['X'] = 113.08406; // set to Leu/Ile for no particular reason
  (*AAMonoisotopicMassTable)['Y'] = 163.06333;
  (*AAMonoisotopicMassTable)['Z'] = 128.55059; // average of Glu and Gln
  (*AAMonoisotopicMassTable)['!'] = 18.01056; // water
  (*AAMonoisotopicMassTable)['+'] = 1.00728;  // proton
  (*AAMonoisotopicMassTable)['a'] = 17.026549; // ammonia - NH3
  (*AAMonoisotopicMassTable)['h'] = 1.00783;  // hydrogen
  (*AAMonoisotopicMassTable)['o'] = 15.99491;  // oxygen
  (*AAMonoisotopicMassTable)['$'] = 12.00000;  // carbon. think carbon = diamond = $
  (*AAMonoisotopicMassTable)['n'] = (*AAMonoisotopicMassTable)['h']; // N-terminus. Just the hydrogen
  (*AAMonoisotopicMassTable)['c'] = (*AAMonoisotopicMassTable)['h'] + (*AAMonoisotopicMassTable)['o']; // C-terminus. The hydroxyl group
  
  Peptide::modMonoisotopicMassTable = new map<string, double>();
  (*modMonoisotopicMassTable)["ICAT_light"] =  227.126991; 
  (*modMonoisotopicMassTable)["ICAT-C"] =  227.126991; // PSI new name

  (*modMonoisotopicMassTable)["ICAT_heavy"] =  236.157185;
  (*modMonoisotopicMassTable)["ICAT-C:13C(9)"] =  236.157185; // PSI new name

  (*modMonoisotopicMassTable)["AB_old_ICATd0"] =  442.224991;
  (*modMonoisotopicMassTable)["ICAT-D"] =  442.224991; // PSI new name

  (*modMonoisotopicMassTable)["AB_old_ICATd8"] =  450.275205;
  (*modMonoisotopicMassTable)["ICAT-D:2H(8)"] =  450.275205; // PSI new name
  
  (*modMonoisotopicMassTable)["Carbamidomethyl"] = 57.021464;
  
  (*modMonoisotopicMassTable)["Carboxymethyl"] = 58.005479;

  (*modMonoisotopicMassTable)["Propionamide"] = 71.037114; // alkylation of acrylamide to cysteines
  (*modMonoisotopicMassTable)["Propionamide:2H(3)"] = 74.055944; // alkylation of heavy acrylamide to cysteines
  (*modMonoisotopicMassTable)["Propionamide:13C(3)"] = 74.047178; // alkylation of heavy acrylamide to cysteines

  (*modMonoisotopicMassTable)["Oxidation"] = (*AAMonoisotopicMassTable)['o'];
  
  (*modMonoisotopicMassTable)["Acetyl"] = 42.010565; // acetylation of N terminus
  
  (*modMonoisotopicMassTable)["Deamidation"] =  0.984016;
  (*modMonoisotopicMassTable)["Deamidated"] =  0.984016; // PSI new name

  (*modMonoisotopicMassTable)["Pyro-cmC"] = 39.994915; // cyclicization of N-terminal CAM-cysteine (FIXED value 01/27/07)
  (*modMonoisotopicMassTable)["Pyro-carbamidomethyl"] = 39.994915; // PSI new name
 
  (*modMonoisotopicMassTable)["Pyro-glu"] = -17.026549; // loss of NH3 from glutamine
  (*modMonoisotopicMassTable)["Gln->pyro-Glu"] = -17.026549; // PSI new name
  
  (*modMonoisotopicMassTable)["Pyro_glu"] =  -18.010565; // loss of H2O from glutamic acid
  (*modMonoisotopicMassTable)["Glu->pyro-Glu"] =  -18.010565; // PSI new name

  (*modMonoisotopicMassTable)["Amide"] =  -0.984016; // amidation of C terminus
  (*modMonoisotopicMassTable)["Amidated"] =  -0.984016; // PSI new name

  (*modMonoisotopicMassTable)["Phospho"] =  79.966331; // phosphorylation

  (*modMonoisotopicMassTable)["Sulfo"] = 79.956815; // O-sulfonation

  (*modMonoisotopicMassTable)["Methyl"] =  14.015650; // methylation
  
  (*modMonoisotopicMassTable)["Carbamyl"] =  43.005814; // carbamylation of N terminus or lysines
  
  (*modMonoisotopicMassTable)["iTRAQ4plex"] =  144.102063; // iTRAQ 4-plex
  
  (*modMonoisotopicMassTable)["PEO-Iodoacetyl-LC-Biotin"] =  414.52; // Hui Zhang's PEO alkylation agent on cysteines

  (*modMonoisotopicMassTable)["Label:2H(3)"] = 3.018830; // SILAC heavy leucine (+3)
  (*modMonoisotopicMassTable)["Label:13C(6)"] = 6.020129; // SILAC heavy lysine and arginine (+6)
  (*modMonoisotopicMassTable)["Label:13C(6)15N(1)"] = 7.017165;
  (*modMonoisotopicMassTable)["Label:13C(6)15N(2)"] = 8.014199; // SILAC heavy lysine (+8)
  (*modMonoisotopicMassTable)["Label:13C(6)15N(3)"] = 9.011235;  
  (*modMonoisotopicMassTable)["Label:13C(6)15N(4)"] = 10.008269; // SILAC heavy arginine (+10)
  
  (*modMonoisotopicMassTable)["Methylthio"] = 45.987721; // methylthiolated cysteine (cys blocking by MMTS)

  (*modMonoisotopicMassTable)["Leucyl"] = 113.08406; // leucine added to N-term or K
  (*modMonoisotopicMassTable)["Leucyl:13C(6)15N(1)"] = 120.101224; // heavy leucine added to N-term or K

  
  (*modMonoisotopicMassTable)["Nitro"] = 44.985078;
  (*modMonoisotopicMassTable)["Dimethyl"] = 28.031300;
  (*modMonoisotopicMassTable)["Trimethyl"] = 42.046950;
  
  (*modMonoisotopicMassTable)["Bromo"] = 77.910511;

  // Ubl chains
  (*modMonoisotopicMassTable)["SUMO_1"] = 2135.920495; // SUMO-1 Tryptic/LysC tail
  (*modMonoisotopicMassTable)["SUMO_2_3_Tryp"] = 3549.536567; // SUMO-2/3 Tryptic tail
  (*modMonoisotopicMassTable)["Smt3_R93A_Tryp"] = 3812.747563; // Smt3_R93A Tryptic tail
  (*modMonoisotopicMassTable)["Smt3_R93A_LysC"] = 4544.074787; // Smt3_R93A LysC tail
  (*modMonoisotopicMassTable)["NEDD8_LysC"] = 1555.956231; // NEDD8 LysC tail
  (*modMonoisotopicMassTable)["Rub1_LysC"] = 2454.341699; // Rub1 LysC tail
  (*modMonoisotopicMassTable)["Ub_LysC"] = 1431.831075; // Ubiquitin LysC tail
  (*modMonoisotopicMassTable)["GlyGly"] = 114.042927; // Ubiquitin/NEDD8 Tryptic tail (2 glycines)

  
  // mod tokens - maps a mod token to a modification specification -- not complete, need to add as we encounter new stuff
  Peptide::modTokenTable = new map<string, string>();

  (*modTokenTable)["n[2]"] = "Deamidated";
  (*modTokenTable)["n[43]"] = "Acetyl";
  (*modTokenTable)["n[44]"] = "Carbamyl";
  (*modTokenTable)["n[58]"] = "Carbamidomethyl";  // rarer artefact of iodoacetamide treatment
  (*modTokenTable)["n[114]"] = "Leucyl";
  (*modTokenTable)["n[121]"] = "Leucyl:13C(6)15N(1)";
  (*modTokenTable)["n[145]"] = "iTRAQ4plex";  
  
  (*modTokenTable)["c[31]"] = "Methyl";
    
  (*modTokenTable)["C[119]"] = "Oxidation";
  (*modTokenTable)["C[143]"] = "Pyro-carbamidomethyl";
  (*modTokenTable)["C[145]"] = "Acetyl";
  (*modTokenTable)["C[149]"] = "Methylthio";
  (*modTokenTable)["C[160]"] = "Carbamidomethyl";
  (*modTokenTable)["C[161]"] = "Carboxymethyl";
  (*modTokenTable)["C[174]"] = "Propionamide";
  (*modTokenTable)["C[177]"] = "Propionamide:13C(3)";
  (*modTokenTable)["C[330]"] = "ICAT-C";
  (*modTokenTable)["C[339]"] = "ICAT-C:13C(9)";
  (*modTokenTable)["C[518]"] = "PEO-Iodoacetyl-LC-Biotin";
  (*modTokenTable)["C[545]"] = "ICAT-D"; // some pepXML's has 545 for this... probably due to adding the monoisotopic mass of the ICAT-tag 
  (*modTokenTable)["C[546]"] = "ICAT-D";
  (*modTokenTable)["C[553]"] = "ICAT-D:2H(8)"; // some pepXML's has 553 for this... probably due to adding the monoisotopic mass of the ICAT-tag 
  (*modTokenTable)["C[554]"] = "ICAT-D:2H(8)";
  
  (*modTokenTable)["D[129]"] = "Methyl";
  (*modTokenTable)["D[131]"] = "Oxidation";
  (*modTokenTable)["D[172]"] = "Carbamidomethyl";  // rarer artefact of iodoacetamide treatment
    
  (*modTokenTable)["E[111]"] = "Glu->pyro-Glu";
  (*modTokenTable)["E[143]"] = "Methyl";
  (*modTokenTable)["E[186]"] = "Carbamidomethyl";  // rarer artefact of iodoacetamide treatment
  
  (*modTokenTable)["H[194]"] = "Carbamidomethyl";  // rarer artefact of iodoacetamide treatment

  (*modTokenTable)["K[134]"] = "Label:13C(6)"; // SILAC heavy lysine (+6)
  (*modTokenTable)["K[135]"] = "Label:13C(6)15N(1)"; // SILAC heavy lysine (+7)
  (*modTokenTable)["K[136]"] = "Label:13C(6)15N(2)";	// SILAC heavy lysine (+8)
  (*modTokenTable)["K[142]"] = "Methyl";
  (*modTokenTable)["K[144]"] = "Oxidation"; // hydroxylysine
  (*modTokenTable)["K[156]"] = "Dimethyl";
  (*modTokenTable)["K[170]"] = "Acetyl";
  (*modTokenTable)["K[Acetyl]"] = "Acetyl";
  (*modTokenTable)["K[Trimethyl]"] = "Trimethyl";
  (*modTokenTable)["K[171]"] = "Carbamyl";
  (*modTokenTable)["K[185]"] = "Carbamidomethyl";  // rarer artefact of iodoacetamide treatment
  (*modTokenTable)["K[241]"] = "Leucyl";
  (*modTokenTable)["K[242]"] = "GlyGly";
  (*modTokenTable)["K[248]"] = "Leucyl:13C(6)15N(1)";
  (*modTokenTable)["K[272]"] = "iTRAQ4plex";
  (*modTokenTable)["K[Trimethyl]"] = "Trimethyl";

  (*modTokenTable)["L[116]"] = "Label:2H(3)";
  
  (*modTokenTable)["M(O)"] = "Oxidation"; // this token is used by some older .msp files
  (*modTokenTable)["M[147]"] = "Oxidation";

  (*modTokenTable)["N[115]"] = "Deamidated";
  (*modTokenTable)["N[130]"] = "Oxidation";
  
  (*modTokenTable)["P[113]"] = "Oxidation"; // hydroxyproline
  
  (*modTokenTable)["Q[111]"] = "Gln->pyro-Glu";
  (*modTokenTable)["Q[129]"] = "Deamidated";

  (*modTokenTable)["R[162]"] = "Label:13C(6)"; // SILAC heavy arginine (+6)
  (*modTokenTable)["R[165]"] = "Label:13C(6)15N(3)"; // SILAC heavy arginine (+9)
  (*modTokenTable)["R[166]"] = "Label:13C(6)15N(4)"; // SILAC heavy arginine (+10)
  (*modTokenTable)["R[170]"] = "Methyl";
  (*modTokenTable)["R[172]"] = "Oxidation";
  (*modTokenTable)["R[184]"] = "Dimethyl";
  (*modTokenTable)["R[198]"] = "Trimethyl";

  (*modTokenTable)["S[129]"] = "Acetyl";
  (*modTokenTable)["S[167]"] = "Phospho";

  (*modTokenTable)["T[143]"] = "Acetyl";
  (*modTokenTable)["T[181]"] = "Phospho";
  
  (*modTokenTable)["W[202]"] = "Oxidation";
  (*modTokenTable)["W[231]"] = "Nitro";
  
  (*modTokenTable)["Y[179]"] = "Oxidation";
  (*modTokenTable)["Y[208]"] = "Nitro";
  (*modTokenTable)["Y[243]"] = "Phospho";
  (*modTokenTable)["Y[Sulfo]"] = "Sulfo";

  // Ubl chains
  (*modTokenTable)["K[SUMO_1]"] = "SUMO_1"; // SUMO-1 Tryptic/LysC tail
  (*modTokenTable)["K[SUMO_2_3_Tryp]"] = "SUMO_2_3_Tryp"; // SUMO-2/3 Tryptic tail
  (*modTokenTable)["K[Smt3_R93A_Tryp]"] = "Smt3_R93A_Tryp"; // Smt3_R93A Tryptic tail
  (*modTokenTable)["K[Smt3_R93A_LysC]"] = "Smt3_R93A_LysC"; // Smt3_R93A LysC tail
  (*modTokenTable)["K[NEDD8_LysC]"] = "NEDD8_LysC"; // NEDD8 LysC tail
  (*modTokenTable)["K[Rub1_LysC]"] = "Rub1_LysC"; // Rub1 LysC tail
  (*modTokenTable)["K[Ub_LysC]"] = "Ub_LysC"; // Ubiquitin LysC tail
  (*modTokenTable)["K[GlyGly]"] = "GlyGly"; // Ubiquitin/NEDD8 Tryptic tail (2 glycines)

  // neutral loss tables - lists likely neutral losses from each amino acid and modification groups
  Peptide::AAMonoisotopicNeutralLossTable = new map<char, double*>();
  (*AAMonoisotopicNeutralLossTable)['A'] = NULL;  // NULL means no neutral loss expected
  (*AAMonoisotopicNeutralLossTable)['B'] = new double[6];
  (*AAMonoisotopicNeutralLossTable)['B'][0] = 18.010565; // H2O
  (*AAMonoisotopicNeutralLossTable)['B'][1] = 17.026549; // NH3
  (*AAMonoisotopicNeutralLossTable)['B'][2] = 43.98982; // CO2
  (*AAMonoisotopicNeutralLossTable)['B'][3] = 45.02146; // CONH2
  (*AAMonoisotopicNeutralLossTable)['B'][4] = 46.00548; // HCOOH
  (*AAMonoisotopicNeutralLossTable)['B'][5] = 0.0; // ALWAYS END THE LIST WITH a 0.0
  (*AAMonoisotopicNeutralLossTable)['C'] = NULL;
  (*AAMonoisotopicNeutralLossTable)['D'] = new double[4];
  (*AAMonoisotopicNeutralLossTable)['D'][0] = 18.010565; // H2O
  (*AAMonoisotopicNeutralLossTable)['D'][1] = 43.98982; // CO2
  (*AAMonoisotopicNeutralLossTable)['D'][2] = 46.00548; // HCOOH
  (*AAMonoisotopicNeutralLossTable)['D'][3] = 0.0; 
  (*AAMonoisotopicNeutralLossTable)['E'] = new double[4];
  (*AAMonoisotopicNeutralLossTable)['E'][0] = 18.010565; // H2O
  (*AAMonoisotopicNeutralLossTable)['E'][1] = 43.98982; // CO2
  (*AAMonoisotopicNeutralLossTable)['E'][2] = 46.00548; // HCOOH
  (*AAMonoisotopicNeutralLossTable)['E'][3] = 0.0; // CO2  
  (*AAMonoisotopicNeutralLossTable)['F'] = NULL;
  (*AAMonoisotopicNeutralLossTable)['G'] = NULL;
  (*AAMonoisotopicNeutralLossTable)['H'] = NULL;
  (*AAMonoisotopicNeutralLossTable)['I'] = NULL;
  (*AAMonoisotopicNeutralLossTable)['K'] = new double[2];
  (*AAMonoisotopicNeutralLossTable)['K'][0] = 17.026549; // NH3
  (*AAMonoisotopicNeutralLossTable)['K'][1] = 0.0;
  (*AAMonoisotopicNeutralLossTable)['L'] = NULL;
  (*AAMonoisotopicNeutralLossTable)['M'] = NULL;
  (*AAMonoisotopicNeutralLossTable)['N'] = new double[3];
  (*AAMonoisotopicNeutralLossTable)['N'][0] = 17.026549; // NH3
  (*AAMonoisotopicNeutralLossTable)['N'][1] = 45.02146; // CONH3
  (*AAMonoisotopicNeutralLossTable)['N'][2] = 0.0;
  (*AAMonoisotopicNeutralLossTable)['P'] = NULL;
  (*AAMonoisotopicNeutralLossTable)['Q'] = new double[3];
  (*AAMonoisotopicNeutralLossTable)['Q'][0] = 17.026549; // NH3
  (*AAMonoisotopicNeutralLossTable)['Q'][1] = 45.02146; // CONH3
  (*AAMonoisotopicNeutralLossTable)['Q'][2] = 0.0;
  (*AAMonoisotopicNeutralLossTable)['R'] = new double[2];
  (*AAMonoisotopicNeutralLossTable)['R'][0] = 17.026549; // NH3
  (*AAMonoisotopicNeutralLossTable)['R'][1] = 0.0;
  (*AAMonoisotopicNeutralLossTable)['S'] = new double[2];
  (*AAMonoisotopicNeutralLossTable)['S'][0] = 18.010565; // H2O
  (*AAMonoisotopicNeutralLossTable)['S'][1] = 0.0;
  (*AAMonoisotopicNeutralLossTable)['T'] = new double[2];
  (*AAMonoisotopicNeutralLossTable)['T'][0] = 18.010565; // H2O
  (*AAMonoisotopicNeutralLossTable)['T'][1] = 0.0;
  (*AAMonoisotopicNeutralLossTable)['V'] = NULL;
  (*AAMonoisotopicNeutralLossTable)['W'] = NULL;
  (*AAMonoisotopicNeutralLossTable)['X'] = NULL;
  (*AAMonoisotopicNeutralLossTable)['Y'] = NULL;
  (*AAMonoisotopicNeutralLossTable)['Z'] = new double[6];
  (*AAMonoisotopicNeutralLossTable)['Z'][0] = 18.010565; // H2O
  (*AAMonoisotopicNeutralLossTable)['Z'][1] = 17.026549; // NH3
  (*AAMonoisotopicNeutralLossTable)['Z'][2] = 43.98982; // CO2
  (*AAMonoisotopicNeutralLossTable)['Z'][3] = 45.02146; // CONH3
  (*AAMonoisotopicNeutralLossTable)['Z'][4] = 46.00548; // HCOOH
  (*AAMonoisotopicNeutralLossTable)['Z'][5] = 0.0;
  
  Peptide::modMonoisotopicNeutralLossTable = new map<string, double*>();
  // NOTE: I'm still trying to determine what some of these are. But the fact is that these are indeed common neutral losses from experimental spectra. I've borrowed these from NIST's library building code.
  (*modMonoisotopicNeutralLossTable)["ICAT-C"] = new double[3]; 
  (*modMonoisotopicNeutralLossTable)["ICAT-C"][0] = 102.4; 
  (*modMonoisotopicNeutralLossTable)["ICAT-C"][1] = 127.0; 
  (*modMonoisotopicNeutralLossTable)["ICAT-C"][2] = 0.0; 
  (*modMonoisotopicNeutralLossTable)["ICAT-C:13C(9)"] =  new double[3]; 
  (*modMonoisotopicNeutralLossTable)["ICAT-C:13C(9)"][0] = 102.4; 
  (*modMonoisotopicNeutralLossTable)["ICAT-C:13C(9)"][1] = 127.0; 
  (*modMonoisotopicNeutralLossTable)["ICAT-C:13C(9)"][2] = 0.0; 
  (*modMonoisotopicNeutralLossTable)["ICAT-D"] = new double[2];
  (*modMonoisotopicNeutralLossTable)["ICAT-D"][0] = 403.2;
  (*modMonoisotopicNeutralLossTable)["ICAT-D"][1] = 0.0;
  (*modMonoisotopicNeutralLossTable)["ICAT-D:2H(8)"] = new double[2];
  (*modMonoisotopicNeutralLossTable)["ICAT-D:2H(8)"][0] = 411.2;
  (*modMonoisotopicNeutralLossTable)["ICAT-D:2H(8)"][1] = 0.0;
  (*modMonoisotopicNeutralLossTable)["Carbamidomethyl"] = new double[2];
  (*modMonoisotopicNeutralLossTable)["Carbamidomethyl"][0] = 91.009195; // HS-CH2-CONH2
  (*modMonoisotopicNeutralLossTable)["Carbamidomethyl"][1] = 0.0;
  (*modMonoisotopicNeutralLossTable)["Carboxymethyl"] = new double[2];
  (*modMonoisotopicNeutralLossTable)["Carboxymethyl"][0] = 91.993211; // HS-CH2-COOH
  (*modMonoisotopicNeutralLossTable)["Carboxymethyl"][1] = 0.0;
  (*modMonoisotopicNeutralLossTable)["Oxidation"] = new double[3];
  (*modMonoisotopicNeutralLossTable)["Oxidation"][0] = 63.998301; // HO-S-CH3 (really only for oxidation on methionine... fix later
  (*modMonoisotopicNeutralLossTable)["Oxidation"][1] = 82.008866; // HO-S-CH3 + H2O
  (*modMonoisotopicNeutralLossTable)["Oxidation"][2] = 0.0;

  (*modMonoisotopicNeutralLossTable)["Acetyl"] = NULL;
  (*modMonoisotopicNeutralLossTable)["Deamidated"] =  NULL; 
  (*modMonoisotopicNeutralLossTable)["Pyro-carbamidomethyl"] = NULL; 
  (*modMonoisotopicNeutralLossTable)["Gln->pyro-Glu"] = NULL; 
  (*modMonoisotopicNeutralLossTable)["Glu->pyro-Glu"] =  NULL; 
  (*modMonoisotopicNeutralLossTable)["Amidated"] = NULL; 
  (*modMonoisotopicNeutralLossTable)["Phospho"] = new double[4]; 
  (*modMonoisotopicNeutralLossTable)["Phospho"][0] = 79.966331; // HPO3
  (*modMonoisotopicNeutralLossTable)["Phospho"][1] = 97.976896; // H3PO4
  (*modMonoisotopicNeutralLossTable)["Phospho"][2] = 115.987461; // H3PO4 + H2O
  (*modMonoisotopicNeutralLossTable)["Phospho"][3] = 0.0; 
  (*modMonoisotopicNeutralLossTable)["Sulfo"] = new double[2]; 
  (*modMonoisotopicNeutralLossTable)["Sulfo"][0] = 79.956815; // SO3
  (*modMonoisotopicNeutralLossTable)["Sulfo"][1] = 0.0;
  (*modMonoisotopicNeutralLossTable)["Methyl"] =  NULL; 
  (*modMonoisotopicNeutralLossTable)["Carbamyl"] =  NULL; 
  (*modMonoisotopicNeutralLossTable)["iTRAQ4plex"] =  NULL; 
  (*modMonoisotopicNeutralLossTable)["PEO-Iodoacetyl-LC-Biotin"] =  NULL;
   
  
  // no average mass neutral loss tables yet -- SpectraST doesn't need them anyway... Will add them later.
  
  
  // immonium ion tables - lists likely immonium ions from amino acids
  Peptide::AAMonoisotopicImmoniumTable = new map<string, double*>();
  (*AAMonoisotopicImmoniumTable)["A"] = new double[2];
  (*AAMonoisotopicImmoniumTable)["A"][0] = 44.05003;
  (*AAMonoisotopicImmoniumTable)["A"][1] = 0.0;
  (*AAMonoisotopicImmoniumTable)["B"] = NULL; 
  (*AAMonoisotopicImmoniumTable)["C"] = NULL;
  (*AAMonoisotopicImmoniumTable)["C[160]"] = new double[2];
  (*AAMonoisotopicImmoniumTable)["C[160]"][0] = 133.04360;
  (*AAMonoisotopicImmoniumTable)["C[160]"][1] = 0.0;
  (*AAMonoisotopicImmoniumTable)["C[161]"] = new double[2];
  (*AAMonoisotopicImmoniumTable)["C[161]"][0] = 134.02760;
  (*AAMonoisotopicImmoniumTable)["C[161]"][1] = 0.0;
  (*AAMonoisotopicImmoniumTable)["C[174]"] = new double[2];
  (*AAMonoisotopicImmoniumTable)["C[174]"][0] = 147.07720;
  (*AAMonoisotopicImmoniumTable)["C[174]"][1] = 0.0;
  (*AAMonoisotopicImmoniumTable)["D"] = NULL;
  (*AAMonoisotopicImmoniumTable)["E"] = NULL;  
  (*AAMonoisotopicImmoniumTable)["F"] = new double[2];
  (*AAMonoisotopicImmoniumTable)["F"][0] = 120.08130; 
  (*AAMonoisotopicImmoniumTable)["F"][1] = 0.0;  
  (*AAMonoisotopicImmoniumTable)["G"] = new double[2];
  (*AAMonoisotopicImmoniumTable)["G"][0] = 30.03438; 
  (*AAMonoisotopicImmoniumTable)["G"][1] = 0.0;  
  (*AAMonoisotopicImmoniumTable)["H"] = new double[3];
  (*AAMonoisotopicImmoniumTable)["H"][0] = 110.07180; 
  (*AAMonoisotopicImmoniumTable)["H"][1] = 166.0; 
  (*AAMonoisotopicImmoniumTable)["H"][2] = 0.0;  
  (*AAMonoisotopicImmoniumTable)["I"] = new double[3];
  (*AAMonoisotopicImmoniumTable)["I"][0] = 86.09698;
  (*AAMonoisotopicImmoniumTable)["I"][1] = 44.0; 
  (*AAMonoisotopicImmoniumTable)["I"][2] = 0.0;  
  (*AAMonoisotopicImmoniumTable)["K"] = new double[5];
  (*AAMonoisotopicImmoniumTable)["K"][0] = 101.10790;
  (*AAMonoisotopicImmoniumTable)["K"][1] = 84.08136;
  (*AAMonoisotopicImmoniumTable)["K"][2] = 112.0;
  (*AAMonoisotopicImmoniumTable)["K"][3] = 129.0;
  (*AAMonoisotopicImmoniumTable)["K"][4] = 0.0;
  (*AAMonoisotopicImmoniumTable)["L"] = new double[3];
  (*AAMonoisotopicImmoniumTable)["L"][0] = 86.09698; 
  (*AAMonoisotopicImmoniumTable)["L"][1] = 44.0; 
  (*AAMonoisotopicImmoniumTable)["L"][2] = 0.0;  
  (*AAMonoisotopicImmoniumTable)["M"] = new double[3];
  (*AAMonoisotopicImmoniumTable)["M"][0] = 104.0534; 
  (*AAMonoisotopicImmoniumTable)["M"][1] = 61.0; 
  (*AAMonoisotopicImmoniumTable)["M"][2] = 0.0; 
  (*AAMonoisotopicImmoniumTable)["M[147]"] = new double[2];
  (*AAMonoisotopicImmoniumTable)["M[147]"][0] = 120.04830;
  (*AAMonoisotopicImmoniumTable)["M[147]"][1] = 0.0;
  (*AAMonoisotopicImmoniumTable)["N"] = NULL;
  (*AAMonoisotopicImmoniumTable)["P"] = new double[2];
  (*AAMonoisotopicImmoniumTable)["P"][0] = 70.06568; 
  (*AAMonoisotopicImmoniumTable)["P"][1] = 0.0;
  (*AAMonoisotopicImmoniumTable)["Q"] = new double[4];
  (*AAMonoisotopicImmoniumTable)["Q"][0] = 101.07150;
  (*AAMonoisotopicImmoniumTable)["Q"][1] = 84.0;
  (*AAMonoisotopicImmoniumTable)["Q"][2] = 56.0;
  (*AAMonoisotopicImmoniumTable)["Q"][3] = 0.0;
  (*AAMonoisotopicImmoniumTable)["R"] = new double[7];
  (*AAMonoisotopicImmoniumTable)["R"][0] = 129.11400;
  (*AAMonoisotopicImmoniumTable)["R"][1] = 112.0;
  (*AAMonoisotopicImmoniumTable)["R"][2] = 100.0;
  (*AAMonoisotopicImmoniumTable)["R"][3] = 87.0;
  (*AAMonoisotopicImmoniumTable)["R"][4] = 70.0;
  (*AAMonoisotopicImmoniumTable)["R"][5] = 59.0;
  (*AAMonoisotopicImmoniumTable)["R"][6] = 0.0;
  (*AAMonoisotopicImmoniumTable)["S"] = new double[2];
  (*AAMonoisotopicImmoniumTable)["S"][0] = 60.04494;
  (*AAMonoisotopicImmoniumTable)["S"][1] = 0.0;
  (*AAMonoisotopicImmoniumTable)["T"] = new double[2];
  (*AAMonoisotopicImmoniumTable)["T"][0] = 74.06059;
  (*AAMonoisotopicImmoniumTable)["T"][1] = 0.0;
  (*AAMonoisotopicImmoniumTable)["V"] = new double[5];
  (*AAMonoisotopicImmoniumTable)["V"][0] = 72.08133;
  (*AAMonoisotopicImmoniumTable)["V"][1] = 69.0;
  (*AAMonoisotopicImmoniumTable)["V"][2] = 55.0;
  (*AAMonoisotopicImmoniumTable)["V"][3] = 41.0;
  (*AAMonoisotopicImmoniumTable)["V"][4] = 0.0;
  (*AAMonoisotopicImmoniumTable)["W"] = new double[7];
  (*AAMonoisotopicImmoniumTable)["W"][0] = 159.09220;
  (*AAMonoisotopicImmoniumTable)["W"][1] = 130.0;
  (*AAMonoisotopicImmoniumTable)["W"][2] = 117.0; 
  (*AAMonoisotopicImmoniumTable)["W"][3] = 77.0;  
  (*AAMonoisotopicImmoniumTable)["W"][4] = 170.0; 
  (*AAMonoisotopicImmoniumTable)["W"][5] = 171.0; 
  (*AAMonoisotopicImmoniumTable)["W"][6] = 0.0;
  (*AAMonoisotopicImmoniumTable)["X"] = NULL;
  (*AAMonoisotopicImmoniumTable)["Y"] = new double[2];
  (*AAMonoisotopicImmoniumTable)["Y"][0] = 136.07620;
  (*AAMonoisotopicImmoniumTable)["Y"][1] = 0.0;
  (*AAMonoisotopicImmoniumTable)["Y[243]"] = new double[2];
  (*AAMonoisotopicImmoniumTable)["Y[243]"][0] = 216.0;
  (*AAMonoisotopicImmoniumTable)["Y[243]"][1] = 0.0;
  (*AAMonoisotopicImmoniumTable)["Z"] = NULL;
  
}

// deleteTables - frees the memory associated with the tables. 
// (Best to call at the end of any program using the Peptide class)
void Peptide::deleteTables() {
	
  if (AAAverageMassTable) {
    delete (AAAverageMassTable);
  }
  if (modAverageMassTable) {
    delete (modAverageMassTable);
  }
  
  if (AAMonoisotopicMassTable) {
    delete (AAMonoisotopicMassTable);
  }
  if (modMonoisotopicMassTable) {
    delete (modMonoisotopicMassTable);
  }
  
  if (modTokenTable) {
    delete (modTokenTable);
  }
  
  if (AAMonoisotopicNeutralLossTable) {
    for (map<char, double*>::iterator i = AAMonoisotopicNeutralLossTable->begin(); i != AAMonoisotopicNeutralLossTable->end(); i++) {
      if (i->second) {
        delete[] i->second;
      }
    }
    delete (AAMonoisotopicNeutralLossTable);
  }
  
  if (modMonoisotopicNeutralLossTable) {
    for (map<string, double*>::iterator i = modMonoisotopicNeutralLossTable->begin(); i != modMonoisotopicNeutralLossTable->end(); i++) {
      if (i->second) {
        delete[] i->second;
      }
    }
    delete (modMonoisotopicNeutralLossTable);
  }
  
  if (AAMonoisotopicImmoniumTable) {
    for (map<string, double*>::iterator i = AAMonoisotopicImmoniumTable->begin(); i != AAMonoisotopicImmoniumTable->end(); i++) {
      if (i->second) {
        delete[] i->second;
      }
    }
    delete (AAMonoisotopicImmoniumTable);
  }
  
}

// getModAverageMass - retrieves the average mass of a mod
double Peptide::getModAverageMass(string modType) {
	
  map<string, double>::iterator found = modAverageMassTable->find(modType);
  if (found != modAverageMassTable->end()) {
    return (found->second); 
  } else if ((found = modMonoisotopicMassTable->find(modType)) != modMonoisotopicMassTable->end()) {
    return (calcApproximateAverageMass(found->second));
  }
  return (0.0);	
}

// getModMonoisotopicMass - retrieves the monoisotopic mass of a mod
double Peptide::getModMonoisotopicMass(string modType) {
	
  map<string, double>::iterator found = modMonoisotopicMassTable->find(modType);
  if (found != modMonoisotopicMassTable->end()) {
    return (found->second); 
  } 
  return (0.0);
  
}

double Peptide::getAAAverageMass(char aa) {
  
  map<char, double>::iterator found = AAAverageMassTable->find(aa);
  if (found != AAAverageMassTable->end()) {
    return (found->second); 
  } 
  return (0.0);
   
}

double Peptide::getAAMonoisotopicMass(char aa) {
  
  map<char, double>::iterator found = AAMonoisotopicMassTable->find(aa);
  if (found != AAMonoisotopicMassTable->end()) {
    return (found->second); 
  } 
  return (0.0);
   
}


// getAAPlusModAverageMass - calculates the AA+mod average mass for a particular AA and mod type
double Peptide::getAAPlusModAverageMass(char aa, string modType) {
	
  return (getAAAverageMass(aa) + getModAverageMass(modType)); 
	
}

// getAAPlusModMonoisotopticMass - calculates the AA+mod monoisotopic mass for a particular AA and mod type
double Peptide::getAAPlusModMonoisotopicMass(char aa, string modType) {
	
  return (getAAMonoisotopicMass(aa) + getModMonoisotopicMass(modType)); 

}

// addModTokenToTables - call to add tokens to the default tables, so that the parsers know what to do with them.
void Peptide::addModTokenToTables(string token, string modType) {
	
  // Use to add modification tokens. Examples include:
  // addModTokenToTables("C", "ICAT_light");
  // addModTokenToTables("C*", "ICAT_heavy");
  // etc.
  
  if (!AAAverageMassTable || !modAverageMassTable || !AAMonoisotopicMassTable || !modMonoisotopicMassTable || !modTokenTable) {
    defaultTables();
  }
  
  if (modAverageMassTable->find(modType) == modAverageMassTable->end() ||
      modMonoisotopicMassTable->find(modType) == modMonoisotopicMassTable->end()) {
    // no such mod!
    cerr << "The modification name \"" << modType << "\" is not known. Ignored." << endl;
    return;
  }	
  
  (*modTokenTable)[token] = modType;
}

// isNTermModType - checks if the modType is a N-terminal mod
bool Peptide::isNTermModType(string modType) {
  
  return (modType == "Acetyl" || 
          modType == "Carbamyl" ||
          modType == "iTRAQ4plex" ||
	  modType == "Leucyl:13C(6)15N(1)" ||
	  modType == "Leucyl" );
  
}

// isCTermModType - checks if the modType is a C-terminal mod
bool Peptide::isCTermModType(string modType) {
	
  return (modType == "Amidated" ||
          modType == "Amide" ||
          modType == "Methyl" );
}


// ===============================================================================
// METHOD TO GENERATE A THEORETICAL SPECTRUM (A LA SEQUEST) FOR THIS PEPTIDE ION
// SEQUESTTheoreticalSpectrum - calculates what the SEQUEST theoretical spectrum for this
// peptide would look like (maybe not 100% correct)
void Peptide::SEQUESTTheoreticalSpectrum(map<int, float>& peaks) {
		
  unsigned int maxIonCharge = 1;
  if (charge == 1) {
    maxIonCharge = 1;
  } else {
    maxIonCharge = charge - 1;
  }
  
  for (unsigned int i = 1; i < NAA(); i++) {
    for (unsigned int ch = 1; ch <= maxIonCharge; ch++) {   
      // only consider fragment charge state up to precursor charge state minus 1
      
      // y ions
      double mz = averageMZFragment('y', i, ch);
      int intMz = (int)(mz + 0.5);
      map<int, float>::iterator found = peaks.find(intMz);	
      if (found == peaks.end()) {
        peaks[intMz] = 50.0;
      } else {
        found->second += 50.0;
      }
      found = peaks.find(intMz + 1);	
      if (found == peaks.end()) {
        peaks[intMz + 1] = 25.0;
      } else {
        found->second += 25.0;
      }
      found = peaks.find(intMz - 1);	
      if (found == peaks.end()) {
        peaks[intMz - 1] = 25.0;
      } else {
        found->second += 25.0;
      }
      found = peaks.find((int)(mz - 17.0/(double)ch + 0.5));	
      if (found == peaks.end()) {
        peaks[(int)(mz - 17.0/(double)ch + 0.5)] = 10.0;
      } else {
        found->second += 10.0;
      }
      found = peaks.find((int)(mz - 18.0/(double)ch + 0.5));	
      if (found == peaks.end()) {
        peaks[(int)(mz - 18.0/(double)ch + 0.5)] = 10.0;
      } else {
        found->second += 10.0;
      }
      
      // b ions
      mz = averageMZFragment('b', i, ch);
      intMz = (int)(mz + 0.5);
      found = peaks.find(intMz);	
      if (found == peaks.end()) {
        peaks[intMz] = 50.0;
      } else {
        found->second += 50.0;
      }
      found = peaks.find(intMz + 1);	
      if (found == peaks.end()) {
        peaks[intMz + 1] = 25.0;
      } else {
        found->second += 25.0;
      }
      found = peaks.find(intMz - 1);	
      if (found == peaks.end()) {
        peaks[intMz - 1] = 25.0;
      } else {
        found->second += 25.0;
      }
      found = peaks.find((int)(mz - 17.0/(double)ch + 0.5));	
      if (found == peaks.end()) {
        peaks[(int)(mz - 17.0/(double)ch + 0.5)] = 10.0;
      } else {
        found->second += 10.0;
      }
      found = peaks.find((int)(mz - 18.0/(double)ch + 0.5));	
      if (found == peaks.end()) {
        peaks[(int)(mz - 18.0/(double)ch + 0.5)] = 10.0;
      } else {
        found->second += 10.0;
      }
      
      // a ions
      mz = averageMZFragment('a', i, ch);
      intMz = (int)(mz + 0.5);
      found = peaks.find(intMz);	
      if (found == peaks.end()) {
        peaks[intMz] = 10.0;
      } else {
        found->second += 10.0;
      }
      
    }
    
  }
  
  for (map<int, float>::iterator p = peaks.begin(); p != peaks.end(); p++) {
    if (p->second > 50.0) {
      p->second = 50.0;
    }
  }
  
}


// ===============================================================================
// METHODS TO CREATE ALL COMMON FRAGMENT IONS (FOR ANNOTATION OF PEAK LIST)

// generateFragmentIons - generates all theoretical fragment ions for this peptides.
// They include precursor, y and b (for all charges <= precursor charge) with neutral losses (see NeutralLossTable), and a ions.
void Peptide::generateFragmentIons(vector<FragmentIon*>& ions, string fragmentationType) {

  if (fragmentationType.empty() || fragmentationType == "CID" || fragmentationType == "CID-QTOF" || fragmentationType == "HCD") {

    generateFragmentIonsCID(ions);

  } else if (fragmentationType == "ETD" || fragmentationType == "ETD-SA") {

    generateFragmentIonsETD(ions);
  
  } else {

    generateFragmentIonsCID(ions); // CID by default

  }

}

void Peptide::generateFragmentIonsCID(vector<FragmentIon*>& ions) {

  double precursorMH = 0.0;
  
  for (unsigned int ch = 1; ch <= (unsigned int)charge; ch++) {
    
    map<int, double> losses; // store losses as pairs of (int value of mod mass, double value of mod mass)
    double sum = 0.0;
    
    
    // BEGIN y ions and precursor
    
    // add a water for the y ion
    sum += (*AAMonoisotopicMassTable)['!'];

    // add C-terminal modifcation mass, if any
    if (isModsSet && !cTermMod.empty()) {
      sum += getModMonoisotopicMass(cTermMod);
    }
    
    losses[18] = (*AAMonoisotopicMassTable)['!'];
    losses[44] = 43.98982; // CO2
    losses[46] = 46.00548; // HCOOH
    
    // add a proton for each charge
    sum += (double)ch * (*AAMonoisotopicMassTable)['+'];
    
    for (int i = (int)(stripped.length()) - 1; i >= 0; i--) { 
     
      sum += (*AAMonoisotopicMassTable)[stripped[i]];
        
      double* nls = (*AAMonoisotopicNeutralLossTable)[stripped[i]];
      if (nls) {
        unsigned int x = 0;
        double nl = 0.0;
	while((nl = nls[x++]) > 0.00001) {
          int intLoss = (int)(nl + 0.5);

	  // allow double H2O/NH3 losses
	  if (intLoss == 17 || intLoss == 18) {
	    if (losses.find(18) != losses.end()) {
	      losses[intLoss + 18] = nl + losses[18];
	    } else if (losses.find(17) != losses.end()) {
	      losses[intLoss + 17] = nl + losses[17];
	    }
	  }

	  losses[intLoss] = nl;
        }
  
      }
        
      if (isModsSet && !mods.empty()) {
        map<int, string>::iterator j = mods.find(i);
        if (j != mods.end()) {
          // modified at this position
          sum += getModMonoisotopicMass(j->second);
          double* nls = (*modMonoisotopicNeutralLossTable)[j->second];

	  // hack - loss of 64 only applies to methionine oxidation, not to other oxidations, so check:
	  if (j->second == "Oxidation" && stripped[i] != 'M') nls = NULL;

          if (nls) { 
            unsigned int x = 0;
            double nl = 0.0;
            while ((nl = nls[x++]) > 0.00001) {
              // hack - if neutral loss is too heavy - over 250 Da, this is really not a "neutral loss"
              // but rather a charge-carrying loss (e.g. the old ICAT losses), so we will not add
              // this fragment if the fragment charge == precursor charge. 
              if (nl > 250.0 && ch == charge) continue;

              losses[(int)(nl + 0.5)] = nl;
            }
          }
        }
      }
        
      
      if (i > 0) {
        // this is a y ion
        unsigned int position = NAA() - (unsigned int)i;
    	unsigned prom = 9;
	if (ch == charge && stripped[i] != 'P' && (double)position > (double)(stripped.length()) * 0.77) prom = 6;
        ions.push_back(new FragmentIon("y", position, 0, sum / (double)ch, ch, prom));
        
        for (map<int, double>::iterator n = losses.begin(); n != losses.end(); n++) {
	  prom = 4;
	  if (n->first == 17 || n->first == 18 || n->first == 64 || n->first == 91 || n->first == 98) {
	    if (ch == charge) {
	      prom = 5;
	    } else {
	      prom = 7;
	    }
	  }
          ions.push_back(new FragmentIon("y", position, n->first, (sum - n->second) / (double)ch, ch, prom));
        }
          
          
      } else {
        // this is really just the precursor!
        
        // add N-terminus mod, if any
        if (isModsSet && !nTermMod.empty()) {
          sum += getModMonoisotopicMass(nTermMod);
        }
        
        precursorMH = sum;
        
        // precursor -- don't consider all charges, since the precursor should carry all the charges 
        if (ch == charge) {
          ions.push_back(new FragmentIon("p", 0, 0, sum / (double)ch, ch, 9));
        
          for (map<int, double>::iterator n = losses.begin(); n != losses.end(); n++) {
            ions.push_back(new FragmentIon("p", 0, n->first, (sum - n->second) / (double)ch, ch, 9));
          }    
        }
      }
        
    }
    // END y ions and precursor
    
    // reset
    sum = 0.0;
    losses.clear();
    
    // BEGIN b and a ions
    
    // add the N-term modification mass, if any
    if (isModsSet && !nTermMod.empty()) {
      sum += getModMonoisotopicMass(nTermMod);
    }
    losses[17] = 17.026549; // loss of NH3

    // add a proton for each charge
    sum += (double)ch * (*AAMonoisotopicMassTable)['+'];
    
    bool hasBasicAA = false;
    
    for (int i = 0; i < (int)(stripped.length()) - 1; i++) {
      
      if (stripped[i] == 'R' || stripped[i] == 'K' || stripped[i] == 'H') hasBasicAA = true;
      
      sum += (*AAMonoisotopicMassTable)[stripped[i]];
      
      double* nls = (*AAMonoisotopicNeutralLossTable)[stripped[i]];

      if (nls) {
        unsigned int x = 0;
        double nl = 0.0;
        while ((nl = nls[x++]) > 0.00001) {
	  int intLoss = (int)(nl + 0.5);
	  
	  // allow double H2O/NH3 losses
	  if (intLoss == 17 || intLoss == 18) {
	    if (losses.find(18) != losses.end()) {
	      losses[intLoss + 18] = nl + losses[18];
	    } else if (losses.find(17) != losses.end()) {
	      losses[intLoss + 17] = nl + losses[17];
	    }
	  }
          losses[intLoss] = nl;
        }
      }
      
      if (isModsSet && !mods.empty()) {
        map<int, string>::iterator j = mods.find(i);
        if (j != mods.end()) {
          // modified at this position
          sum += getModMonoisotopicMass(j->second);
          double* nls = (*modMonoisotopicNeutralLossTable)[j->second];

	  // hack - loss of 64 only applies to methionine oxidation, not to other oxidations, so check:
	  if (j->second == "Oxidation" && stripped[i] != 'M') nls = NULL;

          if (nls) { 
            unsigned int x = 0;
            double nl = 0.0;
            while ((nl = nls[x++]) > 0.00001) {
              // hack - if neutral loss is too heavy - over 250 Da, this is really not a "neutral loss"
              // but rather a charge-carrying loss (e.g. the old ICAT losses), so we will not add
              // this fragment if the fragment charge == precursor charge. 
              if (nl > 250.0 && ch == charge) continue;

              losses[(int)(nl + 0.5)] = nl;
            }
          }
        }
      }
    
      // b ion
      unsigned int position = (unsigned int)i + 1;
      unsigned int prom = 8;
      if (ch == charge && (double)position > (double)(stripped.length()) * 0.77) prom = 5;
      if (hasBasicAA) prom++;
      ions.push_back(new FragmentIon("b", position, 0, sum / (double)ch, ch, prom));
        
      for (map<int, double>::iterator n = losses.begin(); n != losses.end(); n++) {
	prom = 4;
	if (n->first == 17 || n->first == 18 || n->first == 64 || n->first == 91 || n->first == 98) {
	  if (ch == charge) {
	    prom = 5;
	  } else {
	    prom = 6;
	  }
	}
	
        ions.push_back(new FragmentIon("b", position, n->first, (sum - n->second) / (double)ch, ch, prom));
      }

      // special case, b(n-1) ion can have +18 neutral "gain"
      if (position == NAA() - 1) {
	ions.push_back(new FragmentIon("b", position, -18, (sum + (*AAMonoisotopicMassTable)['!']) / (double)ch, ch, ch == charge ? 5 : 6));
      }

      // a ion
      // small a ions are more common
      ions.push_back(new FragmentIon("a", position, 0, (sum - (*AAMonoisotopicMassTable)['$'] - (*AAMonoisotopicMassTable)['o']) / (double)ch, ch, position <= 3 && ch == 1 ? 7 : 4));


  
           
    }
      
    // END b and a ions
  
  } // for all charges <= pep.charge

  // check "special" modifications, namely phospho and old ICAT
  int numP = 0;
  bool isOldICATLight = false;
  bool isOldICATHeavy = false;
  
  if (isModsSet) {
    for (map<int, string>::iterator m = mods.begin(); m != mods.end(); m++) {
      if (m->second == "ICAT-D" || m->second == "AB_old_ICAT_d0") {
        isOldICATLight = true;
      } else if (m->second == "ICAT-D:2H(8)" || m->second == "AB_old_ICAT_d8") {
        isOldICATHeavy = true;
      } 
      if (m->second == "Phospho") {
        numP++;
      }
    }
  }
  
  double waterMass = (*AAMonoisotopicMassTable)['!'];
  double phosphoMass = (*modMonoisotopicMassTable)["Phospho"];
  
  // 2 H2O loss from p-98 for phosphorylation
  if (numP > 0) {
    ions.push_back(new FragmentIon("p", 0, 134, (precursorMH - phosphoMass - 3 * waterMass) / (double)charge, charge, 7));
  }
  
  // multiple phosphorylations
  if (numP > 1) {
    ions.push_back(new FragmentIon("p", 0, 160, (precursorMH - 2 * phosphoMass) / (double)charge, charge, 7));
    ions.push_back(new FragmentIon("p", 0, 178, (precursorMH - 2 * phosphoMass - waterMass) / (double)charge, charge, 7));
    ions.push_back(new FragmentIon("p", 0, 196, (precursorMH - 2 * phosphoMass - 2 * waterMass) / (double)charge, charge, 9));
    ions.push_back(new FragmentIon("p", 0, 214, (precursorMH - 2 * phosphoMass - 3 * waterMass) / (double)charge, charge, 7));
  }
  
  // uncleavable ICAT
  if (isOldICATLight) {
    // these are fragment ions of the ICAT-tag
    ions.push_back(new FragmentIon("IC546A", 0, 0, 284.2, 1, 9));
    ions.push_back(new FragmentIon("IC546B", 0, 0, 403.2, 1, 9));
    ions.push_back(new FragmentIon("IC546C", 0, 0, 477.2, 1, 9));
    
    // the +1-charge-carrying loss from the precursors
    if (charge == 2) {
      ions.push_back(new FragmentIon("p", 0, 284, (precursorMH - 284.2), 1, 9));
      ions.push_back(new FragmentIon("p", 0, 403, (precursorMH - 403.2), 1, 9));  
    }
    if (charge == 3) {
      ions.push_back(new FragmentIon("p", 0, 284, (precursorMH - 284.2) / 2.0, 2, 9));
      ions.push_back(new FragmentIon("p", 0, 403, (precursorMH - 403.2) / 2.0, 2, 9));
    }
  }
  
  if (isOldICATHeavy) {
    // these are fragment ions of the ICAT-tag
    ions.push_back(new FragmentIon("IC554A", 0, 0, 288.2, 1, 9));
    ions.push_back(new FragmentIon("IC554B", 0, 0, 411.2, 1, 9));
    ions.push_back(new FragmentIon("IC554C", 0, 0, 485.2, 1, 9));

    // the +1-charge-carrying loss from the precursor
    if (charge == 2) {
      ions.push_back(new FragmentIon("p", 0, 288, (precursorMH - 288.2), 1, 9));
      ions.push_back(new FragmentIon("p", 0, 411, (precursorMH - 411.2), 1, 9));
    }
    if (charge == 3) {
      ions.push_back(new FragmentIon("p", 0, 288, (precursorMH - 288.2) / 2.0, 2, 9));
      ions.push_back(new FragmentIon("p", 0, 411, (precursorMH - 411.2) / 2.0, 2, 9));
    }
  }
  
  // add immonium ions
  map<string, unsigned int> tokenCounts;
  countAATokens(tokenCounts);   
  for (map<string, unsigned int>::iterator to = tokenCounts.begin(); to != tokenCounts.end(); to++) {
   
    map<string, double*>::iterator foundImmoniums = AAMonoisotopicImmoniumTable->find(to->first);
    if (foundImmoniums == AAMonoisotopicImmoniumTable->end() || foundImmoniums->second == NULL) {
      // no such token in the table, or no listed immonium ions for that token
      continue;
    }
    stringstream imss;
    imss << 'I';
    for (string::size_type topos = 0; topos < to->first.length(); topos++) {
      char toc = to->first[topos];
      if (toc == '[' || toc == ']') {      
        imss << '_';
      } else {
        imss << toc;
      }
    }

 
    unsigned int imIndex = 0; 
    double imMass = 0.0; // assume +1
   
    while ((imMass = (foundImmoniums->second)[imIndex++]) > 0.00001) {
    
      char imSuffix = 'A' + imIndex - 1;
      string im = imss.str() + imSuffix;  
    
      ions.push_back(new FragmentIon(im, 0, 0, imMass, 1, 7));
    }
      
  }
  
  sort(ions.begin(), ions.end(), FragmentIon::sortFragmentIonPtrsByProminence);
  
}

void Peptide::generateFragmentIonsETD(vector<FragmentIon*>& ions) {

  double precursorMH = 0.0;
  
  for (unsigned int ch = 1; ch <= (unsigned int)charge; ch++) { 

    map<int, double> losses; // store losses as pairs of (int value of mod mass, double value of mod mass)
    double sum = 0.0;
    
    // BEGIN y/z ions and precursor
    
    // add a water for the y ion
    sum += (*AAMonoisotopicMassTable)['!'];

    // add C-terminal modifcation mass, if any
    if (isModsSet && !cTermMod.empty()) {
      sum += getModMonoisotopicMass(cTermMod);
    }
    
    // losses[18] = (*AAMonoisotopicMassTable)['!'];
    // losses[44] = 43.98982; // CO2
    // losses[46] = 46.00548; // HCOOH
    
    // add a proton for each charge
    sum += (double)ch * (*AAMonoisotopicMassTable)['+'];
    
    for (int i = (int)(stripped.length()) - 1; i >= 0; i--) { 
     
      sum += (*AAMonoisotopicMassTable)[stripped[i]];
        
      double* nls = (*AAMonoisotopicNeutralLossTable)[stripped[i]];
      if (nls) {
        unsigned int x = 0;
        double nl = 0.0;
	while((nl = nls[x++]) > 0.00001) {
          int intLoss = (int)(nl + 0.5);

	  // allow double H2O/NH3 losses
	  if (intLoss == 17 || intLoss == 18) {
	    if (losses.find(18) != losses.end()) {
	      losses[intLoss + 18] = nl + losses[18];
	    } else if (losses.find(17) != losses.end()) {
	      losses[intLoss + 17] = nl + losses[17];
	    }
	  }

	  losses[intLoss] = nl;
        }
  
      }
        

      if (isModsSet && !mods.empty()) {
        map<int, string>::iterator j = mods.find(i);
        if (j != mods.end()) {
          // modified at this position
          sum += getModMonoisotopicMass(j->second);

          double* nls = (*modMonoisotopicNeutralLossTable)[j->second];

	  // hack - loss of 64 only applies to methionine oxidation, not to other oxidations, so check:
	  if (j->second == "Oxidation" && stripped[i] != 'M') nls = NULL;

          if (nls) { 
            unsigned int x = 0;
            double nl = 0.0;
            while ((nl = nls[x++]) > 0.00001) {
              // hack - if neutral loss is too heavy - over 250 Da, this is really not a "neutral loss"
              // but rather a charge-carrying loss (e.g. the old ICAT losses), so we will not add
              // this fragment if the fragment charge == precursor charge. 
              if (nl > 250.0 && ch == charge) continue;

              losses[(int)(nl + 0.5)] = nl;
            }
          }
        }
      }
      
      if (i > 0) {
	if (ch <= (unsigned int) (charge - 1)) {

	  // this is a y/z ion
	  unsigned int position = NAA() - (unsigned int)i;
	  ions.push_back(new FragmentIon("y", position, 0, sum / (double)ch, ch, 6));
	  
	  // subtract an ammonia to get the z, add a proton to get zdot
	  // NOTE: In annotations, "z" actually means zdot!!
	  double zsum = sum - (*AAMonoisotopicMassTable)['a'] + (*AAMonoisotopicMassTable)['+'];
	  ions.push_back(new FragmentIon("z", position, 0, zsum / (double)ch, ch, 8));
	}	  
          
      } else {
        // this is really just the precursor!
        
        // add N-terminus mod, if any
        if (isModsSet && !nTermMod.empty()) {
          sum += getModMonoisotopicMass(nTermMod);
        }
       
        
        // in ETD, there are charge-reduced precursors, depending on how many e- it absorbs, but
	// they retain the precursor's protons
        precursorMH = monoisotopicMH();
	ions.push_back(new FragmentIon("p", 0, 0, precursorMH / (double)ch, ch, 9));
        
	for (map<int, double>::iterator n = losses.begin(); n != losses.end(); n++) {
	  ions.push_back(new FragmentIon("p", 0, n->first, (precursorMH - n->second) / (double)ch, ch, 9));
	}    
        
      }
        
    }
    // END y ions and precursor
    
    // reset
    sum = 0.0;
    // losses.clear();
    
    // BEGIN b and a ions
    
    // add the N-term modification mass, if any
    if (isModsSet && !nTermMod.empty()) {
      sum += getModMonoisotopicMass(nTermMod);
    }

    // losses[17] = 17.026549; // loss of NH3

    // add a proton for each charge
    sum += (double)ch * (*AAMonoisotopicMassTable)['+'];
    
    for (int i = 0; i < (int)(stripped.length()) - 1; i++) {
      sum += (*AAMonoisotopicMassTable)[stripped[i]];
      
      /*
      double* nls = (*AAMonoisotopicNeutralLossTable)[stripped[i]];

      if (nls) {
        unsigned int x = 0;
        double nl = 0.0;
        while ((nl = nls[x++]) > 0.00001) {
	  int intLoss = (int)(nl + 0.5);
	  
	  // allow double H2O/NH3 losses
	  if (intLoss == 17 || intLoss == 18) {
	    if (losses.find(18) != losses.end()) {
	      losses[intLoss + 18] = nl + losses[18];
	    } else if (losses.find(17) != losses.end()) {
	      losses[intLoss + 17] = nl + losses[17];
	    }
	  }
          losses[intLoss] = nl;
        }
      }
      */
      if (isModsSet && !mods.empty()) {
        map<int, string>::iterator j = mods.find(i);
        if (j != mods.end()) {
          // modified at this position
          sum += getModMonoisotopicMass(j->second);
       
	  /*
	  double* nls = (*modMonoisotopicNeutralLossTable)[j->second];

	  // hack - loss of 64 only applies to methionine oxidation, not to other oxidations, so check:
	  if (j->second == "Oxidation" && stripped[i] != 'M') nls = NULL;

          if (nls) { 
            unsigned int x = 0;
            double nl = 0.0;
            while ((nl = nls[x++]) > 0.00001) {
              // hack - if neutral loss is too heavy - over 250 Da, this is really not a "neutral loss"
              // but rather a charge-carrying loss (e.g. the old ICAT losses), so we will not add
              // this fragment if the fragment charge == precursor charge. 
              if (nl > 250.0 && ch == charge) continue;

              losses[(int)(nl + 0.5)] = nl;
            }
          }
	  */
        }
      }
    
      // b ion
      unsigned int position = (unsigned int)i + 1;
      ions.push_back(new FragmentIon("b", position, 0, sum / (double)ch, ch, 5));

      // add an ammonium to get the c ion
      double csum = sum + (*AAMonoisotopicMassTable)['a'];
      ions.push_back(new FragmentIon("c", position, 0, csum / (double)ch, ch, 8));

   
    }
      
    // END b and c ions
  
  } // for all charges <= pep.charge - 1
  
  sort(ions.begin(), ions.end(), FragmentIon::sortFragmentIonPtrsByProminence);

}



// =============================================================================
// PRIVATE PARSING METHODS
	
// stripPeptide - parses out a peptide string
bool Peptide::stripPeptide(string pep) {

  // make a copy first
  string origPep(pep); 
  
  // take off the /<charge> for msp format (if present)
  string::size_type slashPos = pep.rfind('/');
  if (slashPos != string::npos) {
    if (slashPos != pep.length() - 1) {
      int ch = atoi((pep.substr(slashPos + 1)).c_str());
      if (charge == 0 && ch > 0) {
        charge = ch;
      }
    } 
    pep = pep.substr(0, slashPos);
  }	
  
  // take off the prev and next AA (if present)
  string::size_type firstDotPos = pep.find('.');
  string::size_type lastDotPos = pep.rfind('.');
  if (pep.length() > 4 && firstDotPos == 1 && lastDotPos == pep.length() - 2) { 
    // looks like a full peptide in the form "P.XXXXX.N" where P and N are the prev and next AA
    if (prevAA == 'X') {
      // only set if it is not already given
      prevAA = pep[0];
    } 
    if (nextAA == 'X') { 
      // only set if it is not already given
      nextAA = pep[pep.length() - 1];
    }
    pep = pep.substr(2, pep.length() - 4);	
  }
  
  if (pep.empty()) {
    // nothing left! illegal peptide string
    // cerr << "No amino acid in this peptide: " << origPep << endl;
    return (false);
  }
  
  // strip off the modifications
  stripped = "";
  string::size_type pos = 0;
  bool success = true;
  
  while (pos != string::npos) {
    string token = nextAAToken(pep, pos, pos);
    
    // add AA to stripped peptide, unless the token refers to the N- or C-termini (e.g., n[47])
    if (token[0] != 'n' && token[0] != 'c') {
      stripped += token[0];
    }
    
    // now we set the modifications based on the tokens parsed
    if (!isModsSet) {
      if (token[0] == 'n') {
        if (!stripped.empty()) {
          // wrong. n (N-term mod) must occur before first AA.
          // cerr << "N-terminus token occurs after first AA: " << origPep << endl;
          success = false;
        } else {
          success = setModByToken(token, 0, 'n'); 
        }
      } else if (token[0] == 'c') {
        if (pos != string::npos) {
          // wrong. c (C-term mod) must occur after last AA.
          // cerr << "C-terminus token occurs before last AA: " << origPep << endl;
          success = false;
        } else {
          success = setModByToken(token, 0, 'c'); 
        }
      } else {
        // normal mod
   
	success = setModByToken(token, (unsigned int)stripped.length() - 1);
      }
    }		
    
    if (!success) {
      return (false);
    }
  }
  isModsSet = true;
  return (true);
}

// parseMspModStr - parses an .msp style mod string. that is "0" for no mod, or <num mods>/pos,AA,modType/pos,AA,modType)...
// If "add" is set to true, then the mods in mspModStr will be added to the existing mods, instead of replacing. 
bool Peptide::parseMspModStr(string mspModStr, bool add) {
	
  if (!add) mods.clear();

  if (mspModStr.empty()) { 
    // nothing supplied, just returns and leaves the mods in the unset state
    return (true);
  }
  if (mspModStr == "0") {
    // No mod specified, returns and specifies that the mods are already set
    isModsSet = true;
    return (true);
  }
  string::size_type slashPos = mspModStr.find('/', 0);
  if (slashPos == string::npos) {
    // illegal msp-style mod string
    return (false);
  }
  
  // non-zero number of mods, now get the number of mods
  unsigned int numMods = atoi((mspModStr.substr(0, slashPos)).c_str());
  
  // loops over all the mods
  string m;
  while ((m = nextToken(mspModStr, slashPos + 1, slashPos, "/ \t\r\n")) != "") {
    string::size_type commaPos1 = m.find(',', 0);
    if (commaPos1 == string::npos) {
      return (false);
    }
    string::size_type commaPos2 = m.find(',', commaPos1 + 1);
    if (commaPos2 == string::npos) {
      return (false);
    }
    
    // parses out the position, modified AA, and type of the mod
    int modPos = atoi((m.substr(0, commaPos1)).c_str());
    string modAA = m.substr(commaPos1 + 1, commaPos2 - commaPos1 - 1);
    string modType = m.substr(commaPos2 + 1);
    
    // N-term and C-term modifications can be specified by having pos of -1 and -2 respectively.
    // Unfortunately and uglily, NIST's msp modifcation strings don't make distinction between n-term mod
    // and mod on the first AA. So only way to distinguish is by the mod type. For 
    // instance, Acetyl is assumed to be on n-term if it occurs at the first AA.	
    bool success = true;
    if (modPos == Peptide::NTermPos || (modPos == 0 && isNTermModType(modType) && modType != "iTRAQ4plex")) { 
      success = setModByType(modType, 0, 'n');
      
    // can't quite check for C-terminal mods because by this time we still don't know if we're the last AA. We'll have
    // to assume no NIST-style modifications are C-terminal mods
    } else if (modPos == Peptide::CTermPos) {
      success = setModByType(modType, 0, 'c');
    } else {
      success = setModByType(modType, modPos);
    }
    if (!success) {
      return (false);
    }
  }
  if (numMods != (mods.size() + (nTermMod.empty() ? 0 : 1) + (cTermMod.empty() ? 0 : 1))) {
    // the num mod specified doesn't match the number of /../ in the mod string!
    return (false);
  }
  isModsSet = true;
  return (true);
	
}	

// nextAAToken - simply extracts the next AA token, starting to look from position 'start' in string s. 
// An AA token starts at an AA and ends before the next AA. That is, C*, CJ, C[339], M(O), KJJJJJJ are 
// all AA tokens. When function returns, 'end' will point to the next AA or string::npos if there's nothing left
string Peptide::nextAAToken(string s, string::size_type start, string::size_type& end) {

  string allAA(ALL_AA);
  allAA += "nc"; // add the n- and c-termini
  
  if (allAA.find(s[start], 0) == string::npos) {
    start = s.find_first_of(allAA, start);
    
    if (start == string::npos) {
      // no more AA
      end = string::npos;
      return ("");
    }
  } 
  
  
  end = s.find_first_of(allAA + "[", start + 1);
  
  if(end == string::npos) {
    
    return (s.substr(start));

  } else if (s[end] == '[') {

    string::size_type closeBracket = s.find(']', end + 1);
    if (closeBracket == string::npos) {
      // [ not closed, just stop at first AA after the [
      end = s.find_first_of(allAA, end + 1); 
    } else {
      end = closeBracket + 1;
    }
  }
  
  if (end == string::npos || end >= s.length()) {
    end = string::npos;
    return (s.substr(start));
  }

  return (s.substr(start, end - start));

}


// shufflePeptideSequence - randomly shuffles the peptide sequence (returns the shuffled sequence).
// multiple calls to this method will return different results.
Peptide* Peptide::shufflePeptideSequence(map<int, set<string> >& allSequences) {
  
  // this randomizer tries to keep the N-terminal K/R, P, KP and RP and modified amino acids where they are
  // if not enough movable AAs exist then it will start loosening, first allowing P and modified amino acids to move
  // then allowing KP and RP to move as well
    
  Peptide* newPep = new Peptide(*this);
  int pos = 0;
  int numAA = (int)(NAA());  

  vector<bool> immovable(numAA, false);
  list<char> movableAA;
  
  for (pos = 0; pos < numAA - 1; pos++) { // don't move last AA
      
    if (stripped[pos] == 'P' || mods.find(pos) != mods.end()) {
      immovable[pos] = true;
    } else {
      movableAA.push_back(stripped[pos]);      
    }
  }
        
  bool okay = false;      
  unsigned int attempt = 0;
    
  do {
    
    (*newPep) = (*this);
    list<char> tmpMovableAA = movableAA;
    
    for (pos = 0; pos < numAA - 1; pos++) {
      
      if (immovable[pos]) {
	continue;
      }
      
      unsigned int randIndex = (unsigned int)((double)rand() / (double)RAND_MAX * (double)(tmpMovableAA.size()));
      if (randIndex >= tmpMovableAA.size()) randIndex--; // out-of-bound safeguard: apparently on some platform this could happen
      list<char>::iterator j = tmpMovableAA.begin();
      for (unsigned int i = 0; i < randIndex; i++) j++; // step forward movable AA indexed by randIndex
      newPep->stripped[pos] = (*j);
      tmpMovableAA.erase(j);
    
    }
    
    attempt++;
    
    // check if first movable AA is the same as before
    bool sameFirst = true;
    for (pos = 0; pos < numAA - 1; pos++) {
      if (!immovable[pos]) {
	if (!Peptide::isIsobaric((*newPep)[pos], (*this)[pos])) {
	  sameFirst = false;
	} 
	break;
      }
    }
    
    bool sameLast = true;
    for (pos = numAA - 2; pos >= 0; pos--) {
      if (!immovable[pos]) {
	if (!Peptide::isIsobaric((*newPep)[pos], (*this)[pos])) {
	  sameLast = false;
	} 
	break;
      }
    }
    
    
    int dummy = 0;
    okay = (!sameFirst) && (!sameLast) && (!(isHomolog(*newPep, 0.6, dummy)));
    
    if (okay && allSequences[newPep->NAA()].find(newPep->stripped) != allSequences[newPep->NAA()].end()) {
      okay = false;
    }
     
  } while (attempt < 8 && !okay);
    
  if (!okay) {
    // still can't shuffle to a new sequence? tag on two random non-K/R AA plus the last K/R 
    // (hopefully less likely to become something real)
    string aas("ADEFGHILMNPQSTVWY");
    
    Peptide* newNewPep = NULL;
    do {
      unsigned int randIndex = (unsigned int)((double)rand() / (double)RAND_MAX * (double)(aas.length()));
      if (randIndex >= aas.length()) randIndex--; // out-of-bound safeguard: apparently on some platform this could happen

      string newPepStr = newPep->interactStyle();
      string newPepLastAAStr = (*newPep)[newPep->NAA() - 1];
      newPepStr += aas[randIndex];
      
      randIndex = (unsigned int)((double)rand() / (double)RAND_MAX * (double)(aas.length()));
      if (randIndex >= aas.length()) randIndex--; // out-of-bound safeguard: apparently on some platform this could happen
      
      newPepStr += aas[randIndex];
      newPepStr += newPepLastAAStr;
    
      okay = true;
      newNewPep = new Peptide(newPepStr, charge);
      if (allSequences[newNewPep->NAA()].find(newNewPep->stripped) != allSequences[newNewPep->NAA()].end()) {
	okay = false;
	delete (newNewPep);
      }
      
    } while (!okay);
      
    delete (newPep);
    newPep = newNewPep;
  }
  
  return (newPep);
}

// computePI - computes the theoretical isoelectric point of the peptide.
// This current version can't handle modified amino acids (usually not important charge carriers anyway).
double Peptide::computePI() {
  
  map<string, double> K;
  K["C"] = 1.000e-9;
  K["D"] = 8.913e-5;
  K["E"] = 3.548e-5;
  K["H"] = 1.047e-6;
  K["K"] = 1.000e-10;
  K["R"] = 1.000e-12;
  K["Y"] = 1.000e-10;
  K["X"] = 0.0; // default
  
  map<string, double> K_nTerm;
  K_nTerm["A"] = 2.570e-8;
  K_nTerm["E"] = 1.995e-8;
  K_nTerm["M"] = 1.000e-7;
  K_nTerm["P"] = 4.365e-9;
  K_nTerm["S"] = 1.175e-7;
  K_nTerm["T"] = 1.514e-7;
  K_nTerm["V"] = 3.631e-8;
  K_nTerm["X"] = 3.162e-8; // default
  
  map<string, double> K_cTerm;
  K_cTerm["D"] = 2.818e-5;
  K_cTerm["E"] = 1.778e-5;
  K_cTerm["X"] = 2.818e-4; // default
  
  map<string, unsigned int> composition;
  composition["C"] = 0;
  composition["D"] = 0;
  composition["E"] = 0;
  composition["H"] = 0;
  composition["K"] = 0;
  composition["R"] = 0;
  composition["Y"] = 0;

  string nTerm("");
  string cTerm("");
  
  for (unsigned int j = 0; j < NAA(); j++) {
    
    string aa = interactStyleAA(j);
    if (nTerm.empty()) nTerm = aa;
    cTerm = aa;
    
    map<string, unsigned int>::iterator found = composition.find(aa);
    if (found != composition.end()) {
      found->second++;
    } else {
      composition[aa] = 1;
    }
  }
  
  double pHMin = 0;
  double pHMid = 7;
  double pHMax = 14;
  double charge = 1.0;
  
  for (unsigned int i = 0; i < 200 && (pHMax - pHMin) > 0.01; i++) {
    
    pHMid = pHMin + (pHMax - pHMin) / 2.0;
    
    double HMid = pow(10.0, -pHMid);
    
    double Kc = (K_cTerm.find(cTerm) != K_cTerm.end() ? K_cTerm[cTerm] : K_cTerm["X"]);
    double cter = Kc / (Kc + HMid);

    double Kn = (K_nTerm.find(nTerm) != K_nTerm.end() ? K_nTerm[nTerm] : K_nTerm["X"]);
    double nter = HMid / (Kn + HMid);
    
    double carg = composition["R"] * HMid / (K["R"] + HMid);
    double chis = composition["H"] * HMid / (K["H"] + HMid);
    double clys = composition["K"] * HMid / (K["K"] + HMid);
    
    double casp = composition["D"] * K["D"] / (K["D"] + HMid);
    double cglu = composition["E"] * K["E"] / (K["E"] + HMid);
    double ccys = composition["C"] * K["C"] / (K["C"] + HMid);
      
    double ctyr = composition["Y"] * K["Y"] / (K["Y"] + HMid);
    
    charge = carg + clys + chis + nter - (casp + cglu + ctyr + ccys + cter);
    
    if (charge > 0.0) {
      pHMin = pHMid;
    } else {
      pHMax = pHMid;
    }
  }
  
  return(pHMid);
}

string Peptide::removeModOfType(string mod) {
  
  Peptide newPep(*this);
  
  if (newPep.isModsSet) {
    for (map<int, string>::iterator j = newPep.mods.begin(); j != newPep.mods.end(); j++) {
      if (j->second == mod) {
         newPep.mods.erase(j);
      }
    }
  }
  
  return (newPep.interactStyleWithCharge());
}

double Peptide::getAATokenMonoisotopicMass(string aa) {
  
  if (aa.length() > 1) {
    // modified
    map<string, string>::iterator found = modTokenTable->find(aa);
    if (found != modTokenTable->end()) {
      return (getAAPlusModMonoisotopicMass(aa[0], found->second));
    } else {
      // no such mod token
      return (0.0);
    }
    
  } else {
    return (getAAMonoisotopicMass(aa[0]));
     
  }
}

double Peptide::getAATokenAverageMass(string aa) {
  
  if (aa.length() > 1) {
    // modified
    map<string, string>::iterator found = modTokenTable->find(aa);
    if (found != modTokenTable->end()) {
      return (getAAPlusModAverageMass(aa[0], found->second));
    } else {
      // no such mod token
      return (0.0);
    }
    
  } else {
    return (getAAAverageMass(aa[0]));
     
  }
}

bool Peptide::isIsobaric(string aa1, string aa2) {
  
  return (fabs(getAATokenMonoisotopicMass(aa1) - getAATokenMonoisotopicMass(aa2)) < 0.5);
    
}

double Peptide::calcApproximateAverageMass(double monoisotopicMass) {
  
 // approximate way to convert monoisotopic mass to average mass, when the latter is absent
 // it's ok since the average mass is only used for average-mass searches, which are applicable for low mass-accuracy instrument anyway
 
 // note that this works reasonably well when there is no sulfur or phosphorous (or some other weird atoms)
 double shift = monoisotopicMass * 7.7E-4 - 9.5E-3;
 return (monoisotopicMass + shift);
  
}


bool Peptide::processNewMod(char aa, double& deltaMass, string& modType, string& userToken) {

  // create standard token
  stringstream sTokenss;
  sTokenss << aa << '[' << (int)(deltaMass + (*AAMonoisotopicMassTable)[aa] + 0.5) << ']';
  string sToken(sTokenss.str());
  
  double aveDeltaMass = calcApproximateAverageMass(deltaMass);

  map<string, string>::iterator foundToken = modTokenTable->find(sToken);
  if (foundToken != modTokenTable->end()) {
    // this token is already in the table
    if (modType.empty() || modType == foundToken->second) {
      // modType not specified, or the specified modToken is the existing one mapped in the table 
      if (userToken.empty()) {
	// use sToken, no need to do anything
	deltaMass = (*modMonoisotopicMassTable)[foundToken->second]; // fixed mass
	modType = foundToken->second;
	userToken = sToken;
	return (true);
      } else {
	// make sure userToken does not collide with existing tokens
	map<string, string>::iterator foundUserToken = modTokenTable->find(userToken);
	if (foundUserToken != modTokenTable->end() && foundUserToken->second != foundToken->second) {
	  // collision!
	  return (false);
	} else {
	  // add this new user token, points to same modType
	  (*modTokenTable)[userToken] = foundToken->second;
	  deltaMass = (*modMonoisotopicMassTable)[foundToken->second];
	  modType = foundToken->second;
	  return (true);
	}
      }

    } else if (modMonoisotopicMassTable->find(modType) == modMonoisotopicMassTable->end()) {
      // user specify a new mod that has the same modified mass on the same amino acid as an existing mod
      // in this case, we need a new mod token and a new mod type. 
      // however, note that this new mod will never be used in pepXML import, since in pepXML file only the mass
      // is given. SpectraST will always use the original mod for this modified mass on this amino acid.
      if (userToken.empty()) {
	stringstream userTokenss;
	userTokenss << aa << '[' << modType << ']';
	userToken = userTokenss.str();
	(*modTokenTable)[userToken] = modType;
	(*modMonoisotopicMassTable)[modType] = deltaMass;
	(*modAverageMassTable)[modType] = aveDeltaMass;
	return (true);
      } else {
	map<string, string>::iterator foundUserToken = modTokenTable->find(userToken);
	if (foundUserToken != modTokenTable->end()) {
	  return (false);
	} else {
	  (*modTokenTable)[userToken] = modType;
	  (*modMonoisotopicMassTable)[modType] = deltaMass;
	  (*modAverageMassTable)[modType] = aveDeltaMass;
	  return (true);
	}
      }
      
    } else {
      return (false);
    }
      
  } else {
    // this token is not in the table
    string newModType("");

    map<string, double>::iterator foundModType;
    if (!modType.empty() && ((foundModType = modMonoisotopicMassTable->find(modType)) != modMonoisotopicMassTable->end())) {
      // modType is already present, check mass
      if (fabs(deltaMass - foundModType->second) <= 0.5) {
	// close enough, use same modType
	if (userToken.empty()) {
	  (*modTokenTable)[sToken] = modType;
	  deltaMass = (*modMonoisotopicMassTable)[modType];
	  userToken = sToken;
	  return (true);
	} else {
	  // make sure userToken does not collide with existing tokens
	  map<string, string>::iterator foundUserToken = modTokenTable->find(userToken);
	  if (foundUserToken != modTokenTable->end() && foundUserToken->second != modType) {
	    // collision!
	    return (false);
	  } else {
	    // add this new user token, points to same modType
	    (*modTokenTable)[userToken] = modType;
	    deltaMass = (*modMonoisotopicMassTable)[modType]; // fixed mass
	    return (true);
	  }
	  
	}
      } else {
	// specify a colliding modType but mass is different!
	// in this case we will construct a new modType string to replace it -- below.
      }
    } else {
      newModType = modType;
    }

    if (newModType.empty()) {
      // either the user didn't specify a modType, or the specified modType collides with an existing one
      // need new modType, create as follows: USM_<aa>_<MonoisotopicMass>
      stringstream newModTypess;
      newModTypess.precision(6);
      newModTypess << "USM_" << aa << "_" << fixed << (*AAMonoisotopicMassTable)[aa] + deltaMass;
      newModType = newModTypess.str();
    }
    
    (*modMonoisotopicMassTable)[newModType] = deltaMass;
    (*modAverageMassTable)[newModType] = aveDeltaMass;
    
    if (userToken.empty()) {
      (*modTokenTable)[sToken] = newModType;
      
      // checking
      // cerr << "Added " << sToken << "|" << newModType << ", " << (modTokenTable->find(sToken) != modTokenTable->end() ? "YES" : "NO") << endl;
      
      modType = newModType;
      userToken = sToken;
      return (true);
    } else {
      // make sure userToken does not collide with existing tokens
      map<string, string>::iterator foundUserToken = modTokenTable->find(userToken);
      if (foundUserToken != modTokenTable->end()) {
	// collision!
	return (false);
      } else {
	// add this new user token, points to same modType
	(*modTokenTable)[userToken] = newModType;
	modType = newModType;
	return (true);
      }

    }

  }

  return (false);

}

void Peptide::permuteModTokens(vector<map<char, set<string> > >& allowableTokenSets, vector<pair<string, int> >& permutations) {
  
  permutations.clear();
  
  for (vector<map<char, set<string> > >::iterator se = allowableTokenSets.begin(); se != allowableTokenSets.end(); se++) {
  
    vector<pair<string, int> > perm;
  
    map<char, set<string> >& allowableTokens = *se;
    
    // n term
    if (allowableTokens['n'].empty()) {
      // keep original
      pair<string, int> p;
      p.first = interactStyleAA(0, 'n');
      p.second = 0;    // zero substitution
    
      perm.push_back(p);
      
    } else {
      // user-specified something for 'n'
      for (set<string>::iterator i = allowableTokens['n'].begin(); i != allowableTokens['n'].end(); i++) {
      
        pair<string, int> p;
        if (*i == "n") {  
	  p.first = "";
        } else {
	  p.first = (*i);
	}
	if ((*i == "n" && nTermMod.empty()) || (*i == interactStyleAA(0, 'n'))) {
	  p.second = 0;
	} else {
	  p.second = 1;
	}
      
	perm.push_back(p);

      }
    }
  
    for (string::size_type pos = 0; pos < stripped.length(); pos++) {
    
      char aa = stripped[pos];
      string orig = interactStyleAA((unsigned int)pos);    
    
      if (allowableTokens[aa].empty()) {
	// keep original
	for (vector<pair<string, int> >::iterator j = perm.begin(); j != perm.end(); j++) {
	  j->first += orig;
	}
      
      } else {
      // one or more choices 
      
	vector<pair<string, int> > newPerm;
	for (vector<pair<string, int> >::iterator j = perm.begin(); j != perm.end(); j++) {

	  // create new permutations for extra choices
	  for (set<string>::iterator i = allowableTokens[aa].begin(); i != allowableTokens[aa].end(); i++) {
	    pair<string, int> p;
	    p.first = j->first + (*i);
	    p.second = j->second + (*i != orig ? 1 : 0);
	    if (p.second <= 3) {
	      newPerm.push_back(p);
	    }
	  }
      
	}
      
	perm.clear();
	perm.insert(perm.end(), newPerm.begin(), newPerm.end());
      
      }
  
    }
  
    // c term
    if (allowableTokens['c'].empty()) {
      // keep original
      if (!cTermMod.empty()) {
	for (vector<pair<string, int> >::iterator j = perm.begin(); j != perm.end(); j++) {
	  j->first += interactStyleAA(0, 'c');
	}
      }
	
    } else {
	      
      vector<pair<string, int> > newPerm;
      
      string orig = interactStyleAA(0, 'c');
      if (orig.empty()) orig = "c";
      
      for (vector<pair<string, int> >::iterator j = perm.begin(); j != perm.end(); j++) {
  
	  // create new permutations for extra choices
	for (set<string>::iterator i = allowableTokens['c'].begin(); i != allowableTokens['c'].end(); i++) {
	  pair<string, int> p;
	  if (*i != "c") p.first = j->first + (*i);
	  p.second = j->second + (*i != orig ? 1 : 0);
	  if (p.second <= 3) {
	    newPerm.push_back(p);
	  }
	}
	
      }
	
      perm.clear();
      perm.insert(perm.end(), newPerm.begin(), newPerm.end());
	
    }
    
    // tag on charge
    stringstream chss;
    chss << "/" << charge;
    for (vector<pair<string, int> >::iterator j = perm.begin(); j != perm.end(); j++) {
      j->first += chss.str();
    }
  
    permutations.insert(permutations.end(), perm.begin(), perm.end());
  }
    
}

/* Parser-constructor: Not used anywhere at present and not tested, so commented out for now.
FragmentIon::FragmentIon(double mz, string& annotation, unsigned int curIsotope) :
  m_ion(""),
  m_mz(mz),
  m_charge(0),
  m_prominence(0),
  m_mzDiff(0.0),
  m_isotope(0),
  m_bracket(false) {
  
  if (annotation.empty() || annotation[0] == '?') {
    return;
  }
  
  string::size_type pos = 0;
  
  if (annotation[0] == '[') {
    m_bracket = true;
  }
  
  m_ion = nextToken(annotation, 0, pos, "/], \t\r\n", "[");
  m_mzDiff = atof(nextToken(annotation, pos, pos, ", \t\r\n").c_str());
  
  if (m_ion[0] == 'I') {
    m_charge = 1; // only charge 1?
    m_isotope = 0; // no higher isotope?
    return;
  }
  
  char ionType = m_ion[0];
  
  unsigned int numAA = atoi(nextToken(m_ion, 1, pos, "^i-+/]*, \t\r\n", " \t\r\n").c_str());
  int neutralLoss = 0;
  m_charge = 1;
    
  if (m_ion.find('i') == string::npos) {
    m_isotope = 0;
  } else {
    m_isotope = curIsotope + 1;
  }
    
  if (pos < m_ion.length() && (m_ion[pos] == '-' || m_ion[pos] == '+')) {
    // neutral loss
    neutralLoss = atoi(nextToken(m_ion, pos + 1, pos, "^i/]*, \t\r\n", " \t\r\n").c_str());
      
    if (m_ion[pos] == '+') neutralLoss *= -1; 
  }
    
  bool isNISTisotope = false;
  if (pos < m_ion.length() && (m_ion[pos] == 'i' || m_ion[pos] == '*')) { // NIST format, i before charge
    pos++;
  } 
    
  if (pos < m_ion.length() && (m_ion[pos] == '^')) {
    m_charge = atoi(nextToken(m_ion, pos + 1, pos, "i/]*, \t\r\n", " \t\r\n").c_str());
  }
    
  if (pos < m_ion.length() && (m_ion[pos] == 'i')) { // SpectraST format, i after charge
    pos++;
  }
   
}
*/

// FragmentIon - creates a FragmentIon struct from the passed in information
FragmentIon::FragmentIon(string ionType, int pos, int loss, double mz, unsigned int ch, unsigned int prominence, unsigned int isotope, double mzDiff, bool bracket) :
  m_mz(mz),
  m_charge(ch),
  m_prominence(prominence),
  m_isotope(isotope),
  m_mzDiff(mzDiff),
  m_bracket(bracket) {
  
  stringstream ss;
  ss << ionType;
  
  if (pos > 0) {
    ss << pos;
  }
  
  if (loss > 0) {
    ss << '-' << loss;
  } else if (loss < 0) {
    ss << '+' << -loss;
  }

  if (ch != 1) {
    ss << '^' << ch;
  }

  m_ion = ss.str();
  m_pos = pos;
  m_ionType=ionType;
  m_loss=loss;
}

string FragmentIon::getAnnotation() {
  stringstream ss;
  ss << m_ion << '/';
  ss.precision(2);
  ss << fixed << m_mzDiff;
  
  if (m_bracket) return ("[" + ss.str() + "]");
  
  return (ss.str());
}

// sortFragmentIonsByProminence - comparison function used by sort() to sort fragment ions by prominence
bool FragmentIon::sortFragmentIonPtrsByProminence(FragmentIon* a, FragmentIon* b) {

  if (a->m_prominence > b->m_prominence) {
    return (true);
  } else if (a->m_prominence < b->m_prominence) {
    return (false);
  } else {
  
    // in case of a tie, annotate with ion of smaller charge first
    if (a->m_charge < b->m_charge) {
      return (true);
    } else if (a->m_charge > b->m_charge) {
      return (false);
    } else {
      // if still tied, sort by ion string
      // this should have the effect that smaller losses will go before larger losses
      return (a->m_ion < b->m_ion);
    }
    
  }
}



// TOKENIZER

// nextToken - returns the next token from the string. Here's how it works, it starts scanning from the
// position 'from', skips over all characters in the string 'skipover', then reads in the token until
// it reaches any character in the string 'delim'. 'tokenEnd' will point to the position AFTER the end
// of the token (i.e. where the delimiter is).  
// 
// Typical usage: consecutive calls of nextToken(s, pos, pos, delim, skipover) will return the tokens in
// the string one by one, starting at position 'pos'.
string Peptide::nextToken(string s, string::size_type from, string::size_type& tokenEnd, const char* delim, const char* skipover) {
	
  if (from == string::npos || from >= s.length()) {
    // already got nothing
    tokenEnd = s.length();
    return ("");
  }
  
  // skips over all characters in the string 'skipover'
  string::size_type tokenStart = s.find_first_not_of(skipover, from);
  
  if (tokenStart == string::npos) {
    // whoa, everything from 'from' onwards are characters to be skipped over.
    // That is, no token exists. For this special case, tokenEnd is set to the end of the string.
    tokenEnd = s.length();
    return ("");
  }
  
  // reads until it reaches a delimiter
  tokenEnd = s.find_first_of(delim, tokenStart);
  if(tokenEnd == string::npos) {
    // token is all the way to the end of s
    tokenEnd = s.length();
    return (s.substr(tokenStart));
  }
  
  return (s.substr(tokenStart, tokenEnd - tokenStart));
}

