OUT = projet
CXX = g++
CXXFLAGS = -g -Wall -std=c++17
GTK_CFLAGS = $(shell pkg-config --cflags gtkmm-4.0)
GTK_LIBS = $(shell pkg-config --libs gtkmm-4.0)
CXXFILES = projet.cc gui.cc graphic.cc chaine.cc jeu.cc message.cc mobile.cc tools.cc
OFILES = $(CXXFILES:.cc=.o)

all: $(OUT)

# Fichiers ne dépendant pas de gtkmm
tools.o: tools.cc tools.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

chaine.o: chaine.cc chaine.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

jeu.o: jeu.cc jeu.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

message.o: message.cc message.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

mobile.o: mobile.cc mobile.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Fichiers dépendant de gtkmm
graphic.o: graphic.cc graphic_gui.h graphic.h
	$(CXX) $(CXXFLAGS) $(GTK_CFLAGS) -c $< -o $@

gui.o: gui.cc graphic_gui.h graphic.h gui.h jeu.h tools.h constantes.h
	$(CXX) $(CXXFLAGS) $(GTK_CFLAGS) -c $< -o $@

projet.o: projet.cc gui.h jeu.h graphic.h
	$(CXX) $(CXXFLAGS) $(GTK_CFLAGS) -c $< -o $@

# Règle de liaison (sans GTK_CFLAGS, seulement GTK_LIBS)
$(OUT): $(OFILES)
	$(CXX) $(CXXFLAGS) $(OFILES) -o $@ $(GTK_LIBS)

clean:
	@echo " *** EFFACE MODULES OBJET ET EXECUTABLE ***"
	@/bin/rm -f *.o *.x *.cc~ *.h~ $(OUT)

.PHONY: all clean