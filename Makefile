.PHONY: all clean

PROG=DIPFinal
CXX=g++
RM=rm

SRC=ViewPointSimulation.cpp UnveilinWaves.cpp 
INC=opencvheader.h ViewPointSimulation.h UnveilinWaves.h 
OBJ=$(SRC:.cpp=.o)
#LFLAGS=-lopencv_core220 -lopencv_imgproc220 -lopencv_calib3d220 -lopencv_video220 -lopencv_features2d220 -lopencv_ml220 -lopencv_highgui220 -lopencv_objdetect220 -lopencv_contrib220 -lopencv_legacy220
LFLAGS=-lcv200 -lcvaux200 -lcxcore200 -lcxts200 -lhighgui200 -lml200

CV_INC_PATH="C:\OpenCV\include"
CV_LIB_PATH="C:\OpenCV\lib"


all: $(PROG) $(OBJ)


$(PROG): main.cpp $(OBJ)
	$(CXX) -I$(CV_INC_PATH) -O3 main.cpp $(OBJ) -L$(CV_LIB_PATH) $(LFLAGS)  -o $@
	$(RM) -r $(OBJ)


ViewPointSimulation.o: ViewPointSimulation.cpp ViewPointSimulation.h opencvheader.h
	$(CXX) -I$(CV_INC_PATH) -c $<

UnveilinWaves.o: UnveilinWaves.cpp UnveilinWaves.h opencvheader.h
	$(CXX) -I$(CV_INC_PATH) -c $< 



clean:
#	@$(RM) -r $(OBJ)
	@$(RM) -r $(PROG) $(PROG).exe
