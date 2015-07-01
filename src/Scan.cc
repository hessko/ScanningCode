#include "../interface/Scan.h"
#include "TTree.h"
#include "TChain.h"
#include "TFriendElement.h"
#include "TList.h"
#include "TCollection.h"
#include "TFile.h"
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <cmath>
#include <map>
#include <array>
#include <set>
#include <cassert>
#include <vector>

#define NELE 400
#define OUTPUTNELE 1
#define ETABRANCHNAME "etaHltEle"
#define PHIBRANCHNAME "phiHltEle"
#define PTBRANCHNAME "ptHltEle"
#define NUMELEBRANCHNAME "nHltEle"
//#define DEBUG
//#define DEBUG2
//#define DEBUG3
//#define SHORTTEST

using namespace std;

void Scan::InitCutContainer(){
	ifstream confStrm(_configFileName);
	//cout<<"in InitCutContainer"<<endl;

	while(confStrm.peek() != EOF && confStrm.good()){ /// \todo check if the file is valid
		if(confStrm.peek() == 35){ // 35 = #
			confStrm.ignore(1000,'\n'); // ignore the rest of the line until \n
			continue;
		}
		string branchName, ranges, region;

		///this works much better than getLine()
		confStrm >> branchName >> ranges >> region; // read and put into strings
		if(branchName.empty()) break;
		//cout<<"ranges: \t"<< ranges << endl;

		//cout<<"about to add a branchName to the set of strings named _branchName"<<endl;
		_branchNames.insert(branchName);
		string outputBranchName = branchName.substr(branchName.find(".")+1)+region;

		CutVar cutObject(branchName,region,outputBranchName);
		cutObject.SetValuesFromString(ranges); // the parsing of the string is implemented in CutVar

#ifdef DEBUG3
		//use this to check the range of values assigned to each cut variable, and its status as an upper or lower bound
		cout << cutObject << endl;
#endif
		_cutContainer.push_back(cutObject);
		for(unsigned int i=0; i < NELE; i++){
			//_inputBranches[branchName][i]=0;
			_inputBranches[branchName].push_back(0);	
		}
		for(unsigned int i=0; i < OUTPUTNELE; i++){
			//_outputBranches[outputBranchName][i]=0;
			_outputBranches[outputBranchName].push_back(0);
		}


	}//end while
	cout<<"\t"<<endl;
	cout<<"------------------------------------------------------------------------"<<endl;
	cout<<"\t"<<endl;
	return;
}//end InitCutContainer()



void Scan::InitInputNtuple(TChain *chain){
#ifdef DEBUG
	cout<<"in InitInputNtuple() fxn"<<endl;
#endif
	_pInputChain = chain;
	string chainName = chain->GetName();

	for(unsigned int h=0;h<NELE;h++){
		//add phi,eta, and nHltEle tree.branch names from _pInputChain to _inputBranches and _inputBranchesInt maps 
		//_inputBranches[chainName+"."+PHIBRANCHNAME][h]=-1;
		//_inputBranches[chainName+"."+ETABRANCHNAME][h]=100;
		//_inputBranchesInt[chainName+"."+NUMELEBRANCHNAME][h]=-1;
		_inputBranches[chainName+"."+PHIBRANCHNAME].push_back(1);
		_inputBranches[chainName+"."+ETABRANCHNAME].push_back(100);
		_inputBranchesInt[chainName+"."+NUMELEBRANCHNAME].push_back(-1);
	}//end loop which initializes array values in additional input branches

	TList *friends = chain->GetListOfFriends();
	TIter newfriend_itr(friends);
	for(TFriendElement *friendElement = (TFriendElement*) newfriend_itr.Next();
			friendElement != NULL; friendElement = (TFriendElement*) newfriend_itr.Next()){
		std::string treeName=friendElement->GetTreeName();
		//add phi,eta, and nHltEle tree.branch names from friends of _pInputChain
		//to _inputBranches and _inputBranchesInt maps 
	
		for(unsigned int h=0;h<NELE;h++){
			//_inputBranches[treeName+"."+PHIBRANCHNAME][h]=-1;
			//_inputBranches[treeName+"."+ETABRANCHNAME][h]=100;
			//_inputBranchesInt[treeName+"."+NUMELEBRANCHNAME][h]=-1;
			_inputBranches[treeName+"."+PHIBRANCHNAME].push_back(-1);
			_inputBranches[treeName+"."+ETABRANCHNAME].push_back(100);
			_inputBranchesInt[treeName+"."+NUMELEBRANCHNAME].push_back(-1);
	
		}//end loop which initializes array values to additional input branches tied to friend chains

	}//end loop over TFriendElements

	for(intBranchMap_t::iterator mapIt = _inputBranchesInt.begin(); mapIt!=_inputBranchesInt.end(); mapIt++){
		string brName = mapIt->first;
		if(brName.find(chainName)!=string::npos) brName = brName.substr(brName.find(".")+1);
#ifdef DEBUG
		if(brName.compare(mapIt->first)!=0) cout<<"brName has been shortened to"<<"\t"<<brName<<endl;
#endif
		//now brName will access the correct branch (of the main tree, or one of its friends), and the
		//variable assigned to this branch comes from _inputBranchesInt 
		int iRead=_pInputChain->SetBranchAddress(brName.c_str(),&((mapIt->second)[0]));
#ifdef DEBUG
		if(iRead!=0) cout<<"branch named"<<"\t"<<brName<<"\t"<<"is no good"<<"\t"<<"exit status ="<<"\t"<< iRead <<endl;
#endif
	}

#ifdef DEBUG
	cout<<"initialized Int_t array branches to main input chain and its friends"<<endl;
#endif

	for(floatBranchMap_t::iterator mapIt = _inputBranches.begin(); mapIt!=_inputBranches.end(); mapIt++){
		string brName = mapIt->first;
		if(brName.find(chainName)!=string::npos) brName = brName.substr((brName).find(".")+1);
#ifdef DEBUG
		if(brName.compare(mapIt->first)!=0) cout<<"brName has been shortened to"<<"\t"<<brName<<endl;
#endif
		int readTwo=_pInputChain->SetBranchAddress(brName.c_str(),&((mapIt->second)[0]));
		if(readTwo!=0) cout<<"branch named\t"<<brName<<"\t is no good \t"<<"exit status =\t"<< readTwo<<endl;
	}//end loop over map elements

#ifdef DEBUG
	cout<<"initialized Float_t array branches to main input chain and its friends"<<endl;
	cout<<"\t"<<endl;
#endif
	return;
}

void Scan::InitOutputNtuple(TTree *tree){
#ifdef DEBUG
	cout<<"\t"<<endl;
	cout<<"in InitOutputNtuple() method"<<endl;
#endif
	_outputTree = tree;

	//use _detectorRegion
	for(altFloatBranchMap_t::iterator mapIt = _outputBranches.begin(); mapIt!=_outputBranches.end(); mapIt++){
		_outputTree->Branch((mapIt->first).c_str(),&(mapIt->second), (mapIt->first+"["+to_string(OUTPUTNELE)+"]/F").c_str());
	}//end loop over _outputBranches map elements

	// set additional branches
	//add two additional float branches to count the number of events analyzed, and
	//the number of evts which passed all cuts
	_outputTree->Branch("_nEvents", &_nEvents, "_nEvents/l");
	_outputTree->Branch("_nPassing", &_nPassing, "_nPassing/l");
#ifdef DEBUG	
	cout<<"initialized all branches to output tree"<<endl;
	cout<<"\t"<<endl;
#endif
	return;
}

void Scan::SaveOutput(string pathToOutputFile){
#ifdef DEBUG
	cout<<"\t"<<endl;
	cout<<"in SaveOutput() method"<<endl;
#endif
	TFile * outTupleFile = new TFile(pathToOutputFile.c_str(),"recreate");
	//TFile * outTupleFile = new TFile();
	outTupleFile->cd();
	_outputTree->Scan("","","",10);
	_outputTree->Write();
	outTupleFile->Close();
#ifdef DEBUG
	cout<<"wrote output tree to file"<<endl;
	cout<<"\t"<<endl;
#endif
	return;
}

/**
   Recursive method.
   @param[in] iCut number of remaining variables to loop over. if iCut==0 then end
   if isSignal = true, then use the Scan object member variable cutEfficiency to move to the
   next set of cuts (looser)  
 */
float Scan::runScan(unsigned int iCut){
	//scan over all of the variables in _cutContainer, and for each variable loop over all possible
	//values of the threshold.  For each variable threshold value, loop over all input events
	//and count how many pass the current threshold values of all variables in _cutContainer.

	//once iCut equals zero all of the cut thresholds have been set to new values
	//using these new threshold values I should loop over all events in the input tuple
	//and count how many pass the set of cuts.  This event count, the values of
	//every cut threshold, and the number of input evts analyzed should be written
	//to the output tuple by calling outputTree->Fill().  This tree should then be saved
	//to the output file by calling outputTree->Write().  Finally, I should return control
	//to the main program.
	if(iCut==0){
		float retVal = countEvtsPassing();
		cout<<"finished scan over all evts with one set of unique cut values"<<endl;
		return retVal;
	}//end if(iCut==0)

#ifdef DEBUG
	std::cout << _cutContainer.size() << "\t" <<"elements in _cutContainer"<<"\t"<< iCut << std::endl;
#endif
	// if iCut!=0 call this method varying the cut value
	CutVar& currentCut = _cutContainer[iCut-1];

	for(currentCut._threshVal = currentCut._highEff; 
			currentCut.inRange(); 
			currentCut._threshVal+=currentCut._threshStep){
		// all CutVar member vars (like _threshVal and _threshStep) are public
		// in this loop _threshVal is initialized to _minThresh, then incremented by _threshStep
		// until _maxThresh is reached 
		// The number of iterations which are executed within this for loop has
		// nothing to do with the number of CutVar objects stored in _cutContainer!
		//
		// now set the value of the variable in _outputBranches[_shortCutName]
		// and use some recursive magic!
		cout<< currentCut << endl;
		//_outputBranches[currentCut._shortCutName][0] = currentCut._threshVal;
		_outputBranches[currentCut._shortCutName].push_back(currentCut._threshVal);	
		if(runScan(iCut-1) < 0.5) break;
	}//end loop over all possible values of the cut threshold for a CutVar object in _cutContainer

	return 1;

}//end runScan()

///this method counts the number of evts passing different sets of cuts
///the cuts are stored in _cutContainer
float Scan::countEvtsPassing(){

#ifdef DEBUG
	cout<<"\t"<<endl;
	cout<<"in iCut==0 portion of runScan() method"<<endl;
	cout<<"\t"<<endl;
	cout<<"about to call GetEntries() on main input chain"<<endl;
#endif

	_nPassing=0;
	_nEvents=0;
	//loop over entries in tuple
	cout << "_pInputChain is " << _pInputChain->GetName() << endl;
	Long64_t maxEntries = _pInputChain->GetEntries();
	cout<<"maxEntries: \t" << maxEntries <<endl;
#ifdef SHORTTEST
	maxEntries = 10;
	//	cout<<"running short test, scanning over "<< maxEntries<<" entries in tuple"<<endl;
#endif

	for(Long64_t evt = 0; evt<maxEntries; evt++){
#ifdef DEBUG
		if(evt>=0) cout<<"about to call GetEntry() on main input chain"<<endl;
		if(evt>=0) cout<<"on Tree entry number"<<"\t"<<evt<<endl;
#endif

		_pInputChain->GetEntry(evt);

		if (evt%10000 == 0) cout<<"reached evt \t"<< evt <<"\t in the scan"<<endl;
	
#ifdef DEBUG
		if(evt>=0) cout<<"called GetEntry() on main input chain"<<endl;
#endif

		bool passing=true;

		map<string, set<int> > passingEleTree;
		TList *friends = _pInputChain->GetListOfFriends();
		TIter newfriend_itr(friends);
#ifdef DEBUG
		if(evt>=0) cout<<"called GetListOfFriends() on main input chain"<<endl;
#endif

		string mainChainName = _pInputChain->GetName();
#ifdef DEBUG
		cout << "mainChainName is: " << mainChainName << endl;
		cout << "inputBranchesInt Iterator->first: " << _inputBranchesInt.begin()->first  <<endl;
		cout << "inputBranchesInt Iterator->second.size(): " << (_inputBranchesInt.begin()->second).size()  <<endl;
#endif



		//count number of reco objects made in this evt in the primary input chain
		set<int> mainPassingEle;
#ifdef DEBUG
		cout<<"TEST BEGIN"<<endl;
		cout<< "pt of ele 0 in chain named \t"<< mainChainName <<"\t is \t"<<  _inputBranches[mainChainName+".ptHltEle"][0] << endl;
		cout<< "eta of ele 0 in chain named \t"<< mainChainName <<"\t is \t"<<  _inputBranches[mainChainName+".etaHltEle"][0] << endl;
		cout<< _inputBranchesInt[mainChainName+".nHltEle"].size() <<endl;
		cout<<"TEST END"<<endl;
#endif
		for(unsigned int q=0; q <_inputBranchesInt[mainChainName+".nHltEle"][0]; q++){
			mainPassingEle.insert(q);
		}
		passingEleTree[mainChainName] = mainPassingEle;
#ifdef DEBUG
		if(evt>=0) cout<<"mainChainName="<<"\t"<<mainChainName<<endl;
		if(evt>=0) cout<<"num reco eles in main chain="<<"\t"<<_inputBranchesInt[mainChainName+".nHltEle"][0]<<endl;
#endif

		// loop over friends and count the total number of reco objects made in TTree entry number evt
		for(TFriendElement *friendElement = (TFriendElement*) newfriend_itr.Next();
				friendElement != NULL; friendElement = (TFriendElement*) newfriend_itr.Next()){
			string treeName=friendElement->GetTreeName();
#ifdef DEBUG
			if(evt>=0) cout<<"friendElement treeName = "<< treeName <<endl;
			if(evt>=0) cout<<"num reco eles in friend="<<"\t"<< _inputBranchesInt[treeName+".nHltEle"][0]<<endl;
#endif

			set<int> passingEle;
			for(unsigned int iEle =0; iEle < _inputBranchesInt[treeName+".nHltEle"][0]; iEle++){
				passingEle.insert(iEle);
			}
			passingEleTree[treeName]=passingEle;
		}
		bool recoOk=true;
		for(map<string,set<int> >::const_iterator mapIt=passingEleTree.begin(); mapIt!=passingEleTree.end(); mapIt++){
			if(mapIt->second.size()==0){
#ifdef DEBUG2
				cout<<mapIt->first<<"\t"<<mapIt->second.size()<<endl;
#endif
				recoOk=false;
			}	
		}
		if(!recoOk) continue;	//go to the next evt if no particle is reconstructed
		if(recoOk) _nEvents++;
#ifdef DEBUG2
		cout<<"-----------------------------------------------------"<<endl;
#endif	
		// now you have the map passingEleTree filled with all possible electrons (indexes) from both legs

		for(vector<CutVar>::const_iterator cut_itr = _cutContainer.begin();
				cut_itr != _cutContainer.end(); cut_itr++){

#ifdef DEBUG
			if(evt>=0) cout<<"\t"<<endl;
			if(evt>=0) cout<<"about to apply cut"<<"\t"<<*cut_itr<<endl;
#endif
			string cutName= cut_itr->_cutName;	//here _cutName = treeName.branchName
			size_t dotPos = cutName.find(".");

			string treeName = cutName.substr(0,dotPos);	//treeName listed before dot in cutName
			string branchName = cutName.substr(dotPos+1); 	//branchName listed after dot in cutName
#ifdef DEBUG
			if(evt>=0) cout<<"treeName="<<"\t"<<treeName<<endl;
			if(evt>=0) cout<<"branchName="<<"\t"<<branchName<<endl;
#endif

			set<int>& passingEle = passingEleTree[treeName];
			unsigned int initialSize = passingEleTree[treeName].size();

#ifdef DEBUG
			if(evt>=0) cout<<"size of passingEleTree with key \t"<< treeName <<"\t=\t"<< passingEle.size() <<endl;
			if(evt>=0) cout<<"about to start looping over a set of ints store in passingEleTree"<<endl;
			if(evt>=0) cout<<"passingEleTree[treeName] has this many elements:"<<"\t"<<passingEleTree[treeName].size()<<endl;
#endif

			for(set<int>::iterator iEle_itr=passingEle.begin(); (iEle_itr!= passingEle.end() ) && !passingEle.empty(); ){
				// there is no CutVar tied to the eta branch (we are not optimizing the eta range of the selection)
				// so the treeName + eta branch name must be used to access eta values in _inputBranches

#ifdef DEBUG
				if(evt>=0) cout<<"\t"<<endl;
				if(evt>=0) cout<<"in loop over passingEle elements"<<endl;
#endif
				string fullEtaBrName = treeName;
				fullEtaBrName.append(".");
				fullEtaBrName.append(ETABRANCHNAME);
#ifdef DEBUG
				if(evt>=0) cout<<"reco eta =\t"<< _inputBranches[fullEtaBrName][*iEle_itr] <<endl;
#endif

				//if(_inputBranches[fullEtaBrName][*iEle_itr] == 100) continue;
				//leave this loop over passingEle elements once the eta value in _inputBranches[fullEtaBrName][*iEle_itr]
				//is greater than 5.0 or less than -5.0
				//this will happen because the array of Float_t values at _inputBranches[fullEtaBrName] has 400 entries
				//or whatever NELE is set to
				//assert(fabs(_inputBranches[fullEtaBrName][*iEle_itr]) < 5.0);

				if(fabs(_inputBranches[fullEtaBrName][*iEle_itr]) < 2.5 && (cut_itr->_detectorRegion).compare("utEE")==0){
					iEle_itr++;
					continue;
				}//end trackless eta filter
				if( (fabs(_inputBranches[fullEtaBrName][*iEle_itr]) < 1.479 || fabs(_inputBranches[fullEtaBrName][*iEle_itr]) >= 2.5) && (cut_itr->_detectorRegion).compare("tEE")==0 ){
					iEle_itr++;
					continue;
				}//end tracked EE eta filter
				if(fabs(_inputBranches[fullEtaBrName][*iEle_itr]) > 1.479 && (cut_itr->_detectorRegion).compare("EB")==0){
					iEle_itr++;
					continue;
				}//end tracked EB eta filter
				// here you have one electron matching the _detectorRegion

#ifdef DEBUG
				if(evt>=0) cout<<"have an electron which passes eta requirement"<<endl;
				if(evt>=0) cout<<"about to apply this cut:"<<endl;
				if(evt>=0) cout<<*cut_itr<<endl;
				if(evt>=0) cout<<"\t"<<endl;
#endif

				// now test if the electron passes the cut 
				bool p=true;
				if(cut_itr->_isUpperBound){ 
					if(_inputBranches[cutName][*iEle_itr]>cut_itr->_threshVal) p=false;
				} else{
					if(_inputBranches[cutName][*iEle_itr]<cut_itr->_threshVal) p=false;
				}

#ifdef DEBUG
				if((evt>=0) && p) cout<<"reco electron passed cut"<<endl;
				if((evt>=0) && !p) cout<<"reco electron failed cut"<<endl;
#endif

				if(p) iEle_itr++; 
				else{
#ifdef DEBUG
					cout<<"an element should be erased from passingEleTree"<<endl;
#endif
					passingEle.erase(iEle_itr);	//remove this element if it fails the cut
					iEle_itr++;
#ifdef DEBUG
					if(iEle_itr != passingEle.end() ){
						cout<<"after calling erase, iEle_itr points to an element with eta= "<< _inputBranches[fullEtaBrName][*iEle_itr] <<endl;
					}
#endif
				}
			}//end loop over integers in passingEle set
			//now reassign the set passingEle to the map element passingEleTree[treeName]
			//passingEleTree[treeName] = ;
#ifdef DEBUG
			if(evt>=0){
				cout<<"left loop over integers in passingEle set"<<endl;
				cout<<"after applying a cut"<<endl;
				cout<<"passingEleTree[treeName] has this many elements:"<<"\t"<<passingEleTree[treeName].size()<<endl;
				cout<<"\t"<<endl;
			}
#endif
			if(initialSize > 0 && passingEleTree[treeName].size() == 0){
				passing = false;
				break;	//leave loop over CutVar objects in _cutContainer
			}
		}//end loop over CutVar objects in _cutContainer

		if(passing) _nPassing++;
	}//end loop over evts in TChain

	/*
	   for(vector<CutVar>::const_iterator cut_itr = _cutContainer.begin();
	   cut_itr != _cutContainer.end(); cut_itr++){
	   cout << cut_itr->printNameVal() << endl;
	   }
	   */

	_outputTree->Fill();
	//cout<<"filled _outputTree, leaving runScan() method"<<endl;
	return (float)(_nPassing)/_nEvents;
}///end countEvtsPassing()

/**this function runs a scan with the CutVar value ranges set by a tuple previously outputted by float runScan()
 * this tuple only contains sets of cut values which pass at least 50% of the matched signal evts.
 * The main work done solely by this function is reading in sets of cut values from the input tuple, and
 * updating _threshVal for each CutVar object in _cutContainer with these new sets of cut values. 
 * Everything else is handled by countEvtsPassing()
 *
 * NOTE the input txt config file is still very useful here!
 */
void Scan::runScanUsingTupleInput(TChain * preScannedTuple){
	///each entry in preScannedTuple contains one unique set of cuts
	///read in each entry, and save the cuts in _outputBranches (a map<string,Float_t[1]> object) 
	///then use the map entries to update the _threshVal values in _cutContainer
	///NOTE the string keys used in _outputBranches are identical to the _shortCutName var in CutVar objects
	for(altFloatBranchMap_t::iterator oBrIt=_outputBranches.begin(); oBrIt!=_outputBranches.end(); oBrIt++){
		preScannedTuple->SetBranchAddress((oBrIt->first).c_str(),&(oBrIt->second));
	}///end loop to link branches in input tuple to numerical variables in _outputBranches

	///now when preScannedTuple->GetEntry(evt) is called, the cut values in that particular evtNum will be placed
	///in _outputBranches
	for(long evt=0; evt<preScannedTuple->GetEntries(); evt++){
		preScannedTuple->GetEntry(evt);
		for(vector<CutVar>::iterator cutItr=_cutContainer.begin(); cutItr!=_cutContainer.end(); cutItr++){
			///now update the _threshVal of each CutVar object in _cutContainer using the values in
			///_outputBranches[_shortCutName][0]
#ifdef DEBUG
			cout<<"_outputBranches element 0 with key \t"<< cutItr->_shortCutName <<"\t = \t"<< _outputBranches[cutItr->_shortCutName][0] <<endl;
#endif
			cutItr->_threshVal = _outputBranches[cutItr->_shortCutName][0];
		}///end loop over CutVar objects stored in _cutContainer
		
		///now that all of the CutVar _threshVal values have been updated, count how many evts in _inputChain pass the set of cuts
		countEvtsPassing();
	}///end loop over entries in preScannedTuple

}///end runScanUsingTupleInput()


