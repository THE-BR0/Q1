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

    /**
     * Opérateur d'affectation par déplacement
     * @param other Chaîne à déplacer
     * @return Référence vers cette chaîne
     */
    Chaine& operator=(Chaine&& other) noexcept;

    /**
         * Déplace l'effecteur vers une cible en respectant les contraintes de l'arène
         * @param target Position cible pour l'effecteur
         * @param arena Arène contraignant le mouvement
         * @return true si le guidage a réussi, false sinon
         */
    bool guideEffecteur(const tools::S2d& target, const tools::Cercle& arena);

    // Algorithme pour ajuster les articulations
    void ajust_articulations(const tools::S2d& target);

    /**
     * Modifie une articulation spécifique en fonction d'une cible
     * @param i Index de l'articulation à modifier
     * @param target Position cible
     */
    void modifier_articulations(int i, const tools::S2d &target);

    void ajouterArticulation(const tools::S2d& articulation) ;

    // Récupérer les articulations
    const std::vector<tools::S2d>& getArticulations() const;
    void affiche() const;
    void clear_articulations();
private:
    tools::S2d racine;
    std::vector<tools::S2d> articulations;
    const double r_capture;
};


#endif // CHAINE_H