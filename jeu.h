// jeu.h
// Auteur : selim Ben Tkhayat
// Version : 5.0
#ifndef JEU_H
#define JEU_H

#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include "mobile.h"
#include "chaine.h"
#include "constantes.h"
#include "tools.h"

enum Status
{
	ONGOING,
	WON,
	LOST
};

class Jeu {
public:
	Jeu();

	Jeu(unsigned int score, std::vector<std::unique_ptr<mobile::Particule>>&& particules,
		std::vector<std::unique_ptr<mobile::Faiseur>>&& faiseurs, Chaine chaine,
		tools::Cercle arene, Mode mode);

	// Lecture et sauvegarde
	bool lecture(const std::string& nomFichier); // Retourne bool
	bool sauvegarder(const std::string& nom_fichier) const; // Mettre à jour la déclaration

	// Autres méthodes publiques
	void reset();
	void set_score(unsigned int score);
	void ajouterFaiseur(const mobile::Faiseur& faiseur);
	double get_score() const {return score;}
	double get_Nb_articulations() const {return chaine.getArticulations().size();}
	double get_Nb_faiseurs() const {return faiseurs.size();}
	double get_Nb_particules() const {return particules.size();}
	std::string get_status ();//a ecrire
	void update();//a ecrire

private:
	// Méthodes auxiliaires pour readFile (maintenant privées)
	bool decodage_score(std::istringstream& data);       // Retourne bool
	bool decodage_particule(std::istringstream& data);   // Retourne bool
	bool decodage_faiseur(std::istringstream& data);     // Retourne bool
	bool decodage_chaine(std::istringstream& data);      // Retourne bool
	bool verifier_collisions_faiseurs() const; // Retourne bool

	// Attributs principaux du jeu
	unsigned int score;
	std::vector<std::unique_ptr<mobile::Particule>> particules; // Correction: Ajouter std::
	std::vector<std::unique_ptr<mobile::Faiseur>> faiseurs;     // Correction: Ajouter std::
	Chaine chaine;
	tools::Cercle arene;
	Mode mode;
};

#endif // JEU_H

