// mobile.cc
// Auteur : theo brochier
// Version : 5.0

#include "mobile.h"
#include "message.h"
#include "constantes.h"
#include "tools.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
using namespace std;
using namespace mobile;
using namespace tools;
static Cercle Arene_ex({0.0,0.0},r_max );
Mobile::Mobile(S2d p,Polar v):position(p),vecteurVitesse(v){}
Mobile::Mobile():position({0.0,0.0}),vecteurVitesse({0.0,0.0}){}

Mobile::~Mobile() {/*ici on pourrais mettre delete ptr*/}

// Implémentation des getters pour Mobile
const S2d& Mobile::get_position() const {
    return position;
}

const Polar& Mobile::get_vitesse() const {
    return vecteurVitesse;
}
void Particule::increment_compteur() {
    compteur++;
}


//-----------------------------------------------------------PARTICULE-------------

Particule::Particule(S2d position, Polar vecteurVitesse, double compteur)
        : Mobile(position, vecteurVitesse),  compteur(static_cast<unsigned>(compteur)) {} // Cast explicite

// Implémentation des getters pour Particule
const S2d& Particule::get_position() const {
    return Mobile::get_position(); // Appel au getter de la classe de base
}

const Polar& Particule::get_vitesse() const {
    return Mobile::get_vitesse(); // Appel au getter de la classe de base
}

unsigned Particule::get_compteur() const {
    return compteur;
}
void Particule::dessin() const{
    dessin_cercle({position,r_viz},CYAN);
    dessin_point(position,GREEN);
}


void Particule::particule_deplacement(const tools::Cercle& arene) {
    S2d nouvelle_position = nextDestination(position, vecteurVitesse);
    
    if (!arene.point_appartient_cercle(nouvelle_position)) {
        rebond(position, vecteurVitesse);
        nouvelle_position = nextDestination(position, vecteurVitesse);
    }
    
    position = nouvelle_position;
}
//------------------------------------FAISEUR-------------------------------------
Faiseur::Faiseur() : rayon(0.0), taille(0) {} 

Faiseur::Faiseur(S2d position,Polar vecteurVitesse, double rayon, int taille)
: Mobile(position,vecteurVitesse), rayon(rayon),taille(taille) {
    if (taille<=0) {
        cout<<message::faiseur_nbe(taille)<<endl;
        exit(EXIT_FAILURE);
        }

    corps.resize(taille);
    if ((rayon>r_max_faiseur)or(rayon<r_min_faiseur)) {
        cout<<message::faiseur_radius(rayon)<<endl;
        exit(EXIT_FAILURE);
    }

}

bool Faiseur::collision_element(const Faiseur& autre_faiseur) const {
    const Cercle tete_this(this->position, this->rayon); 
    const Cercle tete_autre(autre_faiseur.position, autre_faiseur.rayon);
    if (tools::collisionEntreCercles(tete_this, tete_autre)) {
        return true; 
    }

    const auto& corps_autre = autre_faiseur.get_corps(); 
    for (const auto& segment_autre : corps_autre) {
        if (tools::collisionEntreCercles(tete_this, segment_autre)) {
            return true; 
        }
    }
    const auto& corps_this = this->get_corps(); 
    for (const auto& segment_this : corps_this) {
        if (tools::collisionEntreCercles(segment_this, tete_autre)) {
            return true; 
        }
    }
    return false;
}

/*
 * Calcule la prochaine position potentielle en fonction de la position actuelle et du vecteur vitesse.
 * Si la prochaine position est en dehors de l'arène fournie, simule un rebond en ajustant
 * la position et le vecteur vitesse, puis recalcule la prochaine position.
 * Enfin, met à jour la position principale du Faiseur .
 */
void Faiseur::faiseurs_deplacement(const tools::Cercle& arene) {
    S2d nouvelle_position = nextDestination(position, vecteurVitesse);
    
    if (!arene.cercle_appartient_cercle({nouvelle_position, rayon})) {
        rebond(position, vecteurVitesse);
        nouvelle_position = nextDestination(position, vecteurVitesse);
    }
    
    vector<tools::Cercle> anciennes_positions(corps);
    position = nouvelle_position;
    corps[0].change_centre(nouvelle_position);
    for (size_t i (1); i< corps.size(); i++){
        corps[i].change_centre(anciennes_positions[i-1].get_centre());
    }
    
}

//Initialise le corps du Faiseur.
void Faiseur::initialisation_corps() {
    if (taille <= 0) return;

    corps.resize(taille);
    double angle_oppose = vecteurVitesse.theta + M_PI;
    renormalisation(angle_oppose);

    for (size_t i = 0; i < corps.size(); ++i) {
        S2d pos = (i == 0) ? position : corps[i-1].get_centre();
        
        if (i != 0) {
            Polar vector = {vecteurVitesse.r, angle_oppose};
            S2d next_pos = nextDestination(pos, vector);
            
            if (!Arene_ex.cercle_appartient_cercle({next_pos, rayon})) {
                rebond(pos, vector);
                next_pos = nextDestination(pos, vector);
            }
            
            pos = next_pos;
        }
        
        corps[i].change_centre(pos);
        corps[i].change_rayon(rayon);
        
    }
}

const S2d& Faiseur::get_position() const {
    return Mobile::get_position(); 
}

const Polar& Faiseur::get_vitesse() const {
    return Mobile::get_vitesse(); 
}

double Faiseur::get_rayon() const {
    return rayon;
}

int Faiseur::get_taille() const {
    return taille;
}

void Faiseur::dessin() const {
    for(size_t i(0); i<corps.size(); i++){
        dessin_cercle(corps[i],BLUE);
    }
}