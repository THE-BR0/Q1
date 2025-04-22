// mobile.h
// Auteur : theo brochier
// Version : 5.0
#ifndef MOBILE_H
#define MOBILE_H
#include <vector>
#include "tools.h"
namespace mobile {
	/**
	 * Classe de base pour les objets mobiles du jeu
	 * Contient la position et le vecteur vitesse
	 */
	class Mobile {
	public:

		Mobile(tools::S2d p,tools:: Polar v);
		Mobile();
		virtual ~Mobile() ;

		// Getters publics pour les attributs protégés
		virtual const tools::S2d& get_position() const;
		virtual const tools::Polar& get_vitesse() const;

	protected:
		tools::S2d position;
		tools::Polar vecteurVitesse;
	};
	class Particule:public Mobile {
	public:

		Particule(tools::S2d position,tools::Polar vecteurVitesse, double compteur);

		// Getters publics pour les attributs de Mobile (via héritage)
		const tools::S2d& get_position() const override;
		const tools::Polar& get_vitesse() const override;
		unsigned get_compteur() const;


	private:
		unsigned compteur;
	};
	class Faiseur:public Mobile {
	public:
		Faiseur(tools::S2d position_tete,tools:: Polar vecteurVitesse,
															double rayon, int taille);
		Faiseur();
		void initialisation_corps();
		bool collision_element(const Faiseur& autre_faiseur) const;
		const std::vector<tools::Cercle>& get_corps() const { return corps; }

		// Getters publics pour les attributs de Mobile (via héritage) et Faiseur
		const tools::S2d& get_position() const override;
		const tools::Polar& get_vitesse() const override;
		double get_rayon() const;
		int get_taille() const;

	private:
		std::vector<tools::Cercle> corps;
		double rayon;
		int taille;
	};
}

#endif //MOBILE_H
