#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
//this simple class represents a cut variable used in some selection
//this variable has a name, min value, max value, step size, and
//threshold value between the min and max 

class CutVar {
public:
     CutVar(std::string cutName_, std::string zone, float val, float min, float max, float step, bool setUpper):
	  _cutName(cutName_),
	  _detectorRegion(zone),
	  _threshVal(val),
	  _threshStep(step),
	  _isUpperBound(setUpper),
	  _highEff(min),
	  _lowEff(max){
		  _minThresh=std::fmin(_highEff,_lowEff);
		  _maxThresh=std::fmax(_highEff,_lowEff);
	  };

	 bool inRange(){
		 if(_threshVal>=_minThresh && _threshVal<=_maxThresh) return true;
		 return false;
	 }

	 CutVar(std::string cutName_, std::string zone, std::string shortName_):
	  _cutName(cutName_),
	  _detectorRegion(zone),
	  _shortCutName(shortName_){
	  };

     inline void setThresholdValue(float value){ _threshVal = value;};

     void SetValuesFromString(std::string ranges_){
	  char sign;
	  sscanf(ranges_.c_str(), "[%f,%f,%f,%c]", &_highEff, &_lowEff, &_threshStep, &sign);
	  if(sign == '>') _isUpperBound=false;
	  else if(sign == '<') _isUpperBound=true;
	  else exit(1); /// \todo fix launching exception with error
	  _minThresh=std::fmin(_highEff,_lowEff);
	  _maxThresh=std::fmax(_highEff,_lowEff);
	 }

	 /**use this fxn to check that the minimum, maximum, and step size for a CutVar are assigned reasonable
	  * values, whether the CutVar is identified as an upper bound or lower bound, and the detector region
	  * (EB, tracked EE, etc) in which this CutVar is relevant.
	  * the CutVar name is printed first
	  */
     friend std::ostream& operator << (std::ostream& os, const CutVar a){
       
       os <<  a._cutName;
       os << "\t[" << std::setprecision(3) << a._highEff << "," << a._lowEff << "," << a._threshStep << ",";
       char c = a._isUpperBound ? '<' : '>';
       os << c << "]\t" << a._detectorRegion;
	   os << "#"<< a._threshVal << ","<< "minThresh"<< a._minThresh << "," << "maxThresh" << a._maxThresh <<std::endl;
     
       return os;
     }

     inline std::string printNameVal() const{
       char line[250];
       sprintf(line, "%s\t%.2f", _cutName.c_str(), _threshVal);
       return std::string(line);
     }
       
public:
     // _detectorRegion is used to distinguish tracked EB, tracked EE, and trackless EE
	 // _shortCutName contains a string similar to an input branch name, like ecalIsoHltEle, plus
	 // the _detectorRegion. For example, _shortCutName could be ptHltEleutEE (pt in trackless EE) 
     std::string _cutName, _detectorRegion, _shortCutName;
     float _threshVal, _minThresh, _maxThresh, _threshStep, _lowEff, _highEff;
     bool _isUpperBound;	//indicates if this cut will be used as an upper bound (someVal < _threshVal)
};//end class cutVar

