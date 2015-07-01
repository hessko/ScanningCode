#include "CutVar.h" 
#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <TTree.h>
#include <TChain.h>

#define OUTPUTNELE 1
#define NELE 400

class Scan{
public:
     //configTxtFile contains names of input tree branches which will be used in optimization (pt, sigmaIEIE, etc),
     //the ranges and step sizes of the threshold cut values, a single character which indicates whether or
     //not the threshold corresponds to an upper bound (like relative ecal iso) or lower bound (pt), and
     //a tag to indicate tracked barrel or endcap which will be used to define the output branch names
     Scan(std::string configTxtFileName): 
	  _configFileName(configTxtFileName){};

     //call InitCutVars() before InitInputTree() and InitOutputTree()
	
     /** initialize the _cutContainer using information from file configFileName_
      */
     void InitCutContainer();

     //this fxn returns the number of CutVar objects in the cutContainer vector 
     unsigned int numCutVars(void){
	  return _cutContainer.size();
     }

     //this fxn initializes the N input TChain pointers, the entries in the inputBranchNamesAndVals
     //map (both the strings and floats), and calls SetBranchAddress using the map entries
     //the vector<string> objects will be filled with information from two txt files which are read
     //by scanning.cpp, and a call to identifyUniqueBranchNames() by a Scan class object
     //the names of the input tuple branches and the names returned by (CutVar object).getCutName()
     //are identical. The CutVar member variable named detectorRegion is used to distinguish
     //tracked barrel, tracked endcap, and trackless endcap.
     //getCutName() gives the name of a cut, and equivalently a branch name
     //getRegion() (part of CutVar class) returns the name of a detector region in the format
     //_tEB, _tEE, or _utEE 
     //void InitInputTuple(std::vector<std::string> pathToInputTuples,std::vector<std::string> inputTupleNames,std::vector<std::string> branchNames);
     void InitInputNtuple(TChain *chain); /// \todo add branches specific for input
     
     void InitOutputNtuple(TTree *tree); /// \todo add branches specific for output
	
		
     //runScan is a recursive function.  It will call itself from within the fxn. The value of iCut
     //passed to this fxn as an input should equal the number of elements in cutContainer. 
     float runScan(unsigned int iCut);

	 void runScanUsingTupleInput(TChain * preScannedTuple);

	 float countEvtsPassing();

     void SaveOutput(std::string pathToOutputFile);


private:
     std::string _configFileName; ///< name of the config file with name of variables to optimize, ranges and steps
     std::vector<CutVar> _cutContainer;	///< vector of the variables to optimize
     std::set<std::string> _branchNames;

     TChain* _pInputChain; ///< only one chain, if multiple chains, they should be added as friends
     TTree *_outputTree;   ///< output tree
    
     Long64_t _nEvents, _nPassing;
     //typedef std::map<std::string, Float_t[NELE]> floatBranchMap_t;
     //typedef std::map<std::string, Int_t[NELE]> intBranchMap_t;
     //typedef std::map<std::string, Float_t[OUTPUTNELE]> altFloatBranchMap_t;
     typedef std::map<std::string, std::vector<Float_t> > floatBranchMap_t;
     typedef std::map<std::string, std::vector<Int_t> > intBranchMap_t;
     typedef std::map<std::string, std::vector<Float_t> > altFloatBranchMap_t;
     
     floatBranchMap_t _inputBranches;
	 intBranchMap_t _inputBranchesInt;

	 //the number of unique keys in _outputBranches will be at least 2 larger than
	 //the number of elements in _cutContainer.
     //one additional key for the number of evts which were analyzed, and another key which 
     //saves the number of evts passing a set of cut values. 
     //These keys will have to be hard coded into this map.
     altFloatBranchMap_t _outputBranches;
	

};//end class scan
