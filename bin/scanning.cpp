#include "TStopwatch.h"
//#include "TString.h"
#include "TList.h"
#include "TTree.h"
#include "TChain.h"
#include "TFriendElement.h"
#include "../interface/Scan.h"
#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

//#define DEBUG
//#define BKGND
#define SIGNAL

using namespace std;

int main(int argc, char **argv){
	TStopwatch globalClock;
	globalClock.Start();

	//call InitCutVars() first, then InitInputTree(), then InitOutputTree() in scanning.cpp file

#ifdef BKGND
	//string testFileName = "/afs/cern.ch/user/s/skalafut/DoubleElectronHLT_2014/CMSSW_7_3_1_patch2/src/doubleElectronTracklessTrigger/doubleEleTracklessAnalyzer/scanningCode/doc/trial_bkgnd.txt";
	string testFileName = "bkgndfile.txt";
#endif

#ifdef SIGNAL
	string testFileName = "/home/hessko/scanningCode/doc/trial_signal.txt";
#endif
	
	Scan testScan(testFileName);
	testScan.InitCutContainer();
#ifdef DEBUG
	cout<<"\t"<<endl;
	cout<<"there are "<< testScan.numCutVars() <<" CutVar objects in the cutContainer vector"<<endl;
	cout<<"\t"<<endl;
#endif
	
	//declare the primary input chain
	TChain * inputChain;
	TChain * inputFriendChain;

	///declare the chain which has already been run through float runScan() and has a constrained set of cut variables
	///this chain will be passed as an input argument to runScanUsingTupleInput()
	//TChain * templateChain = new TChain("scanned_tree","");
	//templateChain->Add("/afs/cern.ch/work/s/skalafut/public/doubleElectronHLT/tuples_mostRecent/signal/scanned_tuples/scanned_signal_tree_4.root");
	//templateChain->Add("/afs/cern.ch/work/s/skalafut/public/doubleElectronHLT/tuples_mostRecent/signal/scanned_tuples/trimmed_template_scanned_tree.root");

#ifdef BKGND
	inputChain = new TChain("recoAnalyzerTracked/recoTreeBeforeTriggerFiltersTrackedBkgnd","");
	inputChain->Add("/afs/cern.ch/work/s/skalafut/public/doubleElectronHLT/tuples_mostRecent/bkgnd_high_pt/high_pt_bkgnd_analyzer_trees_305.root");
	
	//declare a second input chain which will be added as a friend to the primary input chain
	inputFriendChain = new TChain("recoAnalyzerTrackless/recoTreeBeforeTriggerFiltersTracklessBkgnd","");
	inputFriendChain->Add("/afs/cern.ch/work/s/skalafut/public/doubleElectronHLT/tuples_mostRecent/bkgnd_high_pt/high_pt_bkgnd_analyzer_trees_305.root");
#endif

#ifdef SIGNAL
	inputChain = new TChain("recoAnalyzerMatchedTrackedWithL1Filter/recoTreeBeforeTriggerFiltersMatchedTrackedSignalWithL1Filter","");
	inputChain->Add("/home/hessko/summer15CodingProject/ttree_root_file_for_Kai.root");
	
	//declare a second input chain which will be added as a friend to the primary input chain
	inputFriendChain = new TChain("recoAnalyzerMatchedTracklessWithL1Filter/recoTreeBeforeTriggerFiltersMatchedTracklessSignalWithL1Filter","");
	inputFriendChain->Add("/home/hessko/summer15CodingProject/ttree_root_file_for_Kai.root");
#endif

	string friendName = inputFriendChain->GetName();

#ifdef DEBUG
	cout<<"friend name ="<<"\t"<< friendName<<endl;
#endif

	string friendTreeName = friendName.substr(friendName.find("/")+1);
	inputChain->AddFriend(inputFriendChain,friendName.c_str());
	inputChain->GetListOfFriends()->Print();

	testScan.InitInputNtuple(inputChain);
	TTree * outTree = new TTree("scanned_tree","");
	testScan.InitOutputNtuple(outTree);

#ifdef SIGNAL
	cout<<"about to start running the scan"<<endl;
	testScan.runScan(testScan.numCutVars());
	//testScan.runScanUsingTupleInput(templateChain);
	testScan.SaveOutput("test_scanned_signal_tree.root");
#endif

#ifdef BKGND
	//testScan.runScan(testScan.numCutVars());
	testScan.runScanUsingTupleInput(templateChain);
	testScan.SaveOutput("/afs/cern.ch/work/s/skalafut/public/doubleElectronHLT/tuples_mostRecent/bkgnd_high_pt/scanned_tuples/test_scanned_high_pt_bkgnd_tree_305.root");
#endif
	
	globalClock.Stop();
	globalClock.Print("m");

	
	return 0;


}//end main()


