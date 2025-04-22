OUT = projet
CXX = g++
CXXFLAGS = -Wall -std=c++17
LINKING = `pkg-config --cflags gtkmm-4.0`
LDLIBS = `pkg-config --libs gtkmm-4.0`
CXXFILES = projet.cc gui.cc graphic.cc chaine.cc jeu.cc message.cc mobile.cc tools.cc
OFILES = $(CXXFILES:.cc=.o)

all: $(OUT)

clean:
	@echo " *** EFFACE MODULES OBJET ET EXECUTABLE ***"
	@/bin/rm -f *.o *.x *.cc~ *.h~ $(OUT)
	
graphic.o: graphic.cc graphic_gui.h graphic.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ 

gui.o: gui.cc graphic_gui.h graphic.h gui.h jeu.h tools.h constantes.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ 

projet.o: projet.cc gui.h jeu.h graphic.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ 


$(OUT): $(OFILES)
	$(CXX) $(CXXFLAGS) $(LINKING) $(OFILES) -o $@ $(LDLIBS)
