// chaine.cc
// Auteur : selim Ben Khatayat
// Version : 5.0
#include "chaine.h"
#include "tools.h"
#include "mobile.h"
#include <cmath>
#include <iostream>

using namespace std;
using namespace tools;

Chaine::Chaine(const S2d& racine, double r_capture)
    : racine(racine), r_capture(r_capture) {
    // Ajout de la racine comme première articulation
    articulations.push_back(racine);
}
Chaine::Chaine(const Chaine& other)
    : racine(other.racine), articulations(other.articulations),
      r_capture(other.r_capture) {}

Chaine& Chaine::operator=(Chaine&& other) noexcept {
    if (this != &other) {
        this->racine = move(other.racine);
        this->articulations = move(other.articulations);
        other.articulations.clear();
    }
    return *this;
}



void Chaine::ajouterArticulation(const S2d& articulation) {
    articulations.push_back(articulation);
}


const vector<S2d>& Chaine::getArticulations() const {
    return articulations;
}
void Chaine::modifier_articulations(int i,const S2d &target) {
    articulations[i] = target;
}


void Chaine::clear_articulations() {
    articulations.clear();
}

void Chaine::dessin_chaine(Color color)const{
    for ( size_t i(1); i<articulations.size(); i++ ){
        dessin_ligne(articulations[i-1],articulations[i],color );
        dessin_point(articulations[i-1], color);
        // dessin du cercle de capture pour le dernier element
        if (i+1== articulations.size()){
            dessin_cercle({articulations[i],r_capture},color,WHITE);
            dessin_cercle({articulations[i],r_viz},color,WHITE);
        }
    }
}