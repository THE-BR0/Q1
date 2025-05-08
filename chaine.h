// chaine.h
// Auteur : selim Ben Khatayat
// Version : 5.0
#ifndef CHAINE_H
#define CHAINE_H

#include <vector>
#include "tools.h"
class Chaine {
public:
    Chaine(const tools::S2d& racine, double r_capture);
    Chaine(const Chaine& other);

    Chaine& operator=(Chaine&& other) noexcept;
    void dessin_chaine(Color color) const;

    bool guideEffecteur(const tools::S2d& target, const tools::Cercle& arena);

    // Algorithme pour ajuster les articulations rendu3
    void ajust_articulations(const tools::S2d& target);

  
    void modifier_articulations(int i, const tools::S2d &target);

    void ajouterArticulation(const tools::S2d& articulation) ;

    // Récupérer les articulations
    const std::vector<tools::S2d>& getArticulations() const;
    void clear_articulations();
private:
    tools::S2d racine;
    std::vector<tools::S2d> articulations;
    const double r_capture;
};


#endif // CHAINE_H