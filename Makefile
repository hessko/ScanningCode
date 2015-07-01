
CMDLINEFLAGS=""
CXXFLAGS=-Wall -std=c++11 $(CMDLINEFLAGS) # -pedantic -ansi  #-std=c++0x

ROOT_LIB:=`root-config --libs --glibs`
ROOT_FLAGS:=`root-config --cflags --ldflags`
ROOT_INCLUDE:=`root-config --incdir`

# Boost
#BOOST = /afs/cern.ch/cms/slc5_amd64_gcc434/external/boost/1.47.0
#secondary BOOST=/afs/cern.ch/cms/slc5_amd64_gcc472/external/boost/1.50.0
#trial BOOST = /afs/cern.ch/cms/slc6_amd64_gcc481/external/boost/1.51.0
#SCRAMTOOL=$(shell which scram &>/dev/null || echo 1)
#
#ifeq ($(SCRAMTOOL),1)
#	ROOFIT_LIB="-lRooFit"
#	ROOSTAT_LIB="-lRooStats"
#	ROOFIT_INCLUDE="./"
#else
#	ROOFIT_INCLUDE := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep INCLUDE= | sed 's|INCLUDE=||')
#	ROOFIT_LIB := -l$(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIB= | sed 's|LIB=||')
#	ROOFIT_LIB += -l$(shell cd $(CMSSW_BASE); scram tool info roofit | grep LIB= | sed 's|LIB=||')
#	ROOFIT_LIBDIR = -L$(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIBDIR= | sed 's|LIBDIR=||')
#	ROOFIT_LIB+=$(ROOFIT_LIBDIR)
#endif
#ROOSTAT_LIB="-lRooStats"


INCLUDEDIR=./interface
SRCDIR=./src
BUILDDIR=./bin
OBJ_DIR=./lib

#EoPDir:=../EOverPCalibration
#EoPNtupleDir:=../NtuplePackage

#################
#INCLUDE=-I$(INCLUDEDIR) -I$(INCLUDEEPDIR) -isystem$(ROOT_INCLUDE) -I$(ROOT_INCLUDE)  -I$(ROOFIT_INCLUDE) -I$(BOOST)/include
#LIB=-L$(BOOST)/lib -L/usr/lib64 # -L/usr/lib 

INCLUDE=-I$(INCLUDEDIR) -I$(INCLUDEEPDIR) -isystem$(ROOT_INCLUDE) -I$(ROOT_INCLUDE)
LIB=-L/usr/lib # -L/usr/lib 



#### Make the list of modules from the list of .cc files in the SRC directory
MODULES=$(shell ls $(SRCDIR)/*.cc | sed "s|.cc|.o|;s|$(SRCDIR)|$(OBJ_DIR)|g")
MODULESEoP=$(shell ls $(EoPDir)/$(SRCDIR)/*.cc | sed "s|.cc|.o|;s|$(SRCDIR)|$(OBJ_DIR)|g")
MODULESEoPNtuples=$(shell ls $(EoPNtupleDir)/$(SRCDIR)/*.cc | sed "s|.cc|.o|;s|$(SRCDIR)|$(OBJ_DIR)|g")
#### Make the list of dependencies for a particular module

default: $(MODULES)  $(BUILDDIR)/scanning.exe

#------------------------------ MODULES (static libraries)

MAKEDEPEND = -MMD  -MT '$@ lib/$*.d'

# $<: first prerequisite -> put always the .cc as first 
#### General rule for classes (libraries) compiled statically
### Generate also a .d file with prerequisites
#@$(COMPILE.cc) $(CXXFLAGS) $(INCLUDE) $(MAKEDEPEND) -o $@ $<
	
lib/%.o: $(SRCDIR)/%.cc
	@echo "--> Making $@"
	@echo "compile = " $(COMPILE.cc)
	@echo "root_lib = " $(ROOT_LIB)
	@echo "sys lib = " $(LIB)
	#@g++ -v -c $< -Wall -std=c++0x $(INCLUDE) $(MAKEDEPEND) -o $@ 
	@g++ -c $< -Wall -g -std=c++0x $(INCLUDE) $(ROOT_LIB) $(LIB) $(MAKEDEPEND) -o $@ 




-include $(MODULES:.o=.d)


# $(OBJ_DIR)/setTDRStyle.o: $(SRC)/setTDRStyle.C $(INCLUDEDIR)/setTDRStyle.hh
# 	@g++ $(OPT) $(INCLUDE) -c -o $(OBJ_DIR)/setTDRStyle.o $(SRC)/setTDRStyle.C 


###### Main program
# ZFitter.exe: $(BUILDDIR)/ZFitter.exe
# $(BUILDDIR)/ZFitter.exe:  $(BUILDDIR)/ZFitter.cpp 
# 	cd $(EoPDir) && make
# 	cd $(EoPNtupleDir) && make
# 	@echo "---> Making ZFitter $(COMPILE.exe)"
# 	@g++ $(CXXFLAGS) $(INCLUDE) $(MAKEDEPEND) -o $@ $< $(MODULES) $(MODULESEoP) $(MODULESEoPNtuples) $(LIB) $(ROOT_LIB) $(ROOFIT_LIB) $(ROOSTAT_LIB) $(ROOT_FLAGS) \
# 	-lboost_program_options -lTreePlayer CXXFLAGS=-Wall # -pedantic -ansi  #-std=c++0x


#build Scan.exe from the files listed to the right of the colon
scanning.exe: $(BUILDDIR)/scanning.cpp

$(BUILDDIR)/scanning.exe:  $(BUILDDIR)/scanning.cpp 
	@echo "---> Making scanning $(COMPILE.exe)"
	@echo "cxxflags = " $(CXXFLAGS)
#	@g++ $(CXXFLAGS) $(INCLUDE) $(MAKEDEPEND) -o $@ $< $(MODULES) $(LIB) $(ROOT_LIB) $(ROOFIT_LIB) $(ROOSTAT_LIB) $(ROOT_FLAGS)
	@g++ -Wall -std=c++0x $(INCLUDE) $(MAKEDEPEND) -o $@ $< $(MODULES) $(LIB) $(ROOT_LIB) $(ROOFIT_LIB) $(ROOSTAT_LIB) $(ROOT_FLAGS) #\
	
	#-lboost_program_options -lTreePlayer 




clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.d
	rm -f $(BUILDDIR)/*.exe


