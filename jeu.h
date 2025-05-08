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
	bool faux_reset();
	// getteurs
	int get_score() const {
		int a(score);
		return a;}
	int get_Nb_articulations() const {return chaine.getArticulations().size();}
	int get_Nb_faiseurs() const {return faiseurs.size();}
	int get_Nb_particules() const {return particules.size();}
	std::string get_status ()const;
	Mode getMode() const {return mode;}
	

	void reset();
	void set_score(unsigned int score);
	void set_status(Status new_status);
	void ajouterFaiseur(const mobile::Faiseur& faiseur);
	void update();
	void dessiner() const;
	void decomposer_particule(size_t index);
	void col_fais(const tools::Cercle& tete_i, const tools::Cercle& tete_j, 
                  std::vector<bool>& faiseur_doit_sarreter, 
                  size_t i, size_t j);

private:
	// Méthodes auxiliaires pour readFile (maintenant privées)
	bool decodage_score(std::istringstream& data);       
	bool decodage_particule(std::istringstream& data);   
	bool decodage_faiseur(std::istringstream& data);     
	bool decodage_chaine(std::istringstream& data);      
	bool verifier_collisions_faiseurs() const; 

	// Attributs principaux du jeu
	unsigned int score;
	std::vector<std::unique_ptr<mobile::Particule>> particules; // Correction: Ajouter std::
	std::vector<std::unique_ptr<mobile::Faiseur>> faiseurs;     // Correction: Ajouter std::
	Chaine chaine;
	tools::Cercle arene;
	Mode mode;
	Status statut;
};

#endif // JEU_H
