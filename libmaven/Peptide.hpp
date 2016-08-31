#ifndef PEPTIDE_HPP_
#define PEPTIDE_HPP_

#include <string>
#include <vector>
#include <map>
#include <set>

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

#define ALL_AA "ABCDEFGHIJKLMNOPQRSTUVXWYZ"

using namespace std;

class FragmentIon {
  
public:  
  
//  Parser-constructor: Not used anywhere at present and not tested, so commented out for now:
//  FragmentIon(double mz, string& annotation, unsigned int curIsotope = 0); 
  
  FragmentIon(string ionType, int position, int loss, double mz, unsigned int ch, unsigned int prom, unsigned int isotope = 0, double mzDiff = 0.0, bool bracket = false);

  string m_ion;
  double m_mz;
  unsigned int m_charge;
  unsigned int m_prominence;
  unsigned int m_isotope;
  double m_mzDiff;
  bool m_bracket;

  int m_pos;
  string m_ionType;
  int m_loss;
  
  string getAnnotation();
  
  static bool sortFragmentIonPtrsByProminence(FragmentIon* a, FragmentIon* b); 
};

class Peptide {
	
public:

  // constructors, destructors and assignment operator
  Peptide(string pep, int ch);
  Peptide(string pep, int ch, string mspModStr);
  Peptide(Peptide& p);
  Peptide& operator=(Peptide& p);  
  ~Peptide();

  // accessor
  string operator[](unsigned int pos) { return interactStyleAA(pos); }
  
  // method to check integrity of peptide
  bool isGood();
  
  // methods to determine equality and homology of two peptide ions
  bool operator==(Peptide& p);
  bool strippedEquals(Peptide& p);
  bool isSubsequence(Peptide& other, bool ignoreMods);
  bool isHomolog(Peptide& other, double threshold, int& identity);

    // methods to set the modification hash
  bool setModByToken(string token, unsigned int pos, char terminus = '0');
  bool setModByType(string modType, unsigned int pos, char terminus = '0');

  
   // methods to print out a string representation of the peptide ion	
  string strippedWithCharge();
  string fullWithCharge();
  string full();
  string mspMods();
  string htmlStyle();
  string interactStyle();
  string interactStyleWithCharge();	
  string interactStyleFullWithCharge();
  string getSafeName();
  string interactStyleAA(unsigned int pos, char terminus = '0');
  string getModToken(char aa, string modName);
  
  // methods to calculate masses and mass-to-charge ratios
  double averageNeutralM();
  double monoisotopicNeutralM();
  double averageMH();
  double monoisotopicMH();
  double averageMZ();
  double monoisotopicMZ();
  double averageMZFragment(char type, unsigned int numAA, unsigned int charge);
  double monoisotopicMZFragment(char type, unsigned int numAA, unsigned int charge);

  // methods to evalulate some basic properties of the peptide
  unsigned int NTT();
  unsigned int NMC();
  unsigned int NAA();
  bool isCleavableICAT();
  bool isUncleavableICAT();
  bool isCAMCysteine();
  bool hasUnmodifiedCysteine();
  bool getAllPresentModTypes(map<string, unsigned int>& presentModTypes); 
  void countAATokens(map<string, unsigned int>& tokenCounts);

  // method to introduce modifications by means of a MSP-style mod string
  bool parseMspModStr(string mspModStr, bool add = false);

  static string nextAAToken(string s, string::size_type start, string::size_type& end);
  
  // method to generate a theoretical spectrum (a la SEQUEST)
  void SEQUESTTheoreticalSpectrum(map<int, float>& peaks);

  // method to create all common fragment ions (for annotation of a peak list)
  void generateFragmentIons(vector<FragmentIon*>& ions, string fragmentationType = "CID");
  void generateFragmentIonsCID(vector<FragmentIon*>& ions);
  void generateFragmentIonsETD(vector<FragmentIon*>& ions);

  // method to shuffle the peptide sequence randomly
  // string shufflePeptideSequence();
  Peptide* shufflePeptideSequence(map<int, set<string> >& allSequences);

  // method to compute the isoelectric point
  double computePI();

  // method to remove a type of mod
  string removeModOfType(string mod);
  
  static bool processNewMod(char aa, double& deltaMass, string& modType, string& userToken);
 
  void permuteModTokens(vector<map<char, set<string> > >& allowableTokens, vector<pair<string, int> >& permutations);
  
  // fields
  int charge;
  char prevAA;
  char nextAA;
  string stripped;
  map<int, string> mods; // int is the mod position. -1 = n-terminal, -2 = c-terminal
  string nTermMod;
  string cTermMod;
  bool isModsSet;
  bool considerIsobaricAASame; // if TRUE, I and L will be considered the same, same for K and Q, etc
  bool hasUnknownMod;
  bool illegalPeptideStr;
  bool illegalMspModStr;

  // static methods to manage and access the mass tables
  static void defaultTables();
  static void deleteTables();  
  static double getAAAverageMass(char aa);
  static double getAAMonoisotopicMass(char aa);
  static double getModAverageMass(string modType);
  static double getModMonoisotopicMass(string modType);
  static double getAAPlusModAverageMass(char aa, string modType);
  static double getAAPlusModMonoisotopicMass(char aa, string modType);
  static double getAATokenAverageMass(string aa);
  static double getAATokenMonoisotopicMass(string aa);
  static void addModTokenToTables(string token, string modType);
  static bool isNTermModType(string modType);
  static bool isCTermModType(string modType);
  static bool isIsobaric(string aa1, string aa2);

  // static members - pointers to the tables of amino acids, modifications and their masses, etc - see below	
  static map<char, double>* AAAverageMassTable;
  static map<char, double>* AAMonoisotopicMassTable;
  static map<string, double>* modAverageMassTable; 
  static map<string, double>* modMonoisotopicMassTable;
  static map<string, string>* modTokenTable;	
  static map<char, double*>* AAMonoisotopicNeutralLossTable;
  static map<string, double*>* modMonoisotopicNeutralLossTable;
  static map<string, double*>* AAMonoisotopicImmoniumTable;
  
  // constants
  static const int NTermPos; // -1
  static const int CTermPos; // -2
  
	
private:
  
  // private parsing methods
  bool stripPeptide(string pep);
  
 static double calcApproximateAverageMass(double monoisotopicMass);
 string nextToken(string s, string::size_type from, string::size_type& tokenEnd, const char* delim = " \t\r\n", const char* skipover = " \t\r\n");

	
};

/* A NOTE ON THE TABLES
 * 
 * The tables must be set for the Peptide class to function properly. Peptide provides a static
 * function for setting up default tables (Peptide::defaultTables()) and deleting them (Peptide::deleteTables()).
 * It is advisable to call Peptide::defaultTables() at the very beginning of your program, and
 * deleteTables() at the very end, although it will still function without those calls (in the
 * default manner) -- When a Peptide object is instantiated for the first time and the tables are not set up, it
 * calls defaultTables() automatically.
 * 
 * If different tables are required, they need to be supplied by the caller --
 * the table pointers are publicly accessible -- but they need to be in the same format as the 
 * default tables. A static method addModTokenToTables() is also provided for adding tokens; this
 * is most commonly used for specifying special symbols for modifications (e.g., M# for M[147]), or 
 * assumed (static) modification for bare amino acid symbols, e.g. C for C[330] (light ICAT).
  
 SOME OTHER IMPORTANT POINTS:
	
 1. If you are supplying the table map objects yourself by assigning to the table pointers,
 take special care that the map objects will not go out of scope at any time while you are calling Peptide
 methods. Only delete them after you're done with using the Peptide class, or better yet, only when the 
 program is finished.
	   
 2. In addition to the usual 20 amino acids, you should also supply 'X' for unknown, 'B' for Asp or Asn, 'Z' for Glu or Gln,
 '!' for water, 'o' for oxygen, 'h' for hydrogen, '+' for a proton, '$' for carbon, 'n' for the N terminus (just a hydrogen)
 and 'c' for the C terminus (OH). 
 
 3. The mod type you give for the mod token in the mod token table MUST BE one of the entries included in the mod mass table. For instance, "M(O)" is mapped to the mod type "Oxidation" in the mod token table, and accordingly, "Oxidation" is mapped to the mass of oxygen in the mod mass table. 
 
 4. If you have a modification that you cannot find in the default, you can modify the code yourself, but do email me hlam@systemsbiology.org about it. If I find the modification to be "mainstream" enough, I'll put it in myself.
	   
*/


#endif /*PEPTIDE_HPP_*/
