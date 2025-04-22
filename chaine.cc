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


// Récupérer la liste des articulations
const vector<S2d>& Chaine::getArticulations() const {
    return articulations;
}
void Chaine::modifier_articulations(int i,const S2d &target) {
    articulations[i] = target;
}

// Afficher les articulations
void Chaine::affiche() const {
    cout << "Articulations de la chaîne :" << endl;
    for (size_t i = 0; i < articulations.size(); ++i) {
        cout << " (" << articulations[i].x << ", " << articulations[i].y << ")" ;
    }
}
void Chaine::clear_articulations() {
    articulations.clear();
}

